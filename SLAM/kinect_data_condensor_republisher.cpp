#include "ros/ros.h"
#include "std_msgs/String.h"
#include <nav_msgs/Odometry.h>
#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/Imu.h>

//declare global variables to store individual message data from callback functions

//boticelli
nav_msgs::Odometry boticelliOdometryMsg;
sensor_msgs::LaserScan boticelliScanMsg;
sensor_msgs::PointCloud2 boticelliPointsMsg;
sensor_msgs::Image boticelliRawImageMsg;
sensor_msgs::Image boticelliImageColorMsg;

//titian
nav_msgs::Odometry titianOdometryMsg;
sensor_msgs::LaserScan titianScanMsg;
sensor_msgs::PointCloud2 titianPointsMsg;
sensor_msgs::Image titianRawImageMsg;
sensor_msgs::Image titianImageColorMsg;

//subscribing callback functions for specific robots

void boticelliOdomCallback(const nav_msgs::OdometryConstPtr& locator)
{
  boticelliOdometryMsg = *locator;
  ROS_INFO("boticelli Position: %f,%f", boticelliOdometryMsg.pose.pose.position.x,boticelliOdometryMsg.pose.pose.position.y);
  ROS_INFO("boticelli Heading: linear:%f,angular:%f", boticelliOdometryMsg.twist.twist.linear.x,boticelliOdometryMsg.twist.twist.angular.z);
}

void titianOdomCallback(const nav_msgs::OdometryConstPtr& locator)
{
  titianOdometryMsg = *locator;
  ROS_INFO("titian Position: %f,%f", titianOdometryMsg.pose.pose.position.x,titianOdometryMsg.pose.pose.position.y);
  ROS_INFO("titian Heading: linear:%f,angular:%f", titianOdometryMsg.twist.twist.linear.x,titianOdometryMsg.twist.twist.angular.z);
}

void boticelliScanCallback(const sensor_msgs::LaserScanConstPtr& scanner)
{
  boticelliScanMsg = *scanner;
  for (int i=0;i<boticelliScanMsg.ranges.size();i++)
  {
    ROS_INFO("boticelli Ranges[%d]=%f",i,boticelliScanMsg.ranges[i]);
  }
  for (int i=0;i<boticelliScanMsg.intensities.size();i++)
  {
    ROS_INFO("boticelli Intensities[%d]=%f",i,boticelliScanMsg.intensities[i]);
  }
}

void titianScanCallback(const sensor_msgs::LaserScanConstPtr& scanner)
{
  titianScanMsg = *scanner;
  for (int i=0;i<titianScanMsg.ranges.size();i++)
  {
    ROS_INFO("titian Ranges[%d]=%f",i,titianScanMsg.ranges[i]);
  }
  for (int i=0;i<titianScanMsg.intensities.size();i++)
  {
    ROS_INFO("titian Intensities[%d]=%f",i,titianScanMsg.intensities[i]);
  }
}

void boticelliPointCloudCallback(const sensor_msgs::PointCloud2ConstPtr& points)
{
  boticelliPointsMsg = *points;
  for (int p=0;p<boticelliPointsMsg.data.size();p++)
  {
    ROS_INFO("boticelli PointCloud2 Data[%d]=%d",p,boticelliPointsMsg.data[p]);
  }
}

void titianPointCloudCallback(const sensor_msgs::PointCloud2ConstPtr& points)
{
  titianPointsMsg = *points;
  for (int p=0;p<titianPointsMsg.data.size();p++)
  {
    ROS_INFO("titian PointCloud2 Data[%d]=%d",p,titianPointsMsg.data[p]);
  }
}

void boticelliRawImageCallback(const sensor_msgs::ImageConstPtr& image)
{
  boticelliRawImageMsg = *image;
  for (int i=0;i<boticelliRawImageMsg.data.size();i++)
  {
    ROS_INFO("boticelli Raw Image Data[%d]=%d",i,boticelliRawImageMsg.data[i]);
  }
}

void titianRawImageCallback(const sensor_msgs::ImageConstPtr& image)
{
  titianRawImageMsg = *image;
  for (int i=0;i<titianRawImageMsg.data.size();i++)
  {
    ROS_INFO("titian Raw Image Data[%d]=%d",i,titianRawImageMsg.data[i]);
  }
}

void boticelliImageColorCallback(const sensor_msgs::ImageConstPtr& image)
{
  boticelliImageColorMsg = *image;
  for (int i=0;i<boticelliImageColorMsg.data.size();i++)
  {
    ROS_INFO("boticelli Image Color Data[%d]=%d",i,boticelliImageColorMsg.data[i]);
  }
}

void titianImageColorCallback(const sensor_msgs::ImageConstPtr& image)
{
  titianImageColorMsg = *image;
  for (int i=0;i<titianImageColorMsg.data.size();i++)
  {
    ROS_INFO("titian Image Color Data[%d]=%d",i,titianImageColorMsg.data[i]);
  }
}


//this one seems to be really similar to odom, but for now I'm keeping it here anyway
/*
void boticelliImuDataCallback(const sensor_msgs::Imu& locator)
{
  ROS_INFO("boticelli Position: %f,%f", locator.orientation.x,locator.orientation.y);
  ROS_INFO("boticelli Velocity: linear:%f,angular:%f", locator.angular_velocity.x,locator.angular_velocity.z);
  ROS_INFO("boticelli Acceleration: linear:%f", locator.linear_acceleration.x);
}
void titianImuDataCallback(const sensor_msgs::Imu& locator)
{
  ROS_INFO("titian Position: %f,%f", locator.orientation.x,locator.orientation.y);
  ROS_INFO("titian Velocity: linear:%f,angular:%f", locator.angular_velocity.x,locator.angular_velocity.z);
  ROS_INFO("titian Acceleration: linear:%f", locator.linear_acceleration.x);
}
*/




int main(int argc, char **argv)
{

  ros::init(argc, argv, "subscriber");

  //object to interact with ROS
  ros::NodeHandle n;

  //declare pi for easy turns in radians
  float pi = 3.14159;

  ros::Rate loop_rate(50);

  //subscribe to the topics rviz subscribes to, then republish to the name general topics for rviz to see

  //odom
  //subscribe to /boticellibot/odom
  ros::Subscriber boticelliOdomSub = n.subscribe<nav_msgs::Odometry>("/boticellibot/odom", 1000, boticelliOdomCallback);
  //subscribe to /titianbot/odom
  ros::Subscriber titianOdomSub = n.subscribe<nav_msgs::Odometry>("/titianbot/odom", 1000, titianOdomCallback);
  //odom publisher
  ros::Publisher OdomPub = n.advertise<nav_msgs::Odometry>("/odom",1000);

  //scan
  //subscribe to /boticellibot/scan
  ros::Subscriber boticelliScanSub = n.subscribe<sensor_msgs::LaserScan>("/boticellibot/scan", 1000, boticelliScanCallback);
  //subscribe to /titianbot/scan
  ros::Subscriber titianScanSub = n.subscribe<sensor_msgs::LaserScan>("/titianbot/scan", 1000, titianScanCallback);
  //scan publisher
  ros::Publisher ScanPub = n.advertise<sensor_msgs::LaserScan>("/scan",1000);

  //point cloud
  //subscribe to /boticellibot/camera/depth_registered/points
  ros::Subscriber boticelliDepthSub1 = n.subscribe<sensor_msgs::PointCloud2>("/boticellibot/camera/depth_registered/points", 1000, boticelliPointCloudCallback);
  //subscribe to /titianbot/camera/depth_registered/points
  ros::Subscriber titianDepthSub1 = n.subscribe<sensor_msgs::PointCloud2>("/titianbot/camera/depth_registered/points", 1000, titianPointCloudCallback);
  //point cloud publisher
  ros::Publisher DepthPub1 = n.advertise<sensor_msgs::PointCloud2>("/camera/depth_registered/points",1000);

  //raw image
  //subscribe to /boticellibot/camera/depth_registered/image_rect_raw 
  ros::Subscriber boticelliDepthSub2 = n.subscribe<sensor_msgs::Image>("/boticellibot/camera_depth_registered/image_rect_raw", 1000, boticelliRawImageCallback);
  //subscribe to /titianbot/camera/depth_registered/image_rect_raw 
  ros::Subscriber titianDepthSub2 = n.subscribe<sensor_msgs::Image>("/titianbot/camera_depth_registered/image_rect_raw", 1000, titianRawImageCallback);
  //raw image publisher
  ros::Publisher DepthPub2 = n.advertise<sensor_msgs::Image>("/camera/depth_registered/image_rect_raw",1000);

  //image color
  //subscribe to /boticellibot/camera/rgb/image_color
  ros::Subscriber boticelliImageSub1 = n.subscribe<sensor_msgs::Image>("/boticellibot/camera/rgb/image_color", 1000, boticelliImageColorCallback);
  //subscribe to /titianbot/camera/rgb/image_color
  ros::Subscriber titianimageSub1 = n.subscribe<sensor_msgs::Image>("/titianbot/camera/rgb/image_color", 1000, titianImageColorCallback);
  //image color publisher
  ros::Publisher ImagePub1 = n.advertise<sensor_msgs::Image>("/camera/rgb/image_color",1000);


//this is the weird one I'm keeping commented out for now
/*
  //imu data
  //subscribe to /boticellibot/mobile_base/sensors/imu_data
  ros::Subscriber boticelliImageSub2 = n.subscribe("/boticellibot/mobile_base/sensors/imu_data", 1000, boticelliImuDataCallback);
  //subscribe to /titianbot/mobile_base/sensors/imu_data
  ros::Subscriber titianImageSub2 = n.subscribe("/titianbot/mobile_base/sensors/imu_data", 1000, titianImuDataCallback);
*/

  //combine/average all the camera data from multiple robots (not done yet)
  //somehow use known relative positions to do transforms on individual data clouds to condense all corresponding messages into one master message to be sent to rviz
  //average redundant point clouds, overlay non redundant ones
  //...

  //publish the processed/condensed data to rviz
  while (ros::ok()) 
  {
    //in the end each publishing object will only be publishing one condensed message per loop, but right now I'm having them publish both robot's individual messages as a test

    //odom
    OdomPub.publish(boticelliOdometryMsg);
    OdomPub.publish(titianOdometryMsg);
    ROS_INFO("Publishing odometry");
    //scan
    ScanPub.publish(boticelliScanMsg);
    ScanPub.publish(titianScanMsg);
    ROS_INFO("Publishing laser scan");
    //point cloud
    DepthPub1.publish(boticelliPointsMsg);
    DepthPub1.publish(titianPointsMsg);
    ROS_INFO("Publishing point cloud");
    //raw image
    //scan
    DepthPub2.publish(boticelliRawImageMsg);
    DepthPub2.publish(titianRawImageMsg);
    ROS_INFO("Publishing raw image");
    //image color
    ImagePub1.publish(boticelliImageColorMsg);
    ImagePub1.publish(titianImageColorMsg);
    ROS_INFO("Publishing image color");

    ros::spinOnce();

    loop_rate.sleep();
  }

  return 0;
}
