// This node is a simple way to approximate ros units to camera pixel conversion.
// To do this, note initial robot position (camera), then run this node. note final robot position (camera)

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include <tf/tf.h>
#include <stdlib.h> 

int main(int argc, char **argv)
{
  
ros::init(argc, argv, "rosUnitsMove");

std::cout<<"Checkpoint";
geometry_msgs::Twist cmd_vel_;
  ros::NodeHandle ph_, nh_;
  ros::Publisher vel_pub_;
  vel_pub_ = nh_.advertise<geometry_msgs::Twist>("mobile_base/commands/velocity", 1, true);
  cmd_vel_.angular.z = 0.0;
  cmd_vel_.linear.x = .1;
  int x=1;
  while (x<5000){
vel_pub_.publish(cmd_vel_);
x=x+1;
sleep(.1);
}
/*sleep(5);
cmd_vel_.angular.z=0;
cmd_vel_.linear.x=0;
vel_pub_.publish(cmd_vel_);
*/
}
