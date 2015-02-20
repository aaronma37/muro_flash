/*

Path Following algorithm from

http://www.control.utoronto.ca/people/profs/maggiore/DATA/PAPERS/CONFERENCES/ACC08_2.pdf

*/

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include <turtlebot_deployment/PoseWithName.h>
#include <tf/tf.h>
#include <math.h>

//Declare Variables


// Construct Node Class
class pathFollowing
{
public:
pathFollowing();
std::string this_agent_;
private:
// Methods

void poseCallback(const turtlebot_deployment::PoseWithName::ConstPtr&);
void velocityCallback(const geometry_msgs::Twist::ConstPtr&);
// ROS stuff
ros::NodeHandle ph_, nh_;
ros::Subscriber pos_sub_;
ros::Subscriber vel_sub_;
ros::Publisher u_pub_;
// Other member variables
geometry_msgs::Twist cmd_vel_;
geometry_msgs::Twist robVel;
turtlebot_deployment::PoseWithName Pose;
double robVel_;
bool got_vel_;
};

pathFollowing::pathFollowing():
/*cmd_vel_(new geometry_msgs::Twist),
*/got_vel_(false),
ph_("~"),
this_agent_()
{
ph_.param("robot_name", this_agent_,this_agent_);
vel_sub_ = nh_.subscribe<geometry_msgs::Twist>("velocity",1, &pathFollowing::velocityCallback, this);
pos_sub_ = nh_.subscribe<turtlebot_deployment::PoseWithName>("/all_positions", 1, &pathFollowing::poseCallback, this);
u_pub_ = nh_.advertise<geometry_msgs::Twist>("mobile_base/commands/velocity", 1, true);

}

void pathFollowing::velocityCallback(const geometry_msgs::Twist::ConstPtr& robVel){
robVel_= robVel->linear.x;
got_vel_ = true;
}

void pathFollowing::poseCallback(const turtlebot_deployment::PoseWithName::ConstPtr& Pose)
{
if (got_vel_==true){
	
	/*
	
%% Results (linear about V=.1, omega=1
% deltaAngle = 18.2 1 ros angle/s to degrees/s(CCW), STD = 2.05
% dDistance  = 167  1 ros unit/s  to pixel/s         STD = 19.4
	
	*/
	double orientation = tf::getYaw(Pose->pose.orientation);
	double x1=Pose->pose.position.x;
	x1=x1-313;
	double x2=Pose->pose.position.y; //centered
	x2=x2-200;
	double r=5;
	double k=10;
	double u1=robVel_;
	double u2=robVel_/r+k*(r*x1*cos(orientation)+r*x2*sin(orientation)); //check orientation units
	cmd_vel_.linear.x=(u1/167);
	cmd_vel_.angular.z=(u2/167/18.2);
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
