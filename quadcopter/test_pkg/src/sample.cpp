#include "ros/ros.h"
#include "std_msgs/String.h"
#include <stdlib.h>
#include <sstream>
#include <tf/tf.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "geometry_msgs/Pose.h"	
#include "geometry_msgs/PoseArray.h"	
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
  ros::Publisher chatter_pub = n.advertise<geometry_msgs::PoseArray>("/toVoronoiDeployment", 1000);
//ros::Publisher chatter_pub3 = n.advertise<nav_msgs::Odometry>("/vo", 1000);

//ros::Publisher chatter_pub2 = n.advertise<geometry_msgs::Twist>("cmd_vel", 1000);


ros::Rate loop_rate(200);
geometry_msgs::PoseArray pArray;
pArray.poses.resize(5);

pArray.poses[0].position.x=0;
pArray.poses[1].position.x=.1;
pArray.poses[2].position.x=-.1;
pArray.poses[3].position.x=0;
pArray.poses[4].position.x=-.2;


pArray.poses[0].position.y=0;
pArray.poses[1].position.y=-.2;
pArray.poses[2].position.y=.2;
pArray.poses[3].position.y=.6;
pArray.poses[4].position.y=-.1;



pArray.poses[0].orientation=tf::createQuaternionMsgFromYaw(0);
pArray.poses[1].orientation=tf::createQuaternionMsgFromYaw(0);
pArray.poses[2].orientation=tf::createQuaternionMsgFromYaw(0);
pArray.poses[3].orientation=tf::createQuaternionMsgFromYaw(0);
pArray.poses[4].orientation=tf::createQuaternionMsgFromYaw(0);

	geometry_msgs::Twist t;
t.linear.x=.001;
t.angular.z=.005;
float yaw=0;
float yaw2=50;
  int count = 0;

 while (ros::ok()){

pArray.poses[0].position.x=pArray.poses[0].position.x+7*t.linear.x*.1*cos(yaw);
pArray.poses[1].position.x=pArray.poses[1].position.x+5*t.linear.x*.1*cos(yaw);
pArray.poses[2].position.x=pArray.poses[2].position.x+5*t.linear.x*.1*cos(yaw2);
pArray.poses[3].position.x=pArray.poses[3].position.x+1.5*t.linear.x*.1*cos(yaw);
pArray.poses[4].position.x=pArray.poses[4].position.x+1.5*t.linear.x*.1*cos(yaw);


pArray.poses[0].position.y=pArray.poses[0].position.y+t.linear.x*.1*sin(yaw);
pArray.poses[1].position.y=pArray.poses[1].position.y+5*t.linear.x*.1*sin(yaw);
pArray.poses[2].position.y=pArray.poses[2].position.y+5*t.linear.x*.1*sin(yaw2);
pArray.poses[3].position.y=pArray.poses[3].position.y+1.5*t.linear.x*.1*sin(yaw);
pArray.poses[4].position.y=pArray.poses[4].position.y+4*t.linear.x*.1*sin(yaw);


yaw=yaw+t.angular.z;
yaw2= yaw2 +2*t.angular.z;
pArray.poses[0].orientation=tf::createQuaternionMsgFromYaw(yaw);
pArray.poses[1].orientation=tf::createQuaternionMsgFromYaw(yaw);
pArray.poses[2].orientation=tf::createQuaternionMsgFromYaw(yaw2);
pArray.poses[3].orientation=tf::createQuaternionMsgFromYaw(yaw);
pArray.poses[4].orientation=tf::createQuaternionMsgFromYaw(yaw);

chatter_pub.publish(pArray);
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
