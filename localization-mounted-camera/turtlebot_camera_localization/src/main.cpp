#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "std_msgs/Int32.h"
#include "std_msgs/Float32.h"
#include "nav_msgs/Odometry.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "geometry_msgs/Quaternion.h"
#include "geometry_msgs/TransformStamped.h"
#include "geometry_msgs/PoseArray.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <tf/transform_broadcaster.h>
#include <stdio.h>

//INTIALIZE VARIABLES

//Initialize publishers
image_transport::Publisher pub;
ros::Publisher x_pub;
ros::Publisher y_pub;
ros::Publisher theta_pub;
ros::Publisher odom_pub;
ros::Publisher camPose_pub;
ros::Publisher particleCloudPub;
tf::TransformBroadcaster *odom_broadcaster = NULL;
 
//Store all constants for image encodings in the enc namespace to be used later.
namespace enc = sensor_msgs::image_encodings;

//Declare some strings for window names
static const char WINDOW[] = "HSV";
static const char WINDOW2[] = "THRESHOLDED IMAGE (MAIN)";
static const char WINDOW3[] = "LOCATED IMAGE";
static const char WINDOW4[] = "THRESHOLDED IMAGE (POINTER)";
static const char trackbarWindowNameM[] = "Main Pointer Trackbars";
static const char trackbarWindowNameP[] = "Pointer Pointer Trackbars";

//Initalize a name for this turtlebot
std::string turtle_name;
char* turtle_name_char;
std::string pink_bot_name ("/boticellibot");
std::string green_bot_name ("/donatellobot");
std::string orange_bot_name ("/raphaelbot");

//Initalize modes for param functionality
int calibration_mode;
int show_hsv;

//makes an myLocs array to save the "main" data to
double myLocs[3];
//make an myLocs2 array to save the "pointer" data to
double myLocs2[2];

//initalize the messages that will be published
std_msgs::Int32 xLocation;
std_msgs::Int32 yLocation;
std_msgs::Float32 heading;
nav_msgs::Odometry odom;
geometry_msgs::PoseWithCovarianceStamped camPose;
geometry_msgs::Quaternion odom_quat;
geometry_msgs::TransformStamped odom_trans;
geometry_msgs::PoseArray particleCloud;

//HSV Filter Values, set one (main target)
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;

//HSV Filter Values, set two (pointer)
int H_MIN2 = 0;
int H_MAX2 = 256;
int S_MIN2 = 0;
int S_MAX2 = 256;
int V_MIN2 = 0;
int V_MAX2 = 256;

//size values
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20*20;
//const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;
const int MAX_OBJECT_AREA = 200*200;
//maximum number of objects before declaring noise
const int MAX_NUM_OBJECTS = 100;
//scaling factor (meters/pixel) Change the numerator to rescale the "map"
const double SCALING_FACTOR_X = 3.35/640;
const double SCALING_FACTOR_Y = 2.44/480;
ros::Time current_time;
const double PI = 3.14159265;

//FUNCTIONS

std::string intToString(int number){
    std::stringstream ss;
    ss << number;
    return ss.str();
}

void on_trackbar( int, void* )
{//This function gets called whenever a
// trackbar position is changed
}

void createTrackbarsMain(){
//create window for trackbars
	cv::namedWindow(trackbarWindowNameM,0);
//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf( TrackbarName, "H_MIN", H_MIN);
	sprintf( TrackbarName, "H_MAX", H_MAX);
	sprintf( TrackbarName, "S_MIN", S_MIN);
	sprintf( TrackbarName, "S_MAX", S_MAX);
	sprintf( TrackbarName, "V_MIN", V_MIN);
	sprintf( TrackbarName, "V_MAX", V_MAX);

//create trackbars and insert them into window
//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
//the max value the trackbar can move (eg. H_HIGH),
//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
// ----> ----> ---->
	cv::createTrackbar( "H_MIN", trackbarWindowNameM, &H_MIN, H_MAX, on_trackbar );
	cv::createTrackbar( "H_MAX", trackbarWindowNameM, &H_MAX, H_MAX, on_trackbar );
	cv::createTrackbar( "S_MIN", trackbarWindowNameM, &S_MIN, S_MAX, on_trackbar );
	cv::createTrackbar( "S_MAX", trackbarWindowNameM, &S_MAX, S_MAX, on_trackbar );
	cv::createTrackbar( "V_MIN", trackbarWindowNameM, &V_MIN, V_MAX, on_trackbar );
	cv::createTrackbar( "V_MAX", trackbarWindowNameM, &V_MAX, V_MAX, on_trackbar );
}

void createTrackbarsPointer(){
//create window for trackbars
	cv::namedWindow(trackbarWindowNameP,0);
//create memory to store trackbar name on window
	char TrackbarName2[50];
	sprintf( TrackbarName2, "H_MIN", H_MIN2);
	sprintf( TrackbarName2, "H_MAX", H_MAX2);
	sprintf( TrackbarName2, "S_MIN", S_MIN2);
	sprintf( TrackbarName2, "S_MAX", S_MAX2);
	sprintf( TrackbarName2, "V_MIN", V_MIN2);
	sprintf( TrackbarName2, "V_MAX", V_MAX2);

//create trackbars and insert them into window
//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
//the max value the trackbar can move (eg. H_HIGH),
//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
// ----> ----> ---->
	cv::createTrackbar( "H_MIN", trackbarWindowNameP, &H_MIN2, H_MAX2, on_trackbar );
	cv::createTrackbar( "H_MAX", trackbarWindowNameP, &H_MAX2, H_MAX2, on_trackbar );
	cv::createTrackbar( "S_MIN", trackbarWindowNameP, &S_MIN2, S_MAX2, on_trackbar );
	cv::createTrackbar( "S_MAX", trackbarWindowNameP, &S_MAX2, S_MAX2, on_trackbar );
	cv::createTrackbar( "V_MIN", trackbarWindowNameP, &V_MIN2, V_MAX2, on_trackbar );
	cv::createTrackbar( "V_MAX", trackbarWindowNameP, &V_MAX2, V_MAX2, on_trackbar );
}

void drawObject(int x,int y, cv::Mat &frame, int display_tag){
	cv::circle(frame,cv::Point(x,y),10,cv::Scalar(0,0,255));
	cv::putText(frame,intToString(x)+ " , " + intToString(y),cv::Point(x,y+20),1,1, cv::Scalar(0,255,0));
	if(display_tag==1){
	cv::putText(frame, turtle_name, cv::Point(x+100,y),1,1, cv::Scalar(0,255,0));}
}
 
//This function dilates and errodes the image
void morphOps(cv::Mat &thresh){
    //create structuring element that will be used to "dilate" and "erode" image.
    //the element chosen here is a 3px by 3px rectangle
    cv::Mat erodeElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
    //dilate with larger element so make sure object is nicely visible
    cv::Mat dilateElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(8,8));
    cv::erode(thresh,thresh,erodeElement);
    cv::erode(thresh,thresh,erodeElement);
    cv::dilate(thresh,thresh,dilateElement);
    cv::dilate(thresh,thresh,dilateElement);  
}
 
void trackFilteredObject(double myLocs[], cv::Mat threshold, cv::Mat HSV, cv::Mat &cameraFeed, int mode)
{
    //Note that mode 1 indicates "main" position is desired; mode 2 indicates "pointer" position is desired
    cv::Mat temp;
    threshold.copyTo(temp);
    //these two vectors needed for output of findContours
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    //find contours of filtered image using openCV findContours function
    findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE);
    //use moments method to find our filtered object
    double refArea = 0;
    bool objectFound = false;
    if (hierarchy.size() > 0)
    {
        int numObjects = hierarchy.size();
        //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
        if(numObjects<MAX_NUM_OBJECTS)
            {
                for (int index = 0; index >= 0; index = hierarchy[index][0]) 
                {
                    //int index = 0;    
                    cv::Moments moment = moments((cv::Mat)contours[index]);
                    double area = moment.m00;
                    if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea)
                        {
                            if (mode==1)
                            {
                                myLocs[0] = (moment.m10/area);
                		xLocation.data = (int)(moment.m10/area);
                                myLocs[1] = (moment.m01/area);
                		yLocation.data = (int)(moment.m01/area);
                                objectFound = true;
                                refArea = area;
                	    }
                            else
                            {
                        	myLocs2[0] = (moment.m10/area);
        		        myLocs2[1] = (moment.m01/area);
        		        objectFound = true;
                                refArea = area;
    			     }
                        } //else objectFound = false;
                }
            }

        if(objectFound ==true)
            {
 		//draw object location on screen, if you're looking for the main pointer
                if (mode==1)
                {
                    drawObject(xLocation.data,yLocation.data,cameraFeed,1);
                }
                else
                {
                    //calculate the heading
                    myLocs[2] = atan2(-(myLocs2[1]-myLocs[1]),(myLocs2[0]-myLocs[0])) + PI/2;
		    if(myLocs[2] >= PI)
		    {
			myLocs[2] = myLocs[2] - 2*PI;
		    }
                    //myLocs[2] = atan2((myLocs2[1]-myLocs[1]),(myLocs2[0]-myLocs[0]));
                    //probably want to change this to a double or something
                    heading.data = myLocs[2];
                    drawObject(myLocs2[0],myLocs2[1],cameraFeed,0);
                }
            }
        } else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",cv::Point(0,50),1,2,cv::Scalar(0,0,255),2);
}


//This function is called everytime a new image is published
void imageCallback(const sensor_msgs::ImageConstPtr& original_image)
{
    current_time = ros::Time::now();
    
    //Convert from the ROS image message to a CvImage suitable for working with OpenCV for processing
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
        //Always copy, returning a mutable CvImage
        //OpenCV expects color images to use BGR channel order.
        cv_ptr = cv_bridge::toCvCopy(original_image, enc::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
        //if there is an error during conversion, display it
        ROS_ERROR("tutorialROSOpenCV::main.cpp::cv_bridge exception: %s", e.what());
        return;
    }
 
    //translate to HSV plane
    cv::Mat HSV;
    cv::Mat HSV2;
    cv::Mat threshold;
    cv::Mat threshold2;
    cvtColor(cv_ptr->image, HSV, cv::COLOR_BGR2HSV);
    cv::inRange(HSV, cv::Scalar(H_MIN, S_MIN, V_MIN), cv::Scalar(H_MAX, S_MAX, V_MAX), threshold);
    cv::inRange(HSV, cv::Scalar(H_MIN2, S_MIN2, V_MIN2), cv::Scalar(H_MAX2, S_MAX2, V_MAX2), threshold2);
    morphOps(threshold);
    morphOps(threshold2);
     
    if(show_hsv==1){
	//DISPLAY THE HSV IMAGE
    	cv::imshow(WINDOW, HSV);
    }
    

    if(calibration_mode==1){
	//DISPLAY THE THRESHOLDED IMAGE (main)
    	cv::imshow(WINDOW2, threshold);
    	//DISPLAY THE THRESHOLDED IMAGE (pointer)
    	cv::imshow(WINDOW4, threshold2);
	}
  
    //TRACK THE FILTERED OBJECT!!! (EEP!)
    trackFilteredObject(myLocs, threshold, HSV, cv_ptr->image, 1);
    trackFilteredObject(myLocs, threshold2, HSV2, cv_ptr->image, 2);

    //DISPLAY THE TRACKED OBJECT(???)
    cv::imshow(WINDOW3, cv_ptr->image);

    //Add some delay in miliseconds. 
    cv::waitKey(3);
    
    //Convert the CvImage to a ROS image message and publish it on the "camera/image_processed" topic.
        //pub.publish(cv_ptr->toImageMsg());

        //Publish all the relevant information
    	//x_pub.publish(xLocation);
    	//y_pub.publish(yLocation);
    	//theta_pub.publish(heading);

//Start making the odom message
	odom_quat = tf::createQuaternionMsgFromYaw((double)myLocs[2]);	
	ros::Time current_time;
  	current_time = ros::Time::now();	
	odom.header.stamp = current_time;
	camPose.header.stamp = current_time;
	//I changed this to zero of the odom frame
	odom.pose.pose.position.x = 0.0;
	odom.pose.pose.position.y = 0.0;
	odom.pose.pose.position.z = 0.0;
	odom.pose.covariance[0]  = .1237;
	odom.pose.covariance[7]  = .1237;
	odom.pose.covariance[14] = 1.24069;
	odom.pose.covariance[21] = .1237;
	odom.pose.covariance[28] = .124085;
	odom.pose.covariance[35] = .1237;

	camPose.pose.pose.position.x = xLocation.data*SCALING_FACTOR_X;
	camPose.pose.pose.position.y = (480-yLocation.data)*SCALING_FACTOR_Y;
	camPose.pose.pose.position.z = 0.0;
	camPose.pose.pose.orientation = odom_quat;
	odom.header.frame_id = turtle_name+"/odom";
	camPose.header.frame_id = turtle_name+"/pose"; 

	//from here	
	odom_pub.publish(odom);
	camPose_pub.publish(camPose);
	
	//for tf
	odom_trans.header.stamp = current_time;
	odom_trans.header.frame_id = "/map";
	odom_trans.child_frame_id = turtle_name+"/odom";
	odom_trans.transform.translation.x = (xLocation.data)*SCALING_FACTOR_X;
	odom_trans.transform.translation.y = (480-yLocation.data)*SCALING_FACTOR_Y;
	odom_trans.transform.translation.z = 0.0;
	odom_trans.transform.rotation = odom_quat;
	//send transform
	odom_broadcaster->sendTransform(odom_trans);

	particleCloud.header = camPose.header;
	particleCloud.poses[0] = camPose.pose.pose;
	particleCloudPub.publish(particleCloud); 

}
 
int main(int argc, char **argv)
{
	ros::init(argc, argv, "localization");

	if (argc != 2){ROS_ERROR("need turtle name as argument"); return -1;};
    	
	turtle_name = argv[1];
	
	//this stuff is new
	turtle_name_char = argv[1]; 

	if (turtle_name.compare(pink_bot_name)==0){
		ROS_INFO("Calibrating for PINK");
		H_MIN = 136;
		H_MAX = 256;
		S_MIN = 88;
		S_MAX = 256;
		V_MIN = 167;
		V_MAX = 256;
		H_MIN2 = 116;
		H_MAX2 = 256;
		S_MIN2 = 141;
		S_MAX2 = 256;
		V_MIN2 = 116;
		V_MAX2 = 256;
	} 
	else if (turtle_name.compare(orange_bot_name)==0){
		ROS_INFO("Calibrating for ORANGE");
		H_MIN = 5;
		H_MAX = 39;
		S_MIN = 89;
		S_MAX = 175;
		V_MIN = 203;
		V_MAX = 245;
		H_MIN2 = 5;
		H_MAX2 = 39;
		S_MIN2 = 146;
		S_MAX2 = 205;
		V_MIN2 = 210;
		V_MAX2 = 242;
	} 
	else if (turtle_name.compare(green_bot_name)==0){
		ROS_INFO("Calibrating for GREEN");
		H_MIN = 45;
		H_MAX = 103;
		S_MIN = 86;
		S_MAX = 256;
		V_MIN = 88;
		V_MAX = 216;
		H_MIN2 = 44;
		H_MAX2 = 96;
		S_MIN2 = 106;
		S_MAX2 = 256;
		V_MIN2 = 76;
		V_MAX2 = 154;
	} 
	
	odom_broadcaster = new(tf::TransformBroadcaster);
	
	ros::NodeHandle nh;
	
	nh.param("calibration_mode", calibration_mode, 0);
	nh.param("show_hsv", show_hsv, 0);

	if(calibration_mode==1){
		createTrackbarsMain();
		createTrackbarsPointer();
	}

    	image_transport::ImageTransport it(nh);
   	 cv::namedWindow(WINDOW, CV_WINDOW_AUTOSIZE);
    	image_transport::Subscriber sub = it.subscribe("camera/image_raw", 1, imageCallback);
    	cv::destroyWindow(WINDOW);
     
	//x_pub = nh.advertise<std_msgs::Int32>(turtle_name+"/xLocation", 10);
	//y_pub = nh.advertise<std_msgs::Int32>(turtle_name+"/yLocation", 10);
	//theta_pub = nh.advertise<std_msgs::Float32>(turtle_name+"/heading", 10);
	odom_pub = nh.advertise<nav_msgs::Odometry>(turtle_name+"/odom", 1);
	camPose_pub = nh.advertise<geometry_msgs::PoseWithCovarianceStamped>(turtle_name+"/amcl_pose", 1, true);
	particleCloudPub = nh.advertise<geometry_msgs::PoseArray>(turtle_name+"/particlecloud",1,true);
	particleCloud.poses.resize(1);
   
        pub = it.advertise("camera/image_processed", 1);
	
	ros::Rate loop_rate(20);
	while( ros::ok() ) {
	        ros::spinOnce();
		loop_rate.sleep();
	}
   
    	ROS_INFO("tutorialROSOpenCV::main.cpp::No error.");

	//Delete the global pointer
	delete(odom_broadcaster);
 }

