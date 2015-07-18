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

//TODO: IMPLEMENT MOVING AVERAGE FOR DERIVATIVE TERM
// http://www.analog.com/media/en/technical-documentation/dsp-book/dsp_book_Ch15.pdf
// THIS WILL GET RID OF SPIKES AND NOISE (COMMONLY DONE)
//
// TOOLS AGAINST INTEGRAL WINDUP
// GAIN SCHEDULING 

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
geometry_msgs::Vector3 pidGain; // Store pid gain values

// Keep track of Quadcopter state
bool updatedPoseEst, updatedPoseGoal;
double T = 50; // ROS loop rate

// Constants
const double PI = 3.141592653589793238463;
const double DEFAULT_KP = 0.7;
const double DEFAULT_KI = 0;
const double DEFAULT_KD = 1;
double kp = DEFAULT_KP; // Proportional gain
double ki = DEFAULT_KI; // Integral gain
double kd = DEFAULT_KD; // Differential gain

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

// Updates pid gain values
void pidGainCallback(const geometry_msgs::Vector3::ConstPtr& gainPtr)
{
    if(gainPtr -> x == 0.0 && gainPtr -> y == 0.0 && gainPtr -> z == 0.0)
    {
      kp = DEFAULT_KP;
      ki = DEFAULT_KI;
      kd = DEFAULT_KD;
    }
    else
    {
      kp = (double) gainPtr -> x;
      ki = (double) gainPtr -> y;
      kd = (double) gainPtr -> z;
    }
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
    double kpZ = .8;
    double kpYaw = .5;
    
    double kiZ = 1.5/50;
    double kiYaw = 0;
    
    double kdZ = 0;
    double kdYaw = 0;
    
    pastError.pose.position.x += (1/T)*poseError.pose.position.x;
    pastError.pose.position.y += (1/T)*poseError.pose.position.y;
    pastError.pose.position.z += (1/T)*poseError.pose.position.z;
    pastYawErr += (1/T)*poseErrYaw;
    
    velocity.linear.x = (kp*poseError.pose.position.x) + (ki*pastError.pose.position.x) + (kd*T*(poseError.pose.position.x - poseErrorPrev.pose.position.x));
    velocity.linear.y = -( (kp*poseError.pose.position.y) + (ki*pastError.pose.position.y) + (kd*T*(poseError.pose.position.y - poseErrorPrev.pose.position.y)) );
    velocity.linear.z = (kpZ*poseError.pose.position.z) + (kiZ*pastError.pose.position.z) + (kdZ*T*(poseError.pose.position.z - poseErrorPrev.pose.position.z));
    
    velocity.angular.z = (kpYaw*poseErrYaw) + (kiYaw*pastYawErr) + (kdYaw*T*(poseErrYaw - poseErrYawPrev));
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "Quadcopter Hover: version 1, EKF pose estimations only"); //Ros Initialize
    ros::start();
    ros::Rate loop_rate(T); //Set Ros frequency to 50/s (fast)

    ros::NodeHandle n;
    ros::Subscriber poseEstSub;
    ros::Subscriber poseGoalSub;
    ros::Subscriber pidGainSub;
    ros::Publisher velPub;

    poseEstSub = n.subscribe<geometry_msgs::PoseStamped>("/poseEstimation", 1, poseEstCallback);
    poseGoalSub = n.subscribe<geometry_msgs::PoseStamped>("/goal_pose", 1, poseGoalCallback);
    pidGainSub = n.subscribe<geometry_msgs::Vector3>("/pid_gain", 1, pidGainCallback);
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
