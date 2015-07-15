/*

This node subcribes to "/poseEstimation" topic where it will 
listen for any position (pose) data coming in from the ekf 
filter node.
These pose coordinates are the best estimates for where the
Quadcopter is currently located.

This node subscribes to "/goalPose" topic where 
it will listen for any pose data coming from the android node. 
These pose coordinates are the desired position for the 
Quadcopter.
FIXME: This subscriber is yet to be implemented.

This node publishes to "/cmd_vel" topic where it will update 
its velocity, to be used by the ekf node for pose estimation.


*/

#include <iostream>
#include <stdio.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <tf2_msgs/TFMessage.h>
#include "geometry_msgs/TransformStamped.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include <geometry_msgs/PoseStamped.h>
#include <tf/tf.h>
#include <fstream>
#include <math.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <stdlib.h>
#include "std_msgs/String.h"
#include <std_msgs/Float64.h>
#include <sstream>

using namespace std;

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
void poseCallback(const tf2_msgs::TFMessage::ConstPtr& posePtr)
{
    const tf2_msgs::TFMessage& msg=*posePtr;
    if (msg.transforms[0].header.frame_id.compare("ORB_SLAM/World")==0){
    got_pose_ = true;
    std::cout<<"pass";
    // FIXME: Set found agent's position
    // FIXME: NOT SURE ABOUT PITCH AND ROLL
    measurementPose.pose.position.x = msg.transforms[0].transform.translation.x;
    measurementPose.pose.position.y = msg.transforms[0].transform.translation.y;
    measurementPose.pose.position.z = msg.transforms[0].transform.translation.z;
    measurementPose.pose.orientation.x = msg.transforms[0].transform.rotation.x;
    measurementPose.pose.orientation.y = msg.transforms[0].transform.rotation.y;
    measurementPose.pose.orientation.z = msg.transforms[0].transform.rotation.z;
    measurementPose.pose.orientation.w = msg.transforms[0].transform.rotation.z;
   // measurementPose.pose.orientation = posePtr->transforms.transform.rotation;

    yaw = tf::getYaw(measurementPose.pose.orientation)+3.14;
    }
}


void iptCallback(const geometry_msgs::Twist::ConstPtr& ipt)
{
    twist.linear=ipt->linear;
    twist.angular=ipt->angular;
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "Quadcopter Hover: version 1, EKF pose estimations only"); //Ros Initialize
    ros::start();
    ros::Rate loop_rate(T); //Set Ros frequency to 50/s (fast)

    // FIXME: Make variable names clearer or add comments next to them
    ros::NodeHandle nh_, gnh_;
    ros::Subscriber pos_sub_ ;
    ros::Subscriber ipt_sub_ ;
    ros::Publisher gl_pub_ ;

    void poseCallback(const tf2_msgs::TFMessage::ConstPtr& pose);
    void iptCallback(const geometry_msgs::Twist::ConstPtr&);

    got_pose_ = false;
    stationary = false;

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
