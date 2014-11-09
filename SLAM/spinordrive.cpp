#include "ros/ros.h"
#include "std_msgs/String.h"
#include "math.h"
#include "nav_msgs/OccupancyGrid.h"
#include "nav_msgs/GetMap.h"
#include "nav_msgs/MapMetaData.h"
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/Point.h"
#include "tf/transform_listener.h"
#include "tf/tf.h"


#include <sstream>
#include <stdio.h>

#define PI        3.141592654
#define HALF_PI   PI/2
#define TWO_PI    2*PI


nav_msgs::OccupancyGrid map;
geometry_msgs::Pose mappose;
tf::TransformListener* listener = NULL;

void mapCallback(const nav_msgs::OccupancyGrid::ConstPtr& msgmap)
{
  
  ROS_INFO("Received map");
  map = *msgmap;

}


/*
 * Look at points to left and right of the robot, on perpendicular line.  Return 1
 * if it sees a wall first, 0 if it sees unknown first.
 */
float checkSides(const geometry_msgs::Pose pose, const nav_msgs::OccupancyGrid map)
{
  geometry_msgs::Pose newpose = pose;
  float step = map.info.resolution;
  double angle = pose.orientation.z + HALF_PI;
  geometry_msgs::Point pixel;
  int value = 0;
  float output = 0;

  while(value == 0)
  {
    newpose.position.x += step * cos(angle);
    newpose.position.y += step * sin(angle);
    pixel.x = round( (newpose.position.x - map.info.origin.position.x)/map.info.resolution );
    pixel.y = round( (newpose.position.y - map.info.origin.position.y)/map.info.resolution );
    value =  map.data[(pixel.y*map.info.width + pixel.x)];
  }

  if( value == 100 )
  {
    output += 0.5;
  }
  
  // go the other way
  newpose = pose;

  while(value == 0)
  {
    newpose.position.x -= step * cos(angle);
    newpose.position.y -= step * sin(angle);
    pixel.x = round( (newpose.position.x - map.info.origin.position.x)/map.info.resolution );
    pixel.y = round( (newpose.position.y - map.info.origin.position.y)/map.info.resolution );
    value =  map.data[(pixel.y*map.info.width + pixel.x)];
  }

  if( value == 100 )
  {
    output += 0.5;
  }

  return output;
  
}


/**
 * Test reading in maps from gmapping.
 */
int main(int argc, char **argv)
{
  /**
   * The ros::init() function needs to see argc and argv so that it can 
   * perform any ROS arguments and name remapping that were provided at
   * the command line. For programmatic remappings you can use a
   * different version of init() which takes remappings directly, but
   * for most command-line programs, passing argc and argv is the easiest
   * way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any
   * other part of the ROS system.
   */
  ros::init(argc, argv, "spinordrive");

  /**
   * NodeHandle is the main access point to communications with the
   * ROS system. The first NodeHandle constructed will fully initialize
   * this node, and the last NodeHandle destructed will close down
   * the node.
   */
  ros::NodeHandle n;

  // get map data, method 2
  ros::Subscriber sub_map = n.subscribe("/map", 100, mapCallback);

  // listen to turtlebot pose tf within map
  //tf::TransformListener listener;
  listener = new tf::TransformListener(n, ros::Duration(10), true);

  ros::Rate loop_rate(10);

  /**
   * A count of how many messages we have sent.
   */
  int count = 0;
  while (ros::ok())
  {
    tf::StampedTransform transform;
    try
    {
      listener->lookupTransform("/map", "/base_link", ros::Time(0), transform);
    }
    catch (tf::TransformException ex)
    {
      ROS_ERROR("%s", ex.what());
    }
    
    mappose.position.x = transform.getOrigin().x();
    mappose.position.y = transform.getOrigin().y();
    mappose.position.z = transform.getOrigin().z();
    mappose.orientation.x = transform.getRotation().x();
    mappose.orientation.y = transform.getRotation().y();
    mappose.orientation.z = transform.getRotation().z();
    mappose.orientation.w = transform.getRotation().w();

    ROS_INFO("Current Pose: [%.3f, %.3f, %1.1f], [%1.1f, %1.1f, %.3f]",
        mappose.position.x, mappose.position.y, mappose.position.z,
        mappose.orientation.x, mappose.orientation.y, mappose.orientation.z);


    // check whether the area to our sides is known or not.
    //for( 

    ros::spinOnce();

    loop_rate.sleep();
    ++count;
  }


  return 0;
}
