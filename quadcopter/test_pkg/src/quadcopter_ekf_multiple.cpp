#include <iostream>
#include <stdio.h>
#include <string>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/TwistStamped.h>
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
This file is for building an Extended Kalman filter for multiple drones.

*/
using namespace std;
using namespace Eigen;
using Eigen::MatrixXd;

// Declare matrixes used in the Kalman Filter
int k=0;
const int num=1000;
Matrix4f Q= Matrix4f::Zero();
Matrix4f R= Matrix4f::Zero();
Matrix4f W= Matrix4f::Identity();
Matrix4f I= Matrix4f::Identity();
Matrix4f P= Matrix4f::Zero();
Matrix4f H= Matrix4f::Identity();
MatrixXf X[num];
MatrixXf V(3,1);
MatrixXf VZ(3,1);
MatrixXf Vmatrix[num];
Matrix4f A;
Matrix4f K;
VectorXf Z(4);

// Position and movement messages
geometry_msgs::PoseStamped measurementPose[num];
geometry_msgs::Twist twist[num];
geometry_msgs::Twist measurementTwist[num];
geometry_msgs::PoseStamped poseEstimation[num];
geometry_msgs::TwistStamped twistEstimation[num];
// Keep track of Quadcopter state
bool got_pose_[num], stationary, got_vel_[num];
double theta,x,y;
double T = 500; // ROS loop rate
double T1[num];
double T2[num];
double xOld[num];
double yOld[num];
bool   active[num];
double vXTot=0;
double vYTot=0;
double vZTot=0;
double zOld[num];
int counter11 = 0;
int dummyNumber=0;
double yaw[num]; // FIXME: What is this for?

// Updates position coordinates
void poseCallback(const tf2_msgs::TFMessage::ConstPtr& posePtr)
{
    const tf2_msgs::TFMessage& msg=*posePtr;
    if (msg.transforms[0].header.frame_id.compare("ORB_SLAM/World")==0){
    
    std::cout<<"pass";
    if(msg.transforms[0].child_frame_id.compare("Gypsy Danger")==0){
        k=0;
    }
    else if (msg.transforms[0].child_frame_id.compare("Typhoon")==0){
        k=1;
    }
    else if (msg.transforms[0].child_frame_id.compare("Dummy")==0){
        dummyNumber++;
        std::ostringstream ss;
        ss << dummyNumber;
        string s = "dummy ";
        s+=ss.str();
        k=100+dummyNumber;
        poseEstimation[k].header.frame_id=s;
	twistEstimation[k].header.frame_id=s;
    }
    else{
        return;
    }
    got_pose_[k] = true;
    active[k]=true;
    measurementPose[k].header.frame_id=msg.transforms[0].child_frame_id;
    xOld[k]=measurementPose[k].pose.position.x;
    yOld[k]=measurementPose[k].pose.position.y;
    zOld[k]=measurementPose[k].pose.position.z;
    measurementPose[k].pose.position.x = msg.transforms[0].transform.translation.z;
    measurementPose[k].pose.position.y = msg.transforms[0].transform.translation.x;
    measurementPose[k].pose.position.z = -msg.transforms[0].transform.translation.y;
    measurementPose[k].pose.orientation.x = msg.transforms[0].transform.rotation.z;
    measurementPose[k].pose.orientation.y = msg.transforms[0].transform.rotation.x;
    measurementPose[k].pose.orientation.z = -msg.transforms[0].transform.rotation.y;
    measurementPose[k].pose.orientation.w = msg.transforms[0].transform.rotation.w;
   // measurementPose.pose.orientation = posePtr->transforms.transform.rotation;

    yaw[k] = tf::getYaw(measurementPose[k].pose.orientation);
    
    T1[k]=T2[k];
    T2[k]=ros::Time::now().toSec();
    }
}

void imuCallback(const ardrone_autonomy::Navdata::ConstPtr& imuPtr)
{
    got_vel_[k] = true;
std::cout<<imuPtr;
    measurementTwist[k].linear.x= imuPtr->vx/1000;    
    measurementTwist[k].linear.y= imuPtr->vy/1000;
    measurementTwist[k].linear.z= imuPtr->vz/1000;
}

void iptCallback(const tf2_msgs::TFMessage::ConstPtr& ipt)
{
	for (int i =0;i<1000;i++){
//		if(ipt->transforms[i].header.frame_id.compare("Gypsy Danger")==0){
//			k=0;
//		    }
//		    else if (ipt->transforms[i].header.frame_id.compare("Typhoon")==0){
//			k=1;
//		    }
//		else if (ipt->transforms[i].header.frame_id.compare("dummy 1")==0){
//			k=101;
//		    }
//		else if (ipt->transforms[i].header.frame_id.compare("dummy 2")==0){
//			k=102;
//		    }
//		else if (ipt->transforms[i].header.frame_id.compare("dummy 3")==0){
//			k=103;
//		    }
//		else if (ipt->transforms[i].header.frame_id.compare("dummy 4")==0){
//			k=104;
//		    }
//		else if (ipt->transforms[i].header.frame_id.compare("dummy 5")==0){
//			k=105;
//		    }
//		else if (ipt->transforms[i].header.frame_id.compare("dummy 6")==0){
//			k=106;
//		    }
		    	twist[i].linear.x=ipt->transforms[i].transform.translation.x;
			twist[i].linear.y=ipt->transforms[i].transform.translation.y;
			twist[i].linear.z=ipt->transforms[i].transform.translation.z;
		    	twist[i].angular.z=ipt->transforms[i].transform.rotation.z;
	}

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
    void iptCallback(const tf2_msgs::TFMessage::ConstPtr&);
    void imuCallback(const ardrone_autonomy::Navdata::ConstPtr&);
    // ROS stuff
    // Other member variables
    
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
    
    
    poseEstimation[0].header.frame_id="Gypsy Danger";
    poseEstimation[1].header.frame_id="Typhoon";
    twistEstimation[0].header.frame_id="Gypsy Danger";
    twistEstimation[1].header.frame_id="Typhoon";
    
    for (int i=0;i<num;i++){
            poseEstimation[i].pose.position.x=0;
            poseEstimation[i].pose.position.y=0;
            poseEstimation[i].pose.position.z=0;
            poseEstimation[i].pose.orientation=tf::createQuaternionMsgFromYaw(0);
            T1[i]=ros::Time::now().toSec();
            T2[i]=ros::Time::now().toSec();
            got_pose_[i] = false;
            Vmatrix[i].resize(5,3);
            X[i].resize(4,1);
            active[i]=false;
    }
    

    pos_sub_= nh_.subscribe<tf2_msgs::TFMessage>("/tf", 1,poseCallback);
    imu_sub_= nh_.subscribe<ardrone_autonomy::Navdata>("/ardrone/navdata", 1,imuCallback);
    ipt_sub_=nh_.subscribe<tf2_msgs::TFMessage>("/cmd_vel",1,iptCallback);
    gl_pub_ = gnh_.advertise<geometry_msgs::PoseStamped>("/poseEstimation", 1000, true);
    vel_pub_ = gnh_.advertise<geometry_msgs::TwistStamped>("/velocityEstimation", 1000, true);
    while (ros::ok()) 
    {
        for (int i=0;i<num;i++){
        got_pose_[i] = false;
        got_vel_[i]  = false;
        }
        
        ros::spinOnce();

        for (int i=0;i<num;i++){
            
            if (active[i]==true){
                MatrixXf& b = Vmatrix[i];
                MatrixXf& Xx = X[i];
            
			    //Conditionals
			if (got_pose_[i] == true)
			{
			    cout<<"got_pose_: "<<got_pose_[i]<<"\n";
			    R(0,0)=1;
			    R(1,1)=1;
			    R(2,2)=1;
			    R(3,3)=1;
			    
			    
			b(4,0)=b(3,0);
			b(3,0)=b(2,0);
			b(2,0)=b(1,0);
			b(1,0)=b(0,0);
			b(0,0)=(measurementPose[i].pose.position.x-xOld[i])/(T2[i]-T1[i]);
		
			b(4,1)=b(3,1);
			b(3,1)=b(2,1);
			b(2,1)=b(1,1);
			b(1,1)=b(0,1);
			b(0,1)=(measurementPose[i].pose.position.y-yOld[i])/(T2[i]-T1[i]);
		
			b(4,2)=b(3,2);
			b(3,2)=b(2,2);
			b(2,2)=b(1,2);
			b(1,2)=b(0,2);
			b(0,2)=(measurementPose[i].pose.position.z-zOld[i])/(T2[i]-T1[i]);
		
		
		
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
			ux=twist[i].linear.x*cos(yaw[i])-twist[i].linear.y*sin(yaw[i]);
			uy=-twist[i].linear.y*cos(yaw[i])+twist[i].linear.x*sin(yaw[i]);
			uz=twist[i].linear.z;
		
			vXTot=(b(2,0)+b(1,0)+b(0,0))/3;
			vYTot=(b(2,1)+b(1,1)+b(0,1))/3;
			vZTot=(b(2,2)+b(1,2)+b(0,2))/3;
			//std::cout<<"\n Measured Velocity: \n"<<vXTot<<"\n";
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
			 Z << measurementPose[i].pose.position.x,measurementPose[i].pose.position.y,measurementPose[i].pose.position.z,yaw[i];
			// X << X(0)+ V(0)/T,X(1)+V(1)/T,X(2)+V(2)/T,X(3)+twist.angular.z/T;
			Xx << Xx(0)+ ux/T,Xx(1)+uy/T,Xx(2)+uz/T,Xx(3)+twist[i].angular.z/T;
			//Stage 2
			if (got_pose_[i] == true)
			{
			    A << 1, 0,0, -twist[i].linear.x/T*sin(yaw[i])-twist[i].linear.y/T*cos(yaw[i]),0, 1,0, twist[i].linear.x/T*cos(yaw[i])+twist[i].linear.y/T*sin(yaw[i]),0, 0, 1,0, 0,0,0,-1;
			    P = A*P*A.transpose() + W*Q*W.transpose();

			    //Stage 3
			    temp = (W*P*W.transpose() + W*R*W.transpose());
			    K = P*W.transpose()*temp.inverse();

			    //Stage 4
			    Xx = Xx + K*(Z-Xx);

			    //Stage 5
			    P = (I - K*W)*P;
			    V(0)=vXTot;
			    V(1)=vYTot;
			    V(2)=vZTot;
			}

			    twistEstimation[i].twist.linear.x=V(0);
			    twistEstimation[i].twist.linear.y=V(1);
			    twistEstimation[i].twist.linear.z=V(2);

			poseEstimation[i].pose.position.x = Xx(0);
			poseEstimation[i].pose.position.y = Xx(1);
			poseEstimation[i].pose.position.z = Xx(2);
			poseEstimation[i].pose.orientation = tf::createQuaternionMsgFromYaw(Xx(3));

			gl_pub_.publish(poseEstimation[i]);
			vel_pub_.publish(twistEstimation[i]);

			//std::cout<<"\n Measured: \n"<<measurementPose[i]<<"\n";
			//std::cout<<"Twist: \n"<<twist<<"\n";
			//std::cout<<"Best Estimation\n"<<poseEstimation[i]<<"\n---------\n\n";
			//std::cout<<"Yaw: "<<yaw[i]<<"\n---------\n\n";
			//std::cout<<"--------------------------------------------------------------------";
		       
			    }
			    
			}
		     loop_rate.sleep();
        }
        
}
//END
