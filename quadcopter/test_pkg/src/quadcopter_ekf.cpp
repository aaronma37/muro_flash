#include <iostream>
#include <stdio.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <tf2_msgs/TFMessage.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include <geometry_msgs/PoseStamped.h>
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
Matrix4f A;
Matrix4f K;
VectorXf Z(4);

// Position and movement messages
geometry_msgs::PoseStamped measurementPose;
geometry_msgs::Twist twist;

// Keep track of Quadcopter state
bool got_pose_, stationary;
double theta,x,y;
double T = 20; // ROS loop rate
int counter11 = 0;
double yaw; // FIXME: What is this for?

// Updates position coordinates
void poseCallback(const tf2_msgs::TFMessage posePtr)
{
    if (posePtr.transforms.header.frame_id.compare("ORB_SLAM/World")==0){
    got_pose_ = true;
    std::cout<<"pass";
    // FIXME: Set found agent's position
    // FIXME: NOT SURE ABOUT PITCH AND ROLL
    measurementPose.pose.position = posePtr.transforms.transform.translation;
    measurementPose.pose.orientation = posePtr.transforms.transform.rotation;

    yaw = tf::getYaw(posePtr->transforms.transform.rotation)+3.14;
    }
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
    ros::Publisher gl_pub_ ;

    void poseCallback(const tf2_msgs::TFMessage::ConstPtr& pose);
    void iptCallback(const geometry_msgs::Twist::ConstPtr&);
    // ROS stuff
    // Other member variables
    got_pose_ = false;
    stationary = false;

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
    Z(0)=0;
    Z(1)=0;
    Z(2)=0;
    Z(3)=0;

    geometry_msgs::PoseStamped poseEstimation;
    poseEstimation.pose.position.x=0;
    poseEstimation.pose.position.y=0;
    poseEstimation.pose.position.z=0;
    poseEstimation.pose.orientation=tf::createQuaternionMsgFromYaw(3.14);

    pos_sub_= nh_.subscribe<tf2_msgs::TFMessage>("/tf", 1,poseCallback);
    ipt_sub_=nh_.subscribe<geometry_msgs::Twist>("/cmd_vel",1,iptCallback);
    gl_pub_ = gnh_.advertise<geometry_msgs::PoseStamped>("/poseEstimation", 1000, true);

    while (ros::ok()) 
    {
        got_pose_ = false;
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
            Q(0,0)=5;
            Q(1,1)=5;
            Q(2,2)=5;
            Q(3,3)=5;
        }

        Matrix4f temp;

        //Stage 1
        Z << measurementPose.pose.position.x,measurementPose.pose.position.y,measurementPose.pose.position.z,yaw;
        X << X(0)+twist.linear.x/T*cos(yaw)+twist.linear.y/T*sin(yaw),X(1)+twist.linear.y/T*cos(yaw)+twist.linear.x/T*sin(yaw),X(2)+twist.linear.z/T,X(3)+twist.angular.z/T;

        //Stage 2
        if (got_pose_ == true)
        {
            A << 1, 0,0, -twist.linear.x/T*sin(yaw)+twist.linear.y/T*cos(yaw),0, 1,0, twist.linear.x/T*cos(yaw)-twist.linear.y/T*sin(yaw),0, 0, 1,0, 0,0,0,1;
            P = A*P*A.transpose() + W*Q*W.transpose();

            //Stage 3
            temp = (W*P*W.transpose() + W*R*W.transpose());
            K = P*W.transpose()*temp.inverse();

            //Stage 4
            X = X + K*(Z-X);

            //Stage 5
            P = (I - K*W)*P;
        }

        poseEstimation.pose.position.x = X(0);
        poseEstimation.pose.position.y = X(1);
        poseEstimation.pose.position.z = X(2);
        poseEstimation.pose.orientation = tf::createQuaternionMsgFromYaw(X(3)+3.14);
        gl_pub_.publish(poseEstimation);

        std::cout<<"\n Measured: \n"<<Z<<"\n\n";
        std::cout<<"Twist: \n"<<twist<<"\n\n";
        std::cout<<"Best Estimation\n"<<poseEstimation<<"\n---------\n\n\n\n";
        std::cout<<"--------------------------------------------------------------------";
        loop_rate.sleep();
    }
}
//END
