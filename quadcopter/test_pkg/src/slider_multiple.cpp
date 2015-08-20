/*
The main function of this node is to process position (pose) 
data, compute the error between desired and estimated pose, 
and then use a tuned PID controller to determine the correct 
velocity needed to minimize that error and approximate the 
goal pose.
This node subcribes to "/poseEstimation" topic where it will 
listen for any pose data coming in from the ekf filter node.
These pose coordinates are the best estimates for where the
Quadcopter is currently located.
This node subscribes to "/goalPose" topic where 
it will listen for any pose data coming from the android node. 
These pose coordinates are the desired position for the 
Quadcopter.
FIXME: This subscriber is yet to be implemented.
This node publishes to "/cmd_vel" topic where it will update 
its velocity, to be used by the ekf node for pose estimation.
*/

// We need to record using the command below so we can get a good system id
//
// we have add in controls for when yaw isnt 0. for example, we are still sending a pure x signal even when the yaw ~=0, meaning 
// that sin(yaw)*linear.x is in the y component.  we haven't accounted for that yet here.
//
// TOOLS AGAINST INTEGRAL WINDUP
// GAIN SCHEDULING 
// write a bash script to automatically launch all files

/*
This is the command to record to a txt file
rostopic echo -p /topic_name > data.txt
*/

#include <iostream>
#include <stdio.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <tf2_msgs/TFMessage.h>
#include "geometry_msgs/TransformStamped.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include <geometry_msgs/PoseStamped.h>
#include <tf/tf.h>
#include <fstream>
#include <math.h>
#include <time.h> 
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <stdlib.h>
#include "std_msgs/String.h"
#include <std_msgs/Float64.h>
#include <sstream>

using namespace std;
const int num=1000;
// Position and movement messages
geometry_msgs::PoseStamped poseEstimation[num]; // Where the Quadcopter thinks it is
geometry_msgs::Twist velEstimation[num];
geometry_msgs::PoseStamped poseGoal; // Where the Quadcopter should be
geometry_msgs::PoseStamped poseError[num]; // Difference between desired pose and current pose
tf2_msgs::TFMessage velocity; // Velocity command needed to rectify the error
geometry_msgs::Vector3 pidGain; // Store pid gain values
geometry_msgs::Vector3 poseSysId[num]; // Store best pose estimations to use for the system id
geometry_msgs::Vector3 velPoseEstX; // Used for modeling purposes
geometry_msgs::Twist pathVel; // velocity along path


// Keep track of Quadcopter state
double T = 50; // ROS loop rate

// Constants

const double PI = 3.141592653589793238463;
const double DEFAULT_KP = 0.5;
const double DEFAULT_KI = 0.01;
const double DEFAULT_KD = 0;
const double DEFAULT_KPZ = 4;
const double DEFAULT_KIZ = 0;
const double DEFAULT_KDZ = .1;
const double WINDUP_BOUND = 1.0;

// Initialize pid gains
double kp = DEFAULT_KP; // Proportional gain
double ki = DEFAULT_KI; // Integral gain
double kd = DEFAULT_KD; // Differential gain
double kpZ = DEFAULT_KPZ;
double kiZ = DEFAULT_KIZ;
double kdZ = DEFAULT_KDZ;
double sX[num];
double sY[num];
double sZ[num];
double sXprev = 0;
double sYprev = 0;
double sZprev = 0;
//double pastX = 0;
//double pastY = 0;
//double pastZ = 0;
double sliderSlope = 0.8;
double sliderGain = 0.8;
double sliderRange = 0.25;
double sliderAmp = 0.35;

// Integral windup
double windupCap;

// Kepp track of yaw to determine angular component of velocity 
double poseEstYaw[num] ; // twist or oscillation about a vertical axis
bool active[num];
double poseGoalYaw = PI;
double poseErrYaw[num];
double poseErrYawPrev[num]; // for PID derivative term
double pastYawErr[num];

// Moving Average terms
const int numSamples = 10;
float maArrayX [numSamples] = {0}; // Array of size numSamples for moving average calculations.
float maArrayY [numSamples] = {0};
float maArrayZ [numSamples] = {0};
float maArrayYaw [numSamples] = {0};
float *maResults = new float[4];
float activeAngle=0;
int maIndex = 1;
int k=0;

// PID controller terms
geometry_msgs::PoseStamped pastError[num] ; // This is the integral term
geometry_msgs::PoseStamped poseErrorPrev[num] ; // This is used to determine the derviative term
                                          // It stores the previous poseError

// Updates current position estimate sent by the ekf
void poseEstCallback(const tf2_msgs::TFMessage::ConstPtr& posePtr)
{
	for (int i =0;i<1000;i++){
		poseEstimation[i].pose.position.x = posePtr -> transforms[i].transform.translation.x;
		poseEstimation[i].pose.position.y = posePtr -> transforms[i].transform.translation.y;
		poseEstimation[i].pose.position.z = posePtr -> transforms[i].transform.translation.z;

		poseEstimation[i].pose.orientation.x = posePtr -> transforms[i].transform.rotation.x;
		poseEstimation[i].pose.orientation.y = posePtr -> transforms[i].transform.rotation.y;
		poseEstimation[i].pose.orientation.z = posePtr -> transforms[i].transform.rotation.z;
		poseEstimation[i].pose.orientation.w = posePtr -> transforms[i].transform.rotation.w;

    		poseSysId[i].x = poseEstimation[i].pose.position.x; 
    		poseSysId[i].y = poseEstimation[i].pose.position.y;
    		poseSysId[i].z = poseEstimation[i].pose.position.z;
    		poseEstYaw[i] = tf::getYaw(poseEstimation[i].pose.orientation) + PI;
    		if (poseEstimation[i].pose.position.y!=0 || poseEstimation[i].pose.position.x!=0){
			active[i]=true;
			std::cout<<"\n Measured: \n"<<poseEstimation[i]<<"\n";
		}
	}    
}

void velocityEstCallback(const tf2_msgs::TFMessage::ConstPtr& twistPtr)
{
	for (int i=0;i<1000;i++){
		velEstimation[i].linear.x = twistPtr->transforms[i].transform.translation.x;
		velEstimation[i].linear.y = twistPtr->transforms[i].transform.translation.y;
		velEstimation[i].linear.z = twistPtr->transforms[i].transform.translation.z;
	}
}




// Updates goal position sent by android
void poseGoalCallback(const geometry_msgs::PoseStamped::ConstPtr& posePtr)
{
    poseGoal.pose = posePtr -> pose;
    poseGoalYaw = tf::getYaw(poseGoal.pose.orientation) + PI;
}



// Updates slider gain values
void pidGainCallback(const geometry_msgs::Vector3::ConstPtr& gainPtr)
{
    sliderSlope = (double) gainPtr -> x;
    sliderGain = (double) gainPtr -> y;
    kd = (double) gainPtr -> z;
}

// Updates pid gain values for z dimension
void pidGainZCallback(const geometry_msgs::Vector3::ConstPtr& gainPtr)
{
    kpZ = (double) gainPtr -> x;
    kiZ = (double) gainPtr -> y;
    kdZ = (double) gainPtr -> z;
}

// Updates pid gain values for z dimension
void pathVelCallback(const geometry_msgs::Twist::ConstPtr& pathVelPtr)
{
    pathVel = *pathVelPtr;
}

// Calculates updated error to be used by PID
void calculateError(int i)
{
    poseErrorPrev[i].pose = poseError[i].pose; // Store old pose error for PID equation
    poseErrYawPrev[i] = poseErrYaw[i]; // Store old pose yaw for PID equation
    poseError[i].pose.position.x = poseGoal.pose.position.x - poseEstimation[i].pose.position.x;
    poseError[i].pose.position.y = poseGoal.pose.position.y - poseEstimation[i].pose.position.y;
    poseError[i].pose.position.z = poseGoal.pose.position.z - poseEstimation[i].pose.position.z;
    poseErrYaw[i] = poseGoalYaw - poseEstYaw[i];
    poseError[i].pose.orientation = tf::createQuaternionMsgFromYaw(poseErrYaw[i]); 
}

void calcMoveAvg(float newSampleX, float newSampleY, float newSampleZ, float newSampleYaw)
{
  static bool divideSampling = false;
  
  if ((maIndex - 1) == 10)
  {
    maIndex = 1;
    divideSampling = true; 
  }
  
  maArrayX[maIndex - 1] = newSampleX;
  maArrayY[maIndex - 1] = newSampleY;
  maArrayZ[maIndex - 1] = newSampleZ;
  maArrayYaw[maIndex - 1] = newSampleYaw;
  
  maResults[0] = 0;
  maResults[1] = 0;
  maResults[2] = 0;
  maResults[3] = 0;
  
  for (int i = 0; i < numSamples ; i++)
  {
    maResults[0] += maArrayX[i];
    maResults[1] += maArrayY[i];
    maResults[2] += maArrayZ[i];
    maResults[3] += maArrayYaw[i];
  }
   
  if (divideSampling != true)
  {
    maResults[0] = maResults[0]/maIndex;
    maResults[1] = maResults[1]/maIndex;
    maResults[2] = maResults[2]/maIndex;
    maResults[3] = maResults[3]/maIndex;
  }
  else
  {
    maResults[0] = (maResults[0]/numSamples);
    maResults[1] = (maResults[1]/numSamples);
    maResults[2] = (maResults[2]/numSamples);
    maResults[3] = (maResults[3]/numSamples);
  }

  maIndex++;
}

// This is the PID method
void PID(int i)
{
    double kpYaw = .5;
    double kiYaw = 0;
    double kdYaw = 0;
    
    pastError[i].pose.position.x += (1/T)*poseError[i].pose.position.x;
    pastError[i].pose.position.y += (1/T)*poseError[i].pose.position.y;
    pastError[i].pose.position.z += (1/T)*poseError[i].pose.position.z;
    pastYawErr[i] += (1/T)*poseErrYaw[i];
    
    // Check for integral windup
    if( sqrt( pow(poseError[i].pose.position.x, 2) + pow(poseError[i].pose.position.y, 2) ) > WINDUP_BOUND )
    {
      ki = 0;
    }
    
    if(ki == 0)
    {
      windupCap = 0;
    }
    else windupCap = 0.05/ki;
    
    if(pastError[i].pose.position.x > windupCap)
    {
      pastError[i].pose.position.x = windupCap;
    }
    else if(pastError[i].pose.position.x < -windupCap)
    {
      pastError[i].pose.position.x = -windupCap;
    }
    if(pastError[i].pose.position.y > windupCap)
    {
      pastError[i].pose.position.y = windupCap;
    }
    else if(pastError[i].pose.position.y < -windupCap)
    {
      pastError[i].pose.position.y = -windupCap;
    }
    
    // Store previous s value to determine derivative term
    sXprev = sX[i];
    sYprev = sY[i];
    sZprev = sZ[i];
    
    // Linear Sliding Mode
    sX[i] = -sliderSlope*poseError[i].pose.position.x + velEstimation[i].linear.x;
    sY[i] = -sliderSlope*poseError[i].pose.position.y + velEstimation[i].linear.y;
    sZ[i] = -sliderSlope*poseError[i].pose.position.z + velEstimation[i].linear.z;
    
    
    calcMoveAvg(sX[i] - sXprev, sY[i] - sYprev, sZ[i] - sZprev, poseErrYaw[i] - poseErrYawPrev[i]);
      
    velocity.transforms[i].transform.translation.x = sX[i]*(-sliderGain) + (kd*T*(maResults[0])) + pathVel.linear.x;
    if (velocity.transforms[i].transform.translation.x > 1){
      velocity.transforms[i].transform.translation.x = 1;
    }
    else if (velocity.transforms[i].transform.translation.x < -1){
      velocity.transforms[i].transform.translation.x = -1;
    }
    
    velocity.transforms[i].transform.translation.y = sY[i]*sliderGain + (kd*T*(maResults[1])) + pathVel.linear.y;
    if (velocity.transforms[i].transform.translation.y > 1){
      velocity.transforms[i].transform.translation.y = 1;
    }
    else if (velocity.transforms[i].transform.translation.y < -1){
      velocity.transforms[i].transform.translation.y = -1;
    }
    
    velocity.transforms[i].transform.translation.z = sZ[i]*-(sliderGain);
    if (velocity.transforms[i].transform.translation.z > 1){
      velocity.transforms[i].transform.translation.z = 1;
    }
    else if (velocity.transforms[i].transform.translation.z < -1){
      velocity.transforms[i].transform.translation.z = -1;
    }
    
    velocity.transforms[i].transform.rotation.z = (kpYaw*poseErrYaw[i]) + (kiYaw*pastYawErr[i]) + (kdYaw*T*(maResults[3]));
    
      if (velocity.transforms[i].transform.translation.x!=0 && velocity.transforms[i].transform.translation.y!=0){
		    double dummyA=1;
		    double dummyO=tan(poseEstYaw[i]+PI);
		    double norm1=sqrt((velocity.transforms[i].transform.translation.x*velocity.transforms[i].transform.translation.x+velocity.transforms[i].transform.translation.y*velocity.transforms[i].transform.translation.y));


		    double dot = dummyA*velocity.transforms[i].transform.translation.x + dummyO*velocity.transforms[i].transform.translation.y;   
		    double det = dummyA*velocity.transforms[i].transform.translation.y - dummyO*velocity.transforms[i].transform.translation.x;   
		    double activeAngle = atan2(det, dot);

		    velocity.transforms[i].transform.translation.x =  norm1*cos(activeAngle);
		    velocity.transforms[i].transform.translation.y =  norm1*sin(activeAngle);
   	 }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "Quadcopter Hover: version 1, EKF pose estimations only"); //Ros Initialize
    ros::start();
    ros::Rate loop_rate(T); //Set Ros frequency to 50/s (fast)
    srand (time(NULL));
    ros::NodeHandle n;
    ros::Subscriber poseEstSub;
    ros::Subscriber poseGoalSub;
    ros::Subscriber velEstSub;
    ros::Subscriber pidGainSub;
    ros::Subscriber pidGainZSub;
    ros::Subscriber pathVelSub;
    ros::Publisher velPub;
    ros::Publisher poseSysIdPub;
    ros::Publisher velPoseEstXPub;

    poseEstSub = n.subscribe<tf2_msgs::TFMessage>("/poseEstimationC", 1, poseEstCallback);
    velEstSub = n.subscribe<tf2_msgs::TFMessage>("/velocityEstimation", 1, velocityEstCallback);
    poseGoalSub = n.subscribe<geometry_msgs::PoseStamped>("/goal_pose", 1, poseGoalCallback);
    pidGainSub = n.subscribe<geometry_msgs::Vector3>("/pid_gain", 1, pidGainCallback);
    pidGainZSub = n.subscribe<geometry_msgs::Vector3>("/pid_gainZ", 1, pidGainZCallback);
    pathVelSub = n.subscribe<geometry_msgs::Twist>("/path_vel", 1, pathVelCallback);
    velPub = n.advertise<tf2_msgs::TFMessage>("/cmd_vel", 1000, true);
    poseSysIdPub = n.advertise<geometry_msgs::Vector3>("/sys_id", 1000, true);
    velPoseEstXPub = n.advertise<geometry_msgs::Vector3>("/vel_poseEstX", 1000, true);

    // Initialize msgs
    poseGoal.pose.position.x = 0;
    poseGoal.pose.position.y = 0;
    poseGoal.pose.position.z = 0;
    

    
    pathVel.linear.x = 0;
    pathVel.linear.y = 0;
    pathVel.linear.z = 0;
    pathVel.angular.x = 0;
    pathVel.angular.y = 0;
    pathVel.angular.z = 0;

    velPoseEstX.z = 0;
velocity.transforms.resize(1000);
    velocity.transforms[0].header.frame_id="Gypsy Danger";
    velocity.transforms[1].header.frame_id="Typhoon";
cout<<"CHECK \n";
for (int i=1;i<5;i++){
std::ostringstream ss;
ss << i;
string s = "dummy ";
s+=ss.str();
velocity.transforms[i+100].header.frame_id=s;
}
for (int i=0;i<num;i++){

    poseError[i].pose.position.x = 0;
    poseError[i].pose.position.y = 0;
    poseError[i].pose.position.z = 0;

    pastError[i].pose.position.x = 0;
    pastError[i].pose.position.y = 0;
    pastError[i].pose.position.z = 0;
    velocity.transforms[i].transform.rotation.x = 1;
    velocity.transforms[i].transform.rotation.y = 0;
	
    active[i]=false;
    }

    while (ros::ok()) 
    {
        ros::spinOnce();
	for (int i=0;i<num;i++){
		if (active[i]==true){
		calculateError(i);        
		PID(i);
		poseSysIdPub.publish(poseSysId[i]);
		}
	}
	velPub.publish(velocity);
        loop_rate.sleep();
    }
}
//END
