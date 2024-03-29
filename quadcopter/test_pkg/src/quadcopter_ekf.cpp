#include <iostream>
#include <stdio.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <tf2_msgs/TFMessage.h>
#include "geometry_msgs/TransformStamped.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include <geometry_msgs/PoseStamped.h>
#include "ardrone_autonomy/Navdata.h"
#include <tf/tf.h>
#include <fstream>
#include <math.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "eigen/Eigen/Dense"
#include <vector>
#include <stdlib.h>
#include "std_msgs/String.h"
#include <std_msgs/Float64.h>
#include <sstream>
/* -----------------------------------------------------------------------------------------
CREDIT
Credit to
Eigen for their opensource linear algebra library and headers
Allen for previous iteration of Kalman filter
ROS opensource
*/
/*
Notes for meeting-
-Genesis
*/
/*
Changelog
-----------------------------------------------------------------------------------------
BEGIN
*/
using namespace std;
using namespace Eigen;
using Eigen::MatrixXd;

// Declare matrixes used in the Kalman Filter
Matrix4f Q= Matrix4f::Zero();
Matrix4f R= Matrix4f::Zero();
Matrix4f W= Matrix4f::Identity();
Matrix4f I= Matrix4f::Identity();
Matrix4f P= Matrix4f::Zero();
Matrix4f H= Matrix4f::Identity();
MatrixXf X(4,1);
MatrixXf V(3,1);
MatrixXf VZ(3,1);
MatrixXf Vmatrix(5,3);
Matrix4f A;
Matrix4f K;
VectorXf Z(4);

// Position and movement messages
geometry_msgs::PoseStamped measurementPose;
geometry_msgs::Twist twist;
geometry_msgs::Twist measurementTwist;
// Keep track of Quadcopter state
bool got_pose_, stationary, got_vel_;
double theta,x,y;
double T = 50; // ROS loop rate
double T1=0;
double T2=0;
double xOld=0;
double yOld=0;
double vXTot=0;
double vYTot=0;
double vZTot=0;
double zOld=0;
int counter11 = 0;
double yaw; // FIXME: What is this for?

// Updates position coordinates
void poseCallback(const tf2_msgs::TFMessage::ConstPtr& posePtr)
{
    const tf2_msgs::TFMessage& msg=*posePtr;
    if (msg.transforms[0].header.frame_id.compare("ORB_SLAM/World")==0){
    got_pose_ = true;
    std::cout<<"pass";
    // FIXME: Set found agent's position
    // FIXME: NOT SURE ABOUT PITCH AND ROLL
    measurementPose.header.frame_id=msg.transforms[0].child_frame_id;
    xOld=measurementPose.pose.position.x;
    yOld=measurementPose.pose.position.y;
    zOld=measurementPose.pose.position.z;
    measurementPose.pose.position.x = msg.transforms[0].transform.translation.z;
    measurementPose.pose.position.y = msg.transforms[0].transform.translation.x;
    measurementPose.pose.position.z = -msg.transforms[0].transform.translation.y;
    measurementPose.pose.orientation.x = msg.transforms[0].transform.rotation.z;
    measurementPose.pose.orientation.y = msg.transforms[0].transform.rotation.x;
    measurementPose.pose.orientation.z = -msg.transforms[0].transform.rotation.y;
    measurementPose.pose.orientation.w = msg.transforms[0].transform.rotation.w;
   // measurementPose.pose.orientation = posePtr->transforms.transform.rotation;

    yaw = tf::getYaw(measurementPose.pose.orientation);
    
    T1=T2;
    T2=ros::Time::now().toSec();
    }
}

void imuCallback(const ardrone_autonomy::Navdata::ConstPtr& imuPtr)
{
    got_vel_ = true;
std::cout<<imuPtr;
    measurementTwist.linear.x= imuPtr->vx/1000;    
    measurementTwist.linear.y= imuPtr->vy/1000;
    measurementTwist.linear.z= imuPtr->vz/1000;
}

// FIXME: what does ipt stand for?
void iptCallback(const geometry_msgs::Twist::ConstPtr& ipt)
{
    twist.linear=ipt->linear;
    twist.angular=ipt->angular;
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "Quadcopter Extended Kalman Filter: version 1, ORB SLAM only"); //Ros Initialize
    ros::start();
    ros::Rate loop_rate(T); //Set Ros frequency to 50/s (fast)

    // FIXME: Make variable names clearer or add comments next to them
    ros::NodeHandle nh_, ph_, gnh_, ph("~");
    ros::Subscriber pos_sub_ ;
    ros::Subscriber ipt_sub_ ;
    ros::Subscriber imu_sub_ ;
    ros::Publisher gl_pub_ , vel_pub_;

    void poseCallback(const tf2_msgs::TFMessage::ConstPtr& pose);
    void iptCallback(const geometry_msgs::Twist::ConstPtr&);
    void imuCallback(const ardrone_autonomy::Navdata::ConstPtr&);
    // ROS stuff
    // Other member variables
    got_pose_ = false;
    stationary = false;
    double ux=0;
    double uy=0;
    double vx=0;
    double vy=0;
    double uz=0;
    double maxVelFactor=1.25;
    
    Q(0,0)=0;
    Q(1,1)=0;
    Q(2,2)=0;
    Q(3,3)=0;
    R(0,0)=.01;
    R(1,1)=.01;
    R(2,2)=.01;
    R(3,3)=.01;
    P(0,0)=1000;
    P(1,1)=1000;
    P(2,2)=1000;
    P(3,3)=1000;
    X(0)=0;
    X(1)=0;
    X(2)=0;
    X(3)=0;
    V(0)=0;
    V(1)=0;
    V(2)=0;
    Z(0)=0;
    Z(1)=0;
    Z(2)=0;
    Z(3)=0;
    VZ(0)=0;
    VZ(1)=0;
    VZ(2)=0;
    Vmatrix(0,0)=0;
    Vmatrix(1,0)=0;
    Vmatrix(2,0)=0;
    Vmatrix(3,0)=0;
    Vmatrix(4,0)=0;
    T1=ros::Time::now().toSec();
    T2=ros::Time::now().toSec();
    

    geometry_msgs::PoseStamped poseEstimation;
    geometry_msgs::Twist twistEstimation;
    poseEstimation.pose.position.x=0;
    poseEstimation.pose.position.y=0;
    poseEstimation.pose.position.z=0;
    poseEstimation.pose.orientation=tf::createQuaternionMsgFromYaw(0);

    pos_sub_= nh_.subscribe<tf2_msgs::TFMessage>("/tf", 1,poseCallback);
    imu_sub_= nh_.subscribe<ardrone_autonomy::Navdata>("/ardrone/navdata", 1,imuCallback);
    ipt_sub_=nh_.subscribe<geometry_msgs::Twist>("/cmd_vel",1,iptCallback);
    gl_pub_ = gnh_.advertise<geometry_msgs::PoseStamped>("/poseEstimation", 1000, true);
    vel_pub_ = gnh_.advertise<geometry_msgs::Twist>("/velocityEstimation", 1000, true);
    while (ros::ok()) 
    {
        got_pose_ = false;
        got_vel_  = false;
        ros::spinOnce();

        //Conditionals
        if (got_pose_ == true)
        {
            cout<<"got_pose_: "<<got_pose_<<"\n";
            R(0,0)=1;
            R(1,1)=1;
            R(2,2)=1;
            R(3,3)=1;
        }
        else
        {
            R(0,0)=10000;
            R(1,1)=10000;
            R(2,2)=10000;
            R(3,3)=10000;
        }
        
        if (stationary==true)
        {
            Q(0,0)=0;
            Q(1,1)=0;
            Q(2,2)=0;
            Q(3,3)=0;
        }
        else
        {
            Q(0,0)=100;
            Q(1,1)=100;
            Q(2,2)=100;
            Q(3,3)=100;
        }

        Matrix4f temp;
            
        //Stage 1
        ux=twist.linear.x*cos(yaw)-twist.linear.y*sin(yaw);
        uy=-twist.linear.y*cos(yaw)+twist.linear.x*sin(yaw);
        uz=twist.linear.z;
        
        Vmatrix(4,0)=Vmatrix(3,0);
        Vmatrix(3,0)=Vmatrix(2,0);
        Vmatrix(2,0)=Vmatrix(1,0);
        Vmatrix(1,0)=Vmatrix(0,0);
        Vmatrix(0,0)=(measurementPose.pose.position.x-xOld)/(T2-T1);
        
        Vmatrix(4,1)=Vmatrix(3,1);
        Vmatrix(3,1)=Vmatrix(2,1);
        Vmatrix(2,1)=Vmatrix(1,1);
        Vmatrix(1,1)=Vmatrix(0,1);
        Vmatrix(0,1)=(measurementPose.pose.position.y-yOld)/(T2-T1);
        
        Vmatrix(4,2)=Vmatrix(3,2);
        Vmatrix(3,2)=Vmatrix(2,2);
        Vmatrix(2,2)=Vmatrix(1,2);
        Vmatrix(1,2)=Vmatrix(0,2);
        Vmatrix(0,2)=(measurementPose.pose.position.z-zOld)/(T2-T1);
        
        
        vXTot=(Vmatrix(2,0)+Vmatrix(1,0)+Vmatrix(0,0))/3;
        vYTot=(Vmatrix(2,1)+Vmatrix(1,1)+Vmatrix(0,1))/3;
        vZTot=(Vmatrix(2,2)+Vmatrix(1,2)+Vmatrix(0,2))/3;
        
         std::cout<<"\n Measured Velocity: \n"<<vXTot<<"\n";
        // if (ux>1){
        //     ux=1;
        // }
        // else if (ux<-1){
        //     ux=-1;
        // }
        // if (uy>1){
        //     uy=1;
        // }
        // else if (uy<-1){
        //     uy=-1;
        // }
        // if (ux*maxVelFactor<vx && ux>0){
        //     ux=0;
        // }
        // else if(ux*maxVelFactor>vx && ux<0){
        //     ux=0;
        // }
        
        // if (uy*maxVelFactor<vy && uy>0){
        //     uy=0;
        // }
        // else if(uy*maxVelFactor>vy && uy<0){
        //     uy=0;
        // }    
        
        
        // V << V(0)+ .6*ux/T,V(1)+.6*uy/T,V(2)+.6*uz/T;
        // VZ << measurementTwist.linear.x,measurementTwist.linear.y,measurementTwist.linear.y;
         Z << measurementPose.pose.position.x,measurementPose.pose.position.y,measurementPose.pose.position.z,yaw;
        // X << X(0)+ V(0)/T,X(1)+V(1)/T,X(2)+V(2)/T,X(3)+twist.angular.z/T;
        X << X(0)+ ux/T,X(1)+uy/T,X(2)+uz/T,X(3)+twist.angular.z/T;
        //Stage 2
        if (got_pose_ == true)
        {
            A << 1, 0,0, -twist.linear.x/T*sin(yaw)-twist.linear.y/T*cos(yaw),0, 1,0, twist.linear.x/T*cos(yaw)+twist.linear.y/T*sin(yaw),0, 0, 1,0, 0,0,0,-1;
            P = A*P*A.transpose() + W*Q*W.transpose();

            //Stage 3
            temp = (W*P*W.transpose() + W*R*W.transpose());
            K = P*W.transpose()*temp.inverse();

            //Stage 4
            X = X + K*(Z-X);

            //Stage 5
            P = (I - K*W)*P;
            V(0)=vXTot;
            V(1)=vYTot;
            V(2)=vZTot;
        }

            twistEstimation.linear.x=V(0);
            twistEstimation.linear.y=V(1);
            twistEstimation.linear.z=V(2);

        poseEstimation.pose.position.x = X(0);
        poseEstimation.pose.position.y = X(1);
        poseEstimation.pose.position.z = X(2);
        poseEstimation.pose.orientation = tf::createQuaternionMsgFromYaw(X(3));
  
        
      
        
        gl_pub_.publish(poseEstimation);
        vel_pub_.publish(twistEstimation);

        std::cout<<"\n Measured: \n"<<measurementPose<<"\n";
        //std::cout<<"Twist: \n"<<twist<<"\n";
        std::cout<<"Best Estimation\n"<<poseEstimation<<"\n---------\n\n";
        std::cout<<"Yaw: "<<yaw<<"\n---------\n\n";
        std::cout<<"--------------------------------------------------------------------";
        loop_rate.sleep();
    }
}
//END
