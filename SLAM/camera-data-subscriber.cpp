#include "ros/ros.h"
#include "std_msgs/String.h"
#include <nav_msgs/Odometry.h>
#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/Imu.h>


//subscribing callback functions for specific robots

void giottoOdomCallback(const nav_msgs::Odometry locator)
{
  ROS_INFO("Giotto Position: %f,%f", locator.pose.pose.position.x,locator.pose.pose.position.y);
  ROS_INFO("Giotto Heading: linear:%f,angular:%f", locator.twist.twist.linear.x,locator.twist.twist.angular.z);
}
void belliniOdomCallback(const nav_msgs::Odometry locator)
{
  ROS_INFO("Bellini Position: %f,%f", locator.pose.pose.position.x,locator.pose.pose.position.y);
  ROS_INFO("Bellini Heading: linear:%f,angular:%f", locator.twist.twist.linear.x,locator.twist.twist.angular.z);
}
void giottoScanCallback(const sensor_msgs::LaserScan& scanner)
{
  for (int i=0;i<scanner.ranges.size();i++)
  {
    ROS_INFO("Giotto Ranges[%d]=%f",i,scanner.ranges[i]);
  }
  for (int i=0;i<scanner.intensities.size();i++)
  {
    ROS_INFO("Giotto Intensities[%d]=%f",i,scanner.intensities[i]);
  }
}
void belliniScanCallback(const sensor_msgs::LaserScan& scanner)
{
  for (int i=0;i<scanner.ranges.size();i++)
  {
    ROS_INFO("Bellini Ranges[%d]=%f",i,scanner.ranges[i]);
  }
  for (int i=0;i<scanner.intensities.size();i++)
  {
    ROS_INFO("Bellini Intensities[%d]=%f",i,scanner.intensities[i]);
  }
}
void giottoPointCloudCallback(const sensor_msgs::PointCloud2& points)
{
  for (int p=0;p<points.data.size();p++)
  {
    ROS_INFO("Giotto PointCloud2 Data[%d]=%d",p,points.data[p]);
  }
}
void belliniPointCloudCallback(const sensor_msgs::PointCloud2& points)
{
  for (int p=0;p<points.data.size();p++)
  {
    ROS_INFO("Bellini PointCloud2 Data[%d]=%d",p,points.data[p]);
  }
}
void giottoRawImageCallback(const sensor_msgs::Image image)
{
  for (int i=0;i<image.data.size();i++)
  {
    ROS_INFO("Giotto Raw Image Data[%d]=%d",i,image.data[i]);
  }
}
void belliniRawImageCallback(const sensor_msgs::Image image)
{
  for (int i=0;i<image.data.size();i++)
  {
    ROS_INFO("Bellini Raw Image Data[%d]=%d",i,image.data[i]);
  }
}
void giottoImageColorCallback(const sensor_msgs::Image& image)
{
  for (int i=0;i<image.data.size();i++)
  {
    ROS_INFO("Giotto Image Color Data[%d]=%d",i,image.data[i]);
  }
}
void belliniImageColorCallback(const sensor_msgs::Image& image)
{
  for (int i=0;i<image.data.size();i++)
  {
    ROS_INFO("Bellini Image Color Data[%d]=%d",i,image.data[i]);
  }
}
//this one seems to be really similar to odom, but for now I'm keeping it here anyway
void giottoImuDataCallback(const sensor_msgs::Imu& locator)
{
  ROS_INFO("Giotto Position: %f,%f", locator.orientation.x,locator.orientation.y);
  ROS_INFO("Giotto Velocity: linear:%f,angular:%f", locator.angular_velocity.x,locator.angular_velocity.z);
  ROS_INFO("Giotto Acceleration: linear:%f", locator.linear_acceleration.x);
}
void belliniImuDataCallback(const sensor_msgs::Imu& locator)
{
  ROS_INFO("Bellini Position: %f,%f", locator.orientation.x,locator.orientation.y);
  ROS_INFO("Bellini Velocity: linear:%f,angular:%f", locator.angular_velocity.x,locator.angular_velocity.z);
  ROS_INFO("Bellini Acceleration: linear:%f", locator.linear_acceleration.x);
}



int main(int argc, char **argv)
{

  ros::init(argc, argv, "subscriber");

  //object to interact with ROS
  ros::NodeHandle n;

  //declare pi for easy turns in radians
  float pi = 3.14159;

  //subscribe to the topics rviz subscribes to (right now just on the unupdated turtlebots, because of the camera problems with the updated ones)

  //odom
  //subscribe to /giottobot/odom
  ros::Subscriber giottoOdomSub = n.subscribe("/giottobot/odom", 1000, giottoOdomCallback);
  //subscribe to /bellinibot/odom
  ros::Subscriber belliniOdomSub = n.subscribe("/bellinibot/odom", 1000, belliniOdomCallback);

  //scan
  //subscribe to /giottobot/scan
  ros::Subscriber giottoScanSub = n.subscribe("/giottobot/scan", 1000, giottoScanCallback);
  //subscribe to /bellinibot/scan
  ros::Subscriber belliniScanSub = n.subscribe("/bellinibot/scan", 1000, belliniScanCallback);

  //point cloud
  //subscribe to /giottobot/camera/depth_registered/points
  ros::Subscriber giottoDepthSub1 = n.subscribe("/giottobot/camera/depth_registered/points", 1000, giottoPointCloudCallback);
  //subscribe to /bellinibot/camera/depth_registered/points
  ros::Subscriber belliniDepthSub1 = n.subscribe("/bellinibot/camera/depth_registered/points", 1000, belliniPointCloudCallback);

  //raw image
  //subscribe to /giottobot/camera/depth_registered/image_rect_raw 
  ros::Subscriber giottoDepthSub2 = n.subscribe("/giottobot/camera_depth_registered/image_rect_raw", 1000, giottoRawImageCallback);
  //subscribe to /bellinibot/camera/depth_registered/image_rect_raw 
  ros::Subscriber belliniDepthSub2 = n.subscribe("/bellinibot/camera_depth_registered/image_rect_raw", 1000, belliniRawImageCallback);

  //image color
  //subscribe to /giottobot/camera/rgb/image_color
  ros::Subscriber giottoImageSub1 = n.subscribe("/giottobot/camera/rgb/image_color", 1000, giottoImageColorCallback);
  //subscribe to /bellinibot/camera/rgb/image_color
  ros::Subscriber belliniimageSub1 = n.subscribe("/bellinibot/camera/rgb/image_color", 1000, belliniImageColorCallback);

  //imu data
  //subscribe to /giottobot/mobile_base/sensors/imu_data
  ros::Subscriber giottoImageSub2 = n.subscribe("/giottobot/mobile_base/sensors/imu_data", 1000, giottoImuDataCallback);
  //subscribe to /bellinibot/mobile_base/sensors/imu_data
  ros::Subscriber belliniImageSub2 = n.subscribe("/bellinibot/mobile_base/sensors/imu_data", 1000, belliniImuDataCallback);

  ros::spin(); //constantly execute subscribing stuff

  //somehow combine/average all the camera data (not done yet)
  //...

  //somehow remap the resulting data to the topics rviz subscribes to (not done yet)
  //... 

  return 0;
}
