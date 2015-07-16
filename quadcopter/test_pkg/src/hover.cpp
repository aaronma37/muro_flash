/*

The main function of this node is to process position (pose) 
data, compute the error between desired and estimated pose, 
and then use a tuned PID controller to determine the correct 
velocity needed to minimize that error and approximate the 
goal pose.

This node subcribes to "/poseEstimation" topic where it will 
listen for any pose data coming in from the ekf filter node.
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
geometry_msgs::PoseStamped poseEstimation; // Where the Quadcopter thinks it is
geometry_msgs::PoseStamped poseGoal; // Where the Quadcopter should be
geometry_msgs::PoseStamped poseError; // Difference between desired pose and current pose
geometry_msgs::Twist velocity; // Velocity command needed to rectify the error

// Keep track of Quadcopter state
bool updatedPoseEst, updatedPoseGoal;
double theta, x, y;
double T = 50; // ROS loop rate

// Constants
const double PI = 3.141592653589793238463;

// Kepp track of yaw to determine angular component of velocity 
double poseEstYaw; // twist or oscillation about a vertical axis
double poseGoalYaw = PI;
double poseErrYaw = 0;
double poseErrYawPrev; // for PID derivative term
double pastYawErr = 0;

// PID controller terms
geometry_msgs::PoseStamped pastError; // This is the integral term
geometry_msgs::PoseStamped poseErrorPrev; // This is used to determine the derviative term
                                          // It stores the previous poseError

// Updates current position estimate sent by the ekf
void poseEstCallback(const geometry_msgs::PoseStamped::ConstPtr& posePtr)
{
    updatedPoseEst = true;
    poseEstimation.pose = posePtr -> pose;
    poseEstYaw = tf::getYaw(poseEstimation.pose.orientation) + PI;
}

// Updates goal position sent by android
void poseGoalCallback(const geometry_msgs::PoseStamped::ConstPtr& posePtr)
{
    updatedPoseGoal = true;
    poseGoal.pose = posePtr -> pose;
    poseGoalYaw = tf::getYaw(poseGoal.pose.orientation) + PI;
}

// Calculates updated error to be used by PID
void calculateError(void)
{
    poseErrorPrev.pose = poseError.pose; // Store old pose error for PID equation
    poseErrYawPrev = poseErrYaw; // Store old pose yaw for PID equation
    poseError.pose.position.x = poseGoal.pose.position.x - poseEstimation.pose.position.x;
    poseError.pose.position.y = poseGoal.pose.position.y - poseEstimation.pose.position.y;
    poseError.pose.position.z = poseGoal.pose.position.z - poseEstimation.pose.position.z;
    poseErrYaw = poseGoalYaw - poseEstYaw;
    poseError.pose.orientation = tf::createQuaternionMsgFromYaw(poseErrYaw);
}

// This is the PID method
void PID(void)
{
    // FIXME: Tune PID constants
    double kp = .2; // Proportionality constant
    double ki = .1; // Integration constant
    double kd = 0; // Differential constant
    pastError.pose.position.x += (1/T)*poseError.pose.position.x;
    pastError.pose.position.y += (1/T)*poseError.pose.position.y;
    pastError.pose.position.z += (1/T)*poseError.pose.position.z;
    pastYawErr += (1/T)*poseErrYaw;
    
    velocity.linear.x = (kp*poseError.pose.position.x) + (ki*pastError.pose.position.x) + (kd*(poseError.pose.position.x - poseErroPrev.pose.position.x));
    velocity.linear.y = (kp*poseError.pose.position.y) + (ki*pastError.pose.position.y) + (kd*(poseError.pose.position.y - poseErroPrev.pose.position.y));
    velocity.linear.z = (kp*poseError.pose.position.z) + (ki*pastError.pose.position.z) + (kd*(poseError.pose.position.z - poseErroPrev.pose.position.z));
    
    velocity.angular.z = (kp*poseErrYaw) + (ki*pastYawErr) + (kd*(poseErrYaw - poseErrYawPrev));
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "Quadcopter Hover: version 1, EKF pose estimations only"); //Ros Initialize
    ros::start();
    ros::Rate loop_rate(T); //Set Ros frequency to 50/s (fast)

    ros::NodeHandle n;
    ros::Subscriber poseEstSub ;
    ros::Subscriber poseGoalSub ;
    ros::Publisher velPub ;

    poseEstSub = n.subscribe<geometry_msgs::PoseStamped>("/poseEstimation", 1, poseEstCallback);
    poseGoalSub = n.subscribe<geometry_msgs::PoseStamped>("/goal_pose", 1, poseGoalCallback);
    velPub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 1000, true);

    // Initialize msgs
    poseGoal.pose.position.x = 0;
    poseGoal.pose.position.y = 0;
    poseGoal.pose.position.z = 0;
    poseError.pose.position.x = 0;
    poseError.pose.position.y = 0;
    poseError.pose.position.z = 0;
    velocity.angular.x = 1;
    velocity.angular.y = 0;
    pastError.pose.position.x = 0;
    pastError.pose.position.y = 0;
    pastError.pose.position.z = 0;

    while (ros::ok()) 
    {
        updatedPoseEst = false;
        updatedPoseGoal = false;
        
        ros::spinOnce();
        
        calculateError();
        
        PID();

        velPub.publish(velocity);

        std::cout<<"Twist: \n"<<velocity<<"\n\n";
        std::cout<<"--------------------------------------------------------------------";
        loop_rate.sleep();
    }
}
//END
