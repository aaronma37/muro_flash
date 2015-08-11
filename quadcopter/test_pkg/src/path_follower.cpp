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
#include <tf/tf.h>

// Quadcopter state data
geometry_msgs::PoseStamped poseEst; 

// Path data
geometry_msgs::PoseArray pathPose;
geometry_msgs::PoseStamped goalPose;
//geometry_msgs::PoseArray closestPosition; //To store closest position to Quadcopter
//geometry_msgs::Vector2 closestPosition; //To calculates closest position to quadCopter.

// Constants
const double PI = 3.141592653589793238463;
const int T = 50;

//Variables
//double closestX = 0;
//double closestY = 0;
double distanceClosest = 0;
double closestPositionsArray [1] = {0}; //To compare results and store the smallest distance. 

// Flags
bool newPath;
bool isOpenLoop; 

// Determine type of path and unpack array of pose
void pathCallback(const geometry_msgs::PoseArray::ConstPtr& pathPtr)
{
  newPath = true;
  
  // Process path data
  if( (pathPtr -> header.frame_id).compare("OPEN") == 0) // check if open loop
  {
    isOpenLoop = true;
    pathPose = *pathPtr;
  }
  else
  {
    isOpenLoop = false;
    pathPose = *pathPtr;
  }
}

//This function will calculate the shortest distance between the quadcopter and the position points.
//id shorterstDistanceCalc()
{
  for(int i = 0; i < pathPose.poses.size(); i++)
  {
    (pathPose.poses)[i]
    closestPosition.x = pathPose.pose.position.x;
    closestPosition.y = pathPose.pose.position.y;
   
    closestPositionsArray = (sqrt ( (closestPosition.x - poseEst.x)^2 + (closestPostion.y - poseEst.y )^2 ) )
 
    if (distanceClosest > closestPositionsArray[i])
      { 
      distanceClosest = closestPositionArray[i]; 
      pathPose[0] = closestPositionsArray [i];
      } 
  
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
    goalPub = n.advertise<geometry_msgs::PoseStamped>("/goal_pose", 1000, true);

   
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
            for(int i = 0; i < pathPose.poses.size(); i++)
            {
              ros::spinOnce();
              if(newPath)
              {
                break;
              }
              goalPose.pose = (pathPose.poses)[i];
              goalPose.pose.orientation = tf::createQuaternionMsgFromYaw(0);
              goalPub.publish(goalPose);
              ROS_INFO("%d\n", i); //FIXME: testing
              loop_rate.sleep();
            }
          }
          else
          {
            newPath = false; // reset flag and set stopping condition for while loop
            while(!newPath) // while no new path has been published
            {
              if(!ros::ok())
              {
                break;
              }
              // Publish array of pose
              for(int i = 0; i < pathPose.poses.size(); i++)
              {
                ros::spinOnce();
                if(newPath)
                {
                  break;
                }
                goalPose.pose = (pathPose.poses)[i];
                goalPose.pose.orientation = tf::createQuaternionMsgFromYaw(0);
                goalPub.publish(goalPose);
                ROS_INFO("%d\n", i); //FIXME: testing
                loop_rate.sleep();
              }
            }
          }
        }
        
        loop_rate.sleep();
    }
}
//END
