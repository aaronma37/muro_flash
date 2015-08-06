/*

This node processes an array of pose and publishes necessary 
goal pose in order to execute a path following algorithm.

WARNING: This code utilizes a lot of spinOnce(), minimize use
of callbacks to avoid lengthy computations.

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
#include <std_msgs/Bool.h>
#include <sstream>

// Path data
int pathLength = 0;
geometry_msgs::Pose[] pathPose;

// Constants
const double PI = 3.141592653589793238463;


// Flags
bool newPath;
bool isOpenLoop; 


// Unpack array of pose and determine type of path
void pathCallback(const geometry_msgs::PoseStamped::ConstPtr& pathPtr)
{
  newPath = true;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "path_follower"); //Ros Initialize
    ros::start();
    ros::Rate loop_rate(T); //Set Ros frequency to 50/s (fast)

    ros::NodeHandle n;
    ros::Subscriber pathSub;
    ros::Publisher goalPub;

    pathSub = n.subscribe<geometry_msgs::PoseStamped>("/path", 1, pathCallback); // FIXME: change msg type
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
            for(int i = 0; i < pathLength; i++) // FIXME: pseudocode
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
              for(int i = 0; i < pathlength; i++) // FIXME: pseudocode
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
