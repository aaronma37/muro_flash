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
double x2, x1;
double orientation;
double robVel_;
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

}

void pathFollowing::velocityCallback(const geometry_msgs::Twist::ConstPtr& robVel){
robVel_= robVel->linear.x;
got_vel_ = true;
}

void pathFollowing::poseCallback(const turtlebot_deployment::PoseWithName::ConstPtr& Pose)
{
	orientation = tf::getYaw(Pose->pose.orientation);
	//orientation=-orientation;
	x1=Pose->pose.position.x;
	x1=x1-350;
	x2=Pose->pose.position.y; //centered
	x2=x2-200;
	
//	got_vel_=false; *Delete
	
}
int main(int argc, char **argv)
{
ros::init(argc, argv, "PathFollowing");
u_pub_ = nh_.advertise<geometry_msgs::Twist>("mobile_base/commands/velocity", 1, true);
pathFollowing pathFollowingk;
while (1==1){
	ros::spinonce();
        double r=50;
	double k=1;
	double u1=robVel_;
	double u2=robVel_/r;
	std::cout<<"initial angular velocity: \n"<<u2<<"\n\n";
	u2=u2-k*(r*x1*cos(orientation)+r*x2*sin(orientation))/167/167; //check orientation units
	std::cout<<"final angular velocity: \n"<<u2<<"\n\n";
	cmd_vel_.linear.x=(u1/167);
	cmd_vel_.angular.z=(u2*1.45);
	u_pub_.publish(cmd_vel_);}
}
