/*

Path Following algorithm from

http://www.control.utoronto.ca/people/profs/maggiore/DATA/PAPERS/CONFERENCES/ACC08_2.pdf

*/

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include <tf/tf.h>
#include <math.h>

//Declare Variables


// Construct Node Class
class pathFollowing
{
public:
pathFollowing();
private:
// Methods

void poseCallback(const geometry_msgs::Pose::ConstPtr&);
void velocityCallback(const geometry_msgs::Twist::ConstPtr&);
// ROS stuff
ros::NodeHandle ph_, nh_;
ros::Subscriber pos_sub_;
ros::Subscriber vel_sub_;
ros::Publisher u_pub_;
// Other member variables
geometry_msgs::Twist cmd_vel_;
geometry_msgs::Twist robVel;
geometry_msgs::Pose pose;
double robVel_;
bool got_vel_;
};

pathFollowing::pathFollowing():
/*cmd_vel_(new geometry_msgs::Twist),
*/got_vel_(false)
{
vel_sub_ = nh_.subscribe<geometry_msgs::Twist>("velocity",1, &pathFollowing::velocityCallback, this);
pos_sub_ = nh_.subscribe<geometry_msgs::Pose>("amcl_pose", 1, &pathFollowing::poseCallback, this);
u_pub_ = nh_.advertise<geometry_msgs::Twist>("mobile_base/commands/velocity", 1, true);

}

void pathFollowing::velocityCallback(const geometry_msgs::Twist::ConstPtr& robVel){
robVel_= robVel->linear.x;
got_vel_ = true;
}

void pathFollowing::poseCallback(const geometry_msgs::Pose::ConstPtr& pose)
{
if (got_vel_==true){
	
	/*
	
%% Results (linear about V=.1, omega=1
% deltaAngle = 18.2 1 ros angle/s to degrees/s(CCW), STD = 2.05
% dDistance  = 167  1 ros unit/s  to pixel/s         STD = 19.4
	
	*/
	double orientation = tf::getYaw(pose->orientation);
	double x1=pose->position.x;
	x1=x1-250;
	double x2=pose->position.y; //centered
	x2=x2-250;
	double r=100;
	double k=1;
	double u1=robVel_;
	double u2=robVel_/r+k*(r*x1*cos(orientation)+r*x2*sin(orientation)); //check orientation units
	cmd_vel_.linear.x=(u1/167);
	cmd_vel_.angular.z=(u2/18.2);
	u_pub_.publish(cmd_vel_);
//	got_vel_=false; *Delete
	}
}
int main(int argc, char **argv)
{
ros::init(argc, argv, "PathFollowing");
pathFollowing pathFollowingk;
ros::spin();
}
