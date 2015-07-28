#include "ros/ros.h"
#include <geometry_msgs/Twist.h>
#include <std_msgs/Bool.h>

int main(int argc, char **argv)
{
  
  ros::init(argc, argv, "flight");
  ros::NodeHandle n;
  ros::Publisher velPub;
  ros::Publisher goPub;
  velPub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 1000, true);
  goPub = n.advertise<std_msgs::Bool>("/flight", 1000, true);
  ros::Rate loop_rate(1);

  geometry_msgs::Twist vel;
  vel.linear.x = 0;
  vel.linear.y = 0;
  vel.linear.z = 0;
  velPub.publish(vel);
  
  loop_rate.sleep();
  loop_rate.sleep();
  loop_rate.sleep();
  loop_rate.sleep();
  loop_rate.sleep();
  
  std_msgs::Bool go;
  go.data = true;
  goPub.publish(go);
  
  ros::spinOnce();

  return 0;
}
