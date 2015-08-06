/*

This node processes an array of pose and publishes necessary 
goal pose in order to execute a path following algorithm.

*/

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

// Quadcopter state data
geometry_msgs::PoseStamped poseEst; 

// Path data
geometry_msgs::Pose[] pathPose;

// Constants
const double PI = 3.141592653589793238463;


// Flags
bool newPath;
bool isOpenLoop; 


// Determine type of path and unpack array of pose
void pathCallback(const geometry_msgs::PoseArray::ConstPtr& pathPtr)
{
  newPath = true;
  
  // Process path data
  if( (pathPtr -> header.frame_id).compare("open") == 0) // check if open loop
  {
    isOpenLoop = true;
    pathPose = pathPtr -> poses;
  }
  else
  {
    isOpenLoop = false;
    pathPose = pathPtr -> poses;
  }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "path_follower"); //Ros Initialize
    ros::start();
    ros::Rate loop_rate(T); //Set Ros frequency to 50/s (fast)

    ros::NodeHandle n;
    ros::Subscriber pathSub;
    ros::Publisher goalPub;

    pathSub = n.subscribe<geometry_msgs::PoseArray>("/path", 1, pathCallback);
    goalPub = n.advertise<geometry_msgs::Pose>("/goal_pose", 1000, true);

    // Initialize msgs and flags
    newPath = false;

    while (ros::ok()) 
    {
        ros::spinOnce();
        
        if(newPath) // check if a new path has been set
        {
          if(isOpenLoop) // determine path ID
          {
            newPath = false; // reset flag
            
            // Publish array of pose
            for(int i = 0; i < pathPose.size; i++)
            {
              ros::spinOnce();
              if(newPath)
              {
                break;
              }
              goalPub.pub(pathPose[i]);
              loop_rate.sleep();
            }
          }
          else
          {
            newPath = false; // reset flag and set stopping condition for while loop
            while(!newPath) // while no new path has been published
            {
              // Publish array of pose
              for(int i = 0; i < pathPose.size; i++)
              {
                ros::spinOnce();
                if(newPath)
                {
                  break;
                }
                goalPub.pub(pathPose[i]);
                loop_rate.sleep();
              }
            }
          }
        }
        
        loop_rate.sleep();
    }
}
//END
