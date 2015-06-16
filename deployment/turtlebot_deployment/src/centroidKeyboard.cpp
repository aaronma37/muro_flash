

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <std_msgs/Float64.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include <turtlebot_deployment/PoseWithName.h>
#include <tf/tf.h>
#include <math.h>
#include <termios.h>
#include <time.h> 

//Declare Variables
double x2, x1, r;
double orientation;
double robVel_;
double OmegaC;
double OmegaD;
double cenx, ceny;
// Construct Node Class

int getch()
{
  static struct termios oldt, newt;
  tcgetattr( STDIN_FILENO, &oldt);           // save old settings
  newt = oldt;
  newt.c_lflag &= ~(ICANON);                 // disable buffering      
  tcsetattr( STDIN_FILENO, TCSANOW, &newt);  // apply new settings

  int c = getchar();  // read character (non-blocking)

  tcsetattr( STDIN_FILENO, TCSANOW, &oldt);  // restore old settings
  return c;
}

int main(int argc, char **argv)
{
ros::init(argc, argv, "centroidKeyboard");
cenx=300;
ceny=200;
r=75;

ros::NodeHandle ph_("~"), nh_;
ros::Publisher cen_pub_;
geometry_msgs::Twist cmd_vel_;
turtlebot_deployment::PoseWithName cenPose;
cenPose.pose.position.x=cenx;
cenPose.pose.position.y=ceny;
cen_pub_ = nh_.advertise<turtlebot_deployment::PoseWithName>("/centroidPos", 5, true);

double k=1.75;
double u1=robVel_;
double u2=robVel_/r;
OmegaC=2;
OmegaD=1;
std::cout<<"Exiting Main Sequence: \n";
while(1==1){

	int c = getch();   // call your non-blocking input function
  if (c == 'd'){
    cenPose.pose.position.x=cenPose.pose.position.x+10;}
  else if (c == 'w'){
    cenPose.pose.position.y=cenPose.pose.position.y+10;}
    else if (c == 's'){
    cenPose.pose.position.y=cenPose.pose.position.y-10;}
    else if (c == 'a'){
    cenPose.pose.position.x=cenPose.pose.position.x-10;}
    std::cout<<"Centroid Moved: \n";
    std::cout<<"X Centroid: "<<cenPose.pose.position.x<<"\n";
    std::cout<<"Y Centroid: "<<cenPose.pose.position.y<<"\n";
cen_pub_.publish(cenPose);
    usleep(100000);
}	
}

    

