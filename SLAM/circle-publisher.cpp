#include "ros/ros.h"
//include the types of messages you'll be publishing
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>
//include other stuff that's just generally required for publishing things
#include <stdlib.h>
#include <sstream>


int main(int argc, char **argv)
{
  ros::init(argc, argv, "publisher");

  //object to interact with ROS
  ros::NodeHandle n;

  //declare pi for easy turns in radians
  float pi = 3.14159;

  //create publisher object publishing data of type geometry_msgs/Twist on topic name /mobile_base/commands/velocity, with buffer list size of 1000
  ros::Publisher vPub = n.advertise<geometry_msgs::Twist>("/mobile_base/commands/velocity", 1000);
  //publisher object specifically for giotto
  ros::Publisher giottoPub = n.advertise<geometry_msgs::Twist>("/giottobot/mobile_base/commands/velocity", 1000);
  //publisher object specifically for bellini
  ros::Publisher belliniPub = n.advertise<geometry_msgs::Twist>("/bellinibot/mobile_base/commands/velocity", 1000);
  //publisher object specifically for titian
  ros::Publisher titianPub = n.advertise<geometry_msgs::Twist>("/titianbot/mobile_base/commands/velocity", 1000);
  //publisher object specifically for boticelli
  ros::Publisher boticelliPub = n.advertise<geometry_msgs::Twist>("/boticellibot/mobile_base/commands/velocity", 1000);

  ros::Rate loop_rate(10); //set loop rate to 10 hz

  //check every .1 seconds for a subscriber, sleep while waiting, don't proceed until someone has subscribed
  //while(vPub.getNumSubscribers() == 0) ros::Duration(0.1).sleep();

  int count = 0;
  while (ros::ok())
  {
    //stuff partial circle message with data, then publish it
    geometry_msgs::Twist partialCircle;

    partialCircle.linear.x=.5;
    partialCircle.angular.z=pi/2;

    ROS_INFO("partial circle #%d, linear:%f,angular:%f", count, partialCircle.linear.x,partialCircle.angular.z);

    vPub.publish(partialCircle);
    giottoPub.publish(partialCircle);
    belliniPub.publish(partialCircle);
    titianPub.publish(partialCircle);
    boticelliPub.publish(partialCircle);

    ros::spinOnce();

    //this makes the loop run at the loop rate, whch is 10 hz, declared earlier
    loop_rate.sleep();

    ++count;
  }

  return 0;
}
