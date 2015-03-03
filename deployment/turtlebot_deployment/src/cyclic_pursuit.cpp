/*

Cyclic Pursuit


*/

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include <tf/tf.h>
#include <math.h>

//Declare Variables

/*
// Construct Node Class
class cyclicPursuit
{
public:
cyclicPursuit();
private:
// Methods

void poseCallback(const geometry_msgs::Pose::ConstPtr&);
// ROS stuff

ros::Subscriber pos_sub_;

// Other member variables

geometry_msgs::Twist robVel;
geometry_msgs::Pose pose;
double velocity;
};

cyclicPursuit::cyclicPursuit()
{
pos_sub_ = nh_.subscribe<geometry_msgs::Pose>("/position", 1, &cyclicPursuit::poseCallback, this);
}


void cyclicPursuit::poseCallback(const geometry_msgs::Pose::ConstPtr& pose)
{
//THIS IS WHERE CYCLIC PURSUIT ALGORITHM GOES

}*/
int main(int argc, char **argv)
{
ros::init(argc, argv, "cyclic_pursuit");
ros::NodeHandle ph_, nh_;
ros::Publisher vel_pub_;
geometry_msgs::Twist cmd_vel_;
vel_pub_ = nh_.advertise<geometry_msgs::Twist>("velocity", 1, true);
cmd_vel_.linear.x=15;
while (1==1){
vel_pub_.publish(cmd_vel_);
//cyclicPursuit cp;
}ros::spin();
}
