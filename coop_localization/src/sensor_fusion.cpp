//Jonathan Hechtbauer, UCSD, MURO, 2016

#include <ros/ros.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h> // getenv 	
#include <coop_localization/PoseRelative.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/Quaternion.h"
#include "nav_msgs/Odometry.h"
#include <tf/tf.h>
#include <tf/transform_broadcaster.h>
#include <tf/transform_datatypes.h>
#include <bfl/wrappers/matrix/matrix_wrapper.h> //MatrixWrapper::SymmetricMatrix Class 


using namespace std;


const double PI = 3.14159265;

string temp_name = "test";
string RobotName;

bool new_cmd_flag = false, new_prop_data_flag = false, new_exter_data_flag = false;
float X_init[3]={0,0,0};
float X_old[3];
float X_new_pred[3];
float X_new[3];
double lin_vel = 0;
double ang_vel = 0;
double roll, pitch, yaw = 0; 		//TODO this is only set for testing
double prop_Z[3];




MatrixWrapper::SymmetricMatrix prop_covariance_;


//initialise start
ros::Time filter_time_old, filter_stamp;
ros::Time prop_stamp, exter_stamp;
ros::Time time_new, time_old;

tf::StampedTransform prop_tf;
//initialise end


ros::Publisher pub;
geometry_msgs::PoseWithCovarianceStamped pose_msg;
//coop_localization::PoseRelative msg_rel;

void publish_ros_msg()
{
	ros::Time scan_time = ros::Time::now();
	pose_msg.header.stamp = scan_time;
	pose_msg.header.frame_id = RobotName+"_baselink";
	pose_msg.pose.pose.position.x = 10;
	pose_msg.pose.pose.position.y = 10;
	//pose_msg.pose.pose.orientation.z = 10; !!Quaternion
	//pose_msg.pose.covariance = {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 99};
	pose_msg.pose.covariance[0] = 10;
	pub.publish(pose_msg);
}


bool update_filter(const ros::Time&  filter_time)
{
	// only update filter for time later than current filter time
	double dt = (filter_time - filter_time_old).toSec();
	if (dt == 0) return false;
	if (dt <  0){
		ROS_INFO("Will not update robot pose with time %f sec in the past.", dt);
		return false;
	}
	ROS_DEBUG("Update filter at time %f with dt %f", filter_time.toSec(), dt);
	filter_time_old = filter_time;
}

void sensor_fusion()
{ 

	X_old[0] = X_new[0];
	X_old[1] = X_new[1];
	X_old[2] = X_new[2];
	time_new = ros::Time::now();

	////PREDICTION


	if(new_cmd_flag)
	{
		double dt = (time_new - time_old).toSec();
		//X_new_pred = A*X_old + B*U_new		//What is A and B??? Linearization??? =1?
		X_new_pred[0] = X_old[0]+lin_vel*cos(yaw)*dt;   //TODO roll must be received from pose
		X_new_pred[1] = X_old[1]+lin_vel*sin(yaw)*dt;
		X_new_pred[2] = X_old[2]+ang_vel*dt;
	}


	////UPDATE
	if(new_prop_data_flag || new_exter_data_flag)
	{
		double P_new_pred = 1; //TODO
		double H = 1; //TODO vector and what is it?
		double H_transp = H; //TODO
		//K = P_new_pred*H_transp*invers(H*P_new_pred*H_transp+R)
		double K[3] = {1, 1, 1};
		X_new[0] = X_new_pred[0] + K[0] * (prop_Z[0]-H*X_new_pred[0]);
		X_new[1] = X_new_pred[1] + K[1] * (prop_Z[1]-H*X_new_pred[1]);
		X_new[2] = X_new_pred[2] + K[2] * (prop_Z[2]-H*X_new_pred[2]);
	}

	prop_stamp = ros::Time::now(); //TODO Initialize it in the beginning. or. Get this from the message
	exter_stamp = ros::Time::now(); //TODO Initialize it in the beginning. or. Get this from the message









	////MEASUREMENT

	//Linearization

	//Proprioceptive sensor - Wheel encoder
	//Check if new data!!

	//Exteroceptive sensor - AR tags
	//Check if new data!!


	//DO STUFF



	//reset the flags
	new_prop_data_flag = false;
	new_exter_data_flag = false;
	new_cmd_flag = true;



}

























	



//This function is called everytime a new rel_pose is published for this turtlebot
void pose_exter_cb(const coop_localization::PoseRelative& pose_exter) // relative measurements with the AR tags
{ 
	new_exter_data_flag = true;

}

//This function is called everytime a new odom is published for this turtlebot
void pose_prop_cb(const nav_msgs::Odometry pose_prop) //wheel odometry
	//nav_msgs/Odometry Message:
	//std_msgs/Header header
	//string child_frame_id
	//geometry_msgs/PoseWithCovariance pose
	//geometry_msgs/TwistWithCovariance twist		-> for velocity
{ 
	static tf::TransformBroadcaster br;

	prop_stamp = pose_prop.header.stamp;
	prop_Z[0] = pose_prop.pose.pose.position.x;
	prop_Z[1] = pose_prop.pose.pose.position.y;

	geometry_msgs::Quaternion q_msg = pose_prop.pose.pose.orientation;
	tf::Quaternion q(q_msg.x, q_msg.y, q_msg.z, q_msg.w);
	tf::Matrix3x3 m(q);
	m.getRPY(roll, pitch, yaw);


    	for (unsigned int i=0; i<3; i++)
      		for (unsigned int j=0; j<3; j++)
        		prop_covariance_(i+1, j+1) = pose_prop.pose.covariance[3*i+j]; //TODO 
	new_prop_data_flag = true;
}

//This function is called everytime a new cmd is published for this turtlebot
void cmd_cb(const geometry_msgs::Twist cmd)
{ 
	// Cmd will reset automatically to zero if no new command is sent
        lin_vel=cmd.linear.x;
        ang_vel=cmd.angular.z;


	new_cmd_flag = true;
}























//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~main~begin~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int main(int argc, char **argv)
{
	ros::init(argc, argv, "proprioceptive_sensor");
	ros::NodeHandle nh;
	
	ros::param::param<string>("~RobotName", RobotName, "noname");
	pub = nh.advertise<geometry_msgs::PoseWithCovarianceStamped>("pose", 100,true);
	filter_time_old = ros::Time::now(); //TODO

	//Initialization
	for (int i=0;i<=2;i++)
	{
		X_old[i] = X_init[i];
	}

  ros::Rate loop_rate(10); //10 Hz
  while (ros::ok()) //This loops runs until the program is terminated
  {
	ros::spinOnce();

	//Subscribe
	ros::Subscriber sub_exter = nh.subscribe("pose_measured", 1000, pose_exter_cb);
	ros::Subscriber sub_prop = nh.subscribe("odom", 1000, pose_prop_cb);
	ros::Subscriber sub_com=nh.subscribe("mobile_base/commands/velocity",1000, cmd_cb); 




	//Filter
	sensor_fusion();
	//Publish
	publish_ros_msg();
	loop_rate.sleep();
  }
}//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~main~end~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

