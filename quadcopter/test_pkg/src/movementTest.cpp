
#include "ros/ros.h"
#include "std_msgs/String.h"
#include <stdlib.h>
#include <sstream>
#include <tf/tf.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "geometry_msgs/Pose.h"	
#include "geometry_msgs/PoseStamped.h"	
#include <vector>
#include <std_msgs/Empty.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <stdlib.h>

using namespace std;

int main(int argc, char **argv)
{
  ros::init(argc, argv, "movement test");
  ros::NodeHandle n;
  ros::Publisher chatter_pub = n.advertise<std_msgs::Empty>("/ardrone/takeoff", 1000);
  ros::Publisher chatter_pub2 = n.advertise<std_msgs::Empty>("/ardrone/land", 1000);
  std_msgs::Empty myMsg;

ros::Rate loop_rate(200);
int count =0;
chatter_pub.publish(myMsg);
 while (ros::ok()){
  
  if (count > 1000){
    chatter_pub2.publish(myMsg);
    break;
  }
  else{
    chatter_pub.publish(myMsg);
  }
  
  count=count+1;
  loop_rate.sleep();
  }
}
