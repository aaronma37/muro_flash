/*

Cyclic Pursuit


*/

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include "PoseWithName.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include <tf/tf.h>
#include <math.h>

double rad1,x0, z;
double yc;
double rad2,x2,y2,radN;


void selfCallback(const turtlebot_deployment::PoseWithName::ConstPtr& selfPtr)
{
x0=selfPtr->pose.x-350;
//y0=selfPtr->pose.y-250;
z=yc/x0
rad1=atan(z);
}

void allPoseCallback(const turtlebot_deployment::PoseWithName::ConstPtr& posePtr)
{
x2=posePtr->pose.x-350;
y2=posePtr->pose.y-250;
rad2=atan(y2/x2);
  if((rad2-rad1)>3.14){
    rad2=rad2-(rad1+2*pi);
  }
  else{
    rad2=rad2-rad1;
  }

  if (rad2>0 && rad2<radN){
    radN=rad2;
  }

}

int main(int argc, char **argv)
{
ros::init(argc, argv, "cyclic_pursuit");
ros::NodeHandle ph_, nh_;
ros::Publisher vel_pub_;
geometry_msgs::Twist cmd_vel_;
ros::Rate loop_rate(.2);
vel_pub_ = nh_.advertise<geometry_msgs::Twist>("velocity", 1, true);
pos_sub_ = nh_.subscribe<turtlebot_deployment::PoseWithName>("\all_positions", 1,allPoseCallback);
self_sub_ = nh_.subscribe<turtlebot_deployment::PoseWithName>("afterKalman",1,selfCallback)
cmd_vel_.linear.x=10;
double k=1;
radN=1;
rad2=1;
rad1=1;
y0=0;
y2=0;
x0=0;
x2=0;
while (1==1){
ros::spinOnce();
cmd_vel_.linear.x=10*k*(radN);
vel_pub_.publish(cmd_vel_);
loop_rate.sleep();
}
}
