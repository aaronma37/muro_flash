#include "ros/ros.h"
#include <nav_msgs/Odometry.h>
#include <actionlib_msgs/GoalStatusArray.h>
#include <std_msgs/Header.h>
#include <actionlib_msgs/GoalStatus.h>
#include <geometry_msgs/PoseStamped.h>

actionlib_msgs::GoalStatusArray goalstatus;
bool hasbeencalledback = false;

void goalStatusCallback(const actionlib_msgs::GoalStatusArrayConstPtr& status)
{
  hasbeencalledback = true;
  goalstatus = *status;
  if (!goalstatus.status_list.empty())
    ROS_INFO("Current status: %d", goalstatus.status_list[0].status);
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "amclgoalset");

  //object to interact with ROS
  ros::NodeHandle n;

  ros::Rate loop_rate(2);

  ros::Subscriber goalStatusSub = n.subscribe<actionlib_msgs::GoalStatusArray>("/move_base/status", 1000, goalStatusCallback);
  ros::Publisher  newGoalPub = n.advertise<geometry_msgs::PoseStamped>("/move_base_simple/goal",1000);

  geometry_msgs::PoseStamped mygoal;
  geometry_msgs::Point positiongoal;
  geometry_msgs::Quaternion orientationgoal;
  std_msgs::Header goalheader;

  bool haspublished1 = false;
  bool haspublished2 = false;
  bool hasbeenactive = false;
  bool keeplooping = true;


  positiongoal.x = -2.13;
  positiongoal.y = -1.27;
  positiongoal.z = 0.0;

  orientationgoal.x = 0.0;
  orientationgoal.y = 0.0;
  orientationgoal.z = 0.2417;
  orientationgoal.w = 0.9704;
      
  goalheader.frame_id = "map";

  mygoal.pose.position = positiongoal;
  mygoal.pose.orientation = orientationgoal;
  mygoal.header = goalheader;

  newGoalPub.publish(mygoal);

  while (ros::ok() && keeplooping) 
  {
  
  if (hasbeencalledback)
  {
    if (goalstatus.status_list.empty())
    {
      
    }
    else
    {
      if (!haspublished1)
      {
        positiongoal.x = -2.13;
        positiongoal.y = -1.27;
        positiongoal.z = 0.0;

        orientationgoal.x = 0.0;
        orientationgoal.y = 0.0;
        orientationgoal.z = 0.2417;
        orientationgoal.w = 0.9704;
      
        goalheader.frame_id = "map";

        mygoal.pose.position = positiongoal;
        mygoal.pose.orientation = orientationgoal;
        mygoal.header = goalheader;

        newGoalPub.publish(mygoal);
        haspublished1 = true;
      }

      if (goalstatus.status_list[0].status == 1)  hasbeenactive = true;

      if (goalstatus.status_list[0].status == 3 && hasbeenactive && !haspublished2)
      {
        sleep(5);

        positiongoal.x = 3.00;
        positiongoal.y = -0.80;
        positiongoal.z = 0.0;

        orientationgoal.x = 0.0;
        orientationgoal.y = 0.0;
        orientationgoal.z = -0.5424;
        orientationgoal.w = 0.8401;
      
        goalheader.frame_id = "map";

        mygoal.pose.position = positiongoal;
        mygoal.pose.orientation = orientationgoal;
        mygoal.header = goalheader;

        newGoalPub.publish(mygoal);
        haspublished2 = true;
        hasbeenactive = false;
      }
      if (goalstatus.status_list[0].status == 3 && hasbeenactive && haspublished2)
        keeplooping = false;
    }
  }
  
  ros::spinOnce();

  loop_rate.sleep();
  }
  return 0;
}
