// This node is a simple way to approximate ros units to camera pixel conversion.
// To do this, note initial robot position (camera), then run this node. note final robot position (camera)

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include <tf/tf.h>
#include <stdlib.h> 
#include <time.h>   

int main(int argc, char **argv)
{
  
ros::init(argc, argv, "rosUnitsMove");
time_t timer,begin,end;

std::cout<<"Checkpoint";
geometry_msgs::Twist cmd_vel_;
  ros::NodeHandle ph_, nh_;
  ros::Publisher vel_pub_;
  vel_pub_ = nh_.advertise<geometry_msgs::Twist>("mobile_base/commands/velocity", 1, true);
  //cmd_vel_.angular.z = 0.0;
  //cmd_vel_.linear.x = .1;
  int speed=5;
  cmd_vel_.angular.z = speed;
  cmd_vel_.linear.x = 0;
  int x=1;
  time(&end);
  while(1==1){
    time(&begin);
          while (difftime(time(&timer),begin)<100){
        if (x % 500 ==0){
          cmd_vel_.angular.z = speed;
        vel_pub_.publish(cmd_vel_);
        usleep(100000);}
        else
        {
          cmd_vel_.angular.z = .1;
          vel_pub_.publish(cmd_vel_);
          usleep(100000);
        }
        x=x+1;
            
          }
    sleep(10);
  }
}
