/*

Cyclic Pursuit


*/

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include <tf/tf.h>
#include <math.h>

void allPoseCallback(const turtlebot_deployment::PoseWithName::ConstPtr& posePtr)
{
  

}
int main(int argc, char **argv)
{
ros::init(argc, argv, "cyclic_pursuit");
ros::NodeHandle ph_, nh_;
ros::Publisher vel_pub_;
geometry_msgs::Twist cmd_vel_;
vel_pub_ = nh_.advertise<geometry_msgs::Twist>("velocity", 1, true);
pos_sub_ = nh_.subscribe<turtlebot_deployment::PoseWithName>("\all_positions", 1,allPoseCallback);
self_sub_ = nh_.subscribe<turtlebot_deployment::PoseWithName>("afterKalman",1,selfCallback)
cmd_vel_.linear.x=10;
while (1==1){
vel_pub_.publish(cmd_vel_);
//cyclicPursuit cp;
}ros::spin();
}
