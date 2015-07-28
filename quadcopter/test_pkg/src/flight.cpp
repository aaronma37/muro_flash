#include "ros/ros.h"
#include <geometry_msgs/Twist.h>
#include <std_msgs/Bool.h>

int main(int argc, char **argv)
{
  ROS_INFO("flight.cpp: START\n");
  ros::init(argc, argv, "flight");
  ros::NodeHandle n;
  ros::Publisher velPub;
  ros::Publisher goPub;
  velPub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 1000, true);
  goPub = n.advertise<std_msgs::Bool>("/flight", 1000, true);
  ros::Rate loop_rate(1);
  ROS_INFO("flight.cpp initialized\n");

  geometry_msgs::Twist vel;
  vel.linear.x = 0;
  vel.linear.y = 0;
  vel.linear.z = 0;
  velPub.publish(vel);
  ROS_INFO("flight: velocity published\n");
  
  loop_rate.sleep();
  ROS_INFO("1 sec\n");
  loop_rate.sleep();
  ROS_INFO("2 sec\n");
  loop_rate.sleep();
  ROS_INFO("3 sec\n");
  loop_rate.sleep();
  ROS_INFO("4 sec\n");
  loop_rate.sleep();
  ROS_INFO("5 sec\n");
  ROS_INFO("flight: finished sleeping\n");
  
  std_msgs::Bool go;
  go.data = true;
  goPub.publish(go);
  ROS_INFO("flight: go published\n");
  
  ros::spinOnce();

  ROS_INFO("flight.cpp: END\n");
  return 0;
}
