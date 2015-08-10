#include <iostream>
#include <stdio.h>
#include <ros/ros.h>
#include <geometry_msgs/PoseArray.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Pose.h>
#include <math.h>
#include <vector>
#include <stdlib.h>
#include "std_msgs/String.h"
#include <std_msgs/Float64.h>
#include <std_msgs/Bool.h>
#include <sstream>

// Test msg
geometry_msgs::PoseArray testPose;
geometry_msgs::Pose poseArray[10];

// Constants
const double T = 1;

int main(int argc, char **argv)
{
    ros::init(argc, argv, "path_follower"); //Ros Initialize
    ros::start();
    ros::Rate loop_rate(T); //Set Ros frequency to 50/s (fast)

    ros::NodeHandle n;
    ros::Publisher testPub;
    
    //testPose.poses = poseArray;

    testPub = n.advertise<geometry_msgs::PoseArray>("/path", 1000, true);

    // Initialize test msg
    testPose.header.frame_id = "OPEN";
    for(int i = 0; i < 10; i++)
    {
        testPose.poses.push_back(poseArray[i]);
    }
    for(int i = 0; i < 10; i++)
    {
      (testPose.poses[i]).position.x = i;
      (testPose.poses[i]).position.y = i;
      (testPose.poses[i]).position.z = i;
    }
    
    testPub.publish(testPose);
    loop_rate.sleep();
    
    ros::shutdown();
    return 0;
}
//END
