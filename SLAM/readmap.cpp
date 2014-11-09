#include "ros/ros.h"
#include "std_msgs/String.h"
#include "math.h"
#include "nav_msgs/OccupancyGrid.h"
#include "nav_msgs/GetMap.h"
#include "nav_msgs/MapMetaData.h"

#include <sstream>
#include <stdio.h>

void mapCallback(const nav_msgs::OccupancyGrid::ConstPtr& map)
{
  
  ROS_INFO("Received map");
  int width = map->info.width;
  int height = map->info.height;
/*
  for( int k = 0; k<width*height; k++ )
  {
    if( map->data[k] != -1 )
    {
      ROS_INFO("%d at %d", map->data[k], k );
    }
  }
*/

  for( int j = 0; j<height; j++ )
  {
    for( int i = 0; i<width; i++)
    {
      if( map->data[j*width + i] != -1)
      {
        //ROS_INFO("%d (%d,%d)", map->data[(j*width + i)], i, j );
        printf("%d at (%d,%d)\n", map->data[(j*width + i)], i, j );
      }
    }
  }

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
  ros::init(argc, argv, "readmap");

  /**
   * NodeHandle is the main access point to communications with the
   * ROS system. The first NodeHandle constructed will fully initialize
   * this node, and the last NodeHandle destructed will close down
   * the node.
   */
  ros::NodeHandle n;


  // get map data
  ros::ServiceClient getMap = n.serviceClient<nav_msgs::GetMap>("/map");

  nav_msgs::GetMap mapgetter;

  // get map data, method 2
  ros::Subscriber sub_map = n.subscribe("/map", 100, mapCallback);



  ros::Rate loop_rate(1);

  /**
   * A count of how many messages we have sent. This is used to create
   * a unique string for each message.
   */
  int count = 0;
  while (ros::ok())
  {
    /*
     // From http://answers.ros.org/question/11173/map_server-getmap/
     if (getMap.call(mapgetter))
     {
        ROS_INFO("Map service called successfully");
        const nav_msgs::OccupancyGrid& map (mapgetter.response.map);
        //do something with the map
     }
     else
     {
       ROS_ERROR("Failed to call map service");
     }
      */
    
    ros::spinOnce();

    loop_rate.sleep();
    ++count;
  }


  return 0;
}
