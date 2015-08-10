#include "ros/ros.h"
#include "std_msgs/String.h"
#include <stdlib.h>
#include <sstream>
#include <tf/tf.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "geometry_msgs/Pose.h"	
#include "geometry_msgs/PoseStamped.h"	
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <stdlib.h>

//#include <nav_msgs.h>
using namespace std;
float x[10]={1,2,3,4,5,6,7,8,9,10};
float y[10]={1,2,3,4,5,6,7,8,9,10};
string names[3]={"BoB","Rick","Aaron"};

int main(int argc, char **argv)
{
  ros::init(argc, argv, "talker");
  ros::NodeHandle n;
  ros::Publisher chatter_pub = n.advertise<geometry_msgs::PoseStamped>("/tf", 1000);
//ros::Publisher chatter_pub3 = n.advertise<nav_msgs::Odometry>("/vo", 1000);

ros::Publisher chatter_pub2 = n.advertise<geometry_msgs::Twist>("cmd_vel", 1000);


ros::Rate loop_rate(200);
geometry_msgs::PoseStamped Pose;
geometry_msgs::PoseStamped Pose2;
geometry_msgs::PoseStamped Pose3;
geometry_msgs::PoseStamped Pose4;
geometry_msgs::PoseStamped Pose5;		
//turtlebot_deployment::PoseWithName Pose;
Pose.header.seq=1;
Pose2.header.seq=2;

Pose.header.frame_id="Bob";
Pose2.header.frame_id="Frank";
Pose3.header.frame_id="Richard";
Pose4.header.frame_id="Aaron";
Pose5.header.frame_id="Eric";

Pose.pose.position.x=0;
Pose2.pose.position.x=.1;
Pose3.pose.position.x=-.1;
Pose4.pose.position.x=0;
Pose5.pose.position.x=-.2;


Pose.pose.position.y=0;
Pose2.pose.position.y=-.2;
Pose3.pose.position.y=.2;
Pose4.pose.position.y=.6;
Pose5.pose.position.y=-.1;



Pose.pose.orientation=tf::createQuaternionMsgFromYaw(0);
Pose2.pose.orientation=tf::createQuaternionMsgFromYaw(0);
Pose3.pose.orientation=tf::createQuaternionMsgFromYaw(0);
Pose4.pose.orientation=tf::createQuaternionMsgFromYaw(0);
Pose5.pose.orientation=tf::createQuaternionMsgFromYaw(0);

	geometry_msgs::Twist t;
t.linear.x=.001;
t.angular.z=.005;
float yaw=0;
float yaw2=50;
  int count = 0;

 while (ros::ok()){

Pose.pose.position.x=Pose.pose.position.x+7*t.linear.x*.1*cos(yaw);
Pose2.pose.position.x=Pose2.pose.position.x+5*t.linear.x*.1*cos(yaw);
Pose3.pose.position.x=Pose3.pose.position.x+5*t.linear.x*.1*cos(yaw2);
Pose4.pose.position.x=Pose4.pose.position.x+1.5*t.linear.x*.1*cos(yaw);
Pose5.pose.position.x=Pose5.pose.position.x+1.5*t.linear.x*.1*cos(yaw);


Pose.pose.position.y=Pose.pose.position.y+t.linear.x*.1*sin(yaw);
Pose2.pose.position.y=Pose2.pose.position.y+5*t.linear.x*.1*sin(yaw);
Pose3.pose.position.y=Pose3.pose.position.y+5*t.linear.x*.1*sin(yaw2);
Pose4.pose.position.y=Pose4.pose.position.y+1.5*t.linear.x*.1*sin(yaw);
Pose5.pose.position.y=Pose5.pose.position.y+4*t.linear.x*.1*sin(yaw);


yaw=yaw+t.angular.z;
yaw2= yaw2 +2*t.angular.z;
Pose.pose.orientation=tf::createQuaternionMsgFromYaw(yaw);
Pose2.pose.orientation=tf::createQuaternionMsgFromYaw(yaw);
Pose3.pose.orientation=tf::createQuaternionMsgFromYaw(yaw2);
Pose4.pose.orientation=tf::createQuaternionMsgFromYaw(yaw);
Pose5.pose.orientation=tf::createQuaternionMsgFromYaw(yaw);

chatter_pub2.publish(t);
chatter_pub.publish(Pose);
/*if (count % 5 ==0){
chatter_pub.publish(Pose);}
else if (count % 5 ==1){
chatter_pub.publish(Pose2);}
else if (count % 5 == 2){
chatter_pub.publish(Pose3);
}
else if (count % 5 == 3){
chatter_pub.publish(Pose4);
}
else if (count % 5 == 4){
chatter_pub.publish(Pose5);
}*/

std::cout<<"Check echo \n";
count=count+1;
//sleep(1);
loop_rate.sleep();


}
}
