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

// Position data
geometry_msgs::PoseStamped poseEst; 

// Path data
geometry_msgs::PoseArray pathPose;

// Controller data
geometry_msgs::PoseStamped goalPose;
geometry_msgs::Twist constVelTerm;

// Constants
const double PI = 3.141592653589793238463;
const double BOUNDARY_RADIUS = 0.1;
const int T = 50;
const int NUM_ITERATIONS = 10;
const double PATH_VEL = 0.1;

// Interpolation data
double closestDistancePoint = 0; // distance from pose estimation to closest point on the path
int closestPointIndex = 0; // index to closest point on the path
double closestDistanceLine = 0; // distance from pose estimation to closest point on interpolation line
geometry_msgs::PoseStamped closestPointOnLine; // pose of closest point on interpolation line
geometry_msgs::PoseStamped nextPointClosest; // pose of next point in path
double midpoints[2] = {0,0};

// Flags
bool newPath;
bool isOpenLoop; 

// Updates current position estimate sent by the ekf
void poseEstCallback(const geometry_msgs::PoseStamped::ConstPtr& posePtr)
{
    poseEst.pose = posePtr -> pose;
}

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

double distanceFormula (double x3, double x2, double y3, double y2)
{
  double c = 0;
  c = sqrt ( pow(x3 - x2, 2) + pow(y3 - y2, 2) );
  return c;
}

void calculateMidPoints(double x3, double x2, double y3, double y2)
{
    midpoints[0] = (x3+x2)/2 ;
    midpoints[1] = (y3+y2)/2 ;
}

// This function will identify the closest point on the path to the quadcopter
void closestPointOnPath (void)
{
  double tempClosestDistance = 0;
  
  for(int i = 0; i < pathPose.poses.size(); i++)
  {
    tempClosestDistance = distanceFormula ( pathPose.poses[i].position.x, poseEst.pose.position.x, 
                                        pathPose.poses[i].position.y, poseEst.pose.position.y );

    if ( (closestDistancePoint == 0) || (closestDistancePoint > tempClosestDistance) )
      { 
        closestDistancePoint = tempClosestDistance; 
        closestPointIndex = i;
      }
  }
}

// Outputs a constant velocity term for the sliding mode controller
void calcConstVelTerm(void)
{
    double globalAngle = 0;
    double vector1[2] = {1, 0};
    double vector2[2] = {nextPointClosest.pose.position.x - closestPointOnLine.pose.position.x, 
                        nextPointClosest.pose.position.y - closestPointOnLine.pose.position.y};
    double dotProduct = (vector1[0]*vector2[0]) + (vector1[1]*vector2[1]);
    double absProduct = distanceFormula(nextPointClosest.pose.position.x, closestPointOnLine.pose.position.x,
                                        nextPointClosest.pose.position.y, closestPointOnLine.pose.position.y);
    globalAngle = acos(dotProduct/absProduct);
    
    constVelTerm.linear.x = cos(globalAngle)*PATH_VEL;
    constVelTerm.linear.y = sin(globalAngle)*PATH_VEL;
}

// Interpolates to find closest point on the path using the bisection method
void findClosestPointOnLine(void)
{
    double point1[2] = {0,0};
    double point2[2] = {0,0};
    closestPointOnPath();
    // FIXME: Check to see if pose estimation is between line boundary
    // FIXME: Check to see if point returned is last point
     if (closestPointIndex == pathPose.poses.size() - 1)
     {
         return;
     }
    
    point1[0] = pathPose.poses[closestPointIndex].position.x;
    point1[1] = pathPose.poses[closestPointIndex].position.y;
    point2[0] = pathPose.poses[closestPointIndex + 1].position.x;
    point2[1] = pathPose.poses[closestPointIndex + 1].position.y;
    nextPointClosest.pose.position.x = point2[0];
    nextPointClosest.pose.position.y = point2[1];
    
    double distance1 = 0;
    double distance2 = 0;
    for(int i=0; i<NUM_ITERATIONS; i++)
    {
        distance1 = distanceFormula(poseEst.pose.position.x, point1[0], poseEst.pose.position.y, point1[1]);
        distance2 = distanceFormula(poseEst.pose.position.x, point2[0], poseEst.pose.position.y, point2[1]);
        calculateMidPoints (point2[0], point1[0], point2[1], point1[1]);
        if(distance1 < distance2)
        {
            point2[0] = midpoints[0];
            point2[1] = midpoints[1];
        }
        else
        {
            point1[0] = midpoints[0];
            point1[1] = midpoints[1];
        }
    }
    
    closestPointOnLine.pose.position.x = midpoints[0];
    closestPointOnLine.pose.position.y = midpoints[1];
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "path_follower"); //Ros Initialize
    ros::start();
    ros::Rate loop_rate(T); //Set Ros frequency to 50/s (fast)

    ros::NodeHandle n;
    ros::Subscriber pathSub;
    ros::Subscriber poseEstSub;
    ros::Publisher goalPub;
    ros::Publisher velPub;

    pathSub = n.subscribe<geometry_msgs::PoseArray>("/path", 1, pathCallback);
    poseEstSub = n.subscribe<geometry_msgs::PoseStamped>("/poseEstimation", 1, poseEstCallback);
    goalPub = n.advertise<geometry_msgs::PoseStamped>("/goal_pose", 1000, true);
    velPub = n.advertise<geometry_msgs::Twist>("/path_vel", 1000, true);
   
    // Initialize msgs and flags
    newPath = false;
    bool onPath;
    closestPointOnLine.pose.position.x = 0;
    closestPointOnLine.pose.position.y = 0;
    constVelTerm.linear.x = 0;
    constVelTerm.linear.y = 0;
    constVelTerm.linear.z = 0;
    constVelTerm.angular.x = 0;
    constVelTerm.angular.y = 0;
    constVelTerm.angular.z = 0;

    while (ros::ok()) 
    {
        ros::spinOnce();
        
        if(newPath) // check if a new path has been set
        {
          if(isOpenLoop) // path given is OPEN
          {
            newPath = false; // reset flag
            onPath = true; // reset interpolation flag
            
            // Publish array of pose
            for(int i = 0; i < pathPose.poses.size(); i++)
            {
              if(newPath || !ros::ok())
              {
                break;
              }
              if ( (pathPose.poses[i].position.x !=0) && (pathPose.poses[i].position.y != 0) )
              {
                  goalPose.pose = (pathPose.poses)[i];
              }
              else onPath = false;
              
              goalPose.pose.orientation = tf::createQuaternionMsgFromYaw(0);
              if(i == 0 || onPath == false)
              {
                goalPub.publish(goalPose);
                while( distanceFormula(pathPose.poses[i].position.x, poseEst.pose.position.x, 
                                      pathPose.poses[i].position.y, poseEst.pose.position.y) >= BOUNDARY_RADIUS ) // FIXME: Implement this sleep cycle as a function
                {
                    ros::spinOnce();
                    loop_rate.sleep();
                    if(newPath || !ros::ok())
                    {
                    break;
                    }
                }
              }
              else // use interpolation
              {
                  if(onPath)
                  {
                      findClosestPointOnLine();
                      closestPointOnLine.pose.orientation = tf::createQuaternionMsgFromYaw(0);
                      calcConstVelTerm();
                      velPub.publish(constVelTerm);
                      goalPub.publish(closestPointOnLine);
                  }
              }
            }
          }
          else // path given is CLOSED
          {
            newPath = false; // reset flag and set stopping condition for while loop
            // FIXME: rearrange path
            while(!newPath) // while no new path has been published
            {
              if(newPath || !ros::ok())
                {
                    break;
                }
                
              // Publish array of pose
              for(int i = 0; i < pathPose.poses.size(); i++)
              {
                if(newPath || !ros::ok())
                {
                  break;
                }
                if ( (pathPose.poses[i].position.x == 0) && (pathPose.poses[i].position.y == 0) ){
                    break;
                }
                goalPose.pose = (pathPose.poses)[i];
                goalPose.pose.orientation = tf::createQuaternionMsgFromYaw(0);
                goalPub.publish(goalPose);
                while(distanceFormula(pathPose.poses[i].position.x, poseEst.pose.position.x,
                                    pathPose.poses[i].position.y, poseEst.pose.position.y) >= BOUNDARY_RADIUS )
                {
                    ros::spinOnce();
                    loop_rate.sleep();
                    if(newPath || !ros::ok())
                    {
                        break;
                    }
                }
              }
            }
          }
        }
        
        loop_rate.sleep();
    }
}
//END
