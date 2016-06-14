//Jonathan Hechtbauer, UCSD, MURO, 2016
//Based on:
//Katherine Liu, UCSD, Dynamics Systems and Controls, 2014

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <aruco/aruco.h>
#include <aruco/cameraparameters.h>
#include <aruco/cvdrawingutils.h>
#include <aruco/markerdetector.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h> // getenv 	
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "geometry_msgs/Quaternion.h"
#include <geometry_msgs/Twist.h>
#include <coop_localization/PoseRelative.h>
#include <map>
#include <boost/assign/list_of.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/assign/list_of.hpp>

#include "name_map.h" //List that contains the information for every marker
//map name: name\_map
//key:   int
//value: tuple{name(string),side(int),size(float)}

std::map<std::string, float> n,x_cum, y_cum, roll_cum;
std::map<std::string, ros::Publisher> pub_map;
float xx, yy;
const double PI = 3.14159265;
std::string home, yml_file, RobotName;

// ROS
image_transport::Publisher pub;
ros::Publisher pose_rel_pub;

geometry_msgs::PoseWithCovarianceStamped camPose; //TODO
coop_localization::PoseRelative relative_pose_msg; 
// ArUco
aruco::MarkerDetector MDetector;
vector<aruco::Marker> Markers;
aruco::CameraParameters CamParam;
// OpenCV
namespace enc = sensor_msgs::image_encodings;
static const char WINDOW[] = "MULTIAGENT LOCALIZATION";// For the OpenCV-window.

std::string getEnvVar(std::string const& key)
{
	char const* val = getenv(key.c_str()); 
	return val == NULL ? std::string() : std::string(val);
}

//This function is called everytime a new image is published
void image_cb(const sensor_msgs::ImageConstPtr& original_image)
{ 
	//Convert ROS-image-msg to a CvImage (OpenCV)
	cv_bridge::CvImagePtr cv_ptr; 
	cv_ptr = cv_bridge::toCvCopy(original_image, enc::BGR8);


	//Call to Aruco to identify markers
	MDetector.detect(cv_ptr->image, Markers,CamParam,1); //Size respected later.

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~~~~~~~~~~~~~~~START LOOP for the markers~~~~~~~~~~~~~~~~~~~~~
	float x, y, z;
	float roll,yaw,pitch;
	n.clear();
	x_cum.clear();
	y_cum.clear();
	roll_cum.clear();
	for (unsigned int i=0;i<Markers.size();i++) { // Runs through every detected marker
		int id = Markers[i].id;
		// Checks if the detected marker is defined in the name-map
		map<int,boost::tuple<std::string,int,std::string> >::iterator j=name_map.find(id);
		if (j != name_map.end() )
		{			
			std::string 	name = j->second.get<0>();
			int 		side = j->second.get<1>();
			std::string 	size = j->second.get<2>();
			double size_box = 0.200;
			double offset_kinect = 0.09;
			float size_, offset;

			if (size=="s") 		{size_=0.04; offset=0.075;} 
			else if (size=="m") 	{size_=0.14; offset=0.025;} 
			else if (size=="l") 	{size_=0.17; offset=0;} 

			using namespace cv;
			// Calculate the detected pose
			// According to https://github.com/warp1337/ros\_aruco
			x =  Markers[i].Tvec.at<Vec3f>(0,0)[2];//- and 0
			y = -Markers[i].Tvec.at<Vec3f>(0,0)[0];//+ and 1
			z = -Markers[i].Tvec.at<Vec3f>(0,0)[1];//+ and 2
			cv::Mat rot_mat(3,3,cv::DataType<float>::type);
			cv::Rodrigues(Markers[i].Rvec,rot_mat); //angles wrt to camera coords
			roll = acos(rot_mat.at<float>(2,2)) - CV_PI/2;
			pitch = atan2(rot_mat.at<float>(0,2), rot_mat.at<float>(1,2)) + CV_PI/2;
			yaw = atan2(rot_mat.at<float>(2,0), rot_mat.at<float>(2,1)) - CV_PI;

			n[name]++; // Increase number of detected markers. Saved in robot name.
			// Cumulate the measured values for every marker. Saved in robot name.
			x_cum[name] += x*size_ + size_box/2*cos(roll) - offset*sin(roll) - offset_kinect;
			y_cum[name] += y + size_box/2*sin(roll) + offset*cos(roll); 
			roll_cum[name] += roll + side*PI/2; // TODO side is missing


			ROS_INFO("Marker:%ld size:%5.2f",(long int)id,size_);
			ROS_INFO("x:%5.2f \n", x*size_ + size_box/2*cos(roll) - offset*sin(roll));

			// ~~~DRAW
			aruco::CvDrawingUtils::draw3dAxis(cv_ptr->image,Markers[i],CamParam);
			Markers[i].draw(cv_ptr->image,cv::Scalar(0,0,255),2);
			float x_pxl = Markers[i].getCenter().x;
			float y_pxl = Markers[i].getCenter().y;
		}

	}// ~~~~~~~~~~~~~~~~END LOOP for the markers~~~~~~~~~~~~~~~~~~~~~~~
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~~~~~~~~~~~~~~~START LOOP for the robots~~~~~~~~~~~~~~~~~~~~~~
	typedef map<std::string, float>::const_iterator MapIterator;
	for (MapIterator j = n.begin(); j != n.end(); j++) {

		std::string name = j->first;
		xx = x_cum[name]/n[name];
		yy = y_cum[name]/n[name];

		// ~~~ROS MSG
		//ros::Time current\_time;
		//current\_time = ros::Time::now();	
		//odom.header.stamp = current\_time; 
		//odom.pose.covariance[35] = .1237;
		geometry_msgs::Twist twist_msg;
		twist_msg.linear.x = xx;
		twist_msg.linear.y = yy;
		twist_msg.angular.z = yaw;
		relative_pose_msg.twist_relative = twist_msg;
		relative_pose_msg.parent_name = RobotName;
		pub_map[name].publish(relative_pose_msg);

		// ~~~CONSOLE
		ROS_INFO("\n\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
		ROS_INFO("Distance according to ROS convetions");
		ROS_INFO("x=forward y=left z=up");
		ROS_INFO("Distance in m");
		ROS_INFO( "x:%5.2f y:%5.2f \n", xx, yy);
		ROS_INFO("Angle values are relativ to the detected frame!");
		ROS_INFO("Angle in deg");
		ROS_INFO( "roll(x)=%5.2f pitch(y)=%5.2f yaw(z)=%5.2f", roll*(180.0/CV_PI), pitch*(180.0/CV_PI), yaw*(180.0/CV_PI));
		ROS_INFO("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n\n");

	} // ~~~~~~~~~~~~~~~~END LOOP for the robots~~~~~~~~~~~~~~~~~~~~~
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	//Display the tracked robots
	cv::imshow(WINDOW, cv_ptr->image);   
	cv::waitKey(3); //Add some delay in miliseconds.

	//Convert CvImage to a ROS image message and publish it on "camera/image\_processed".
	pub.publish(cv_ptr->toImageMsg());

}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "exteroceptive_sensor");
	ros::NodeHandle nh;

	// Get environment variables
	ros::param::param<string>("~RobotName", RobotName, "noname");
    	ROS_INFO("RobotName = %s", RobotName.c_str());
	home = getEnvVar(std::string("HOME"));
	yml_file = home + std::string("/catkin_ws/src/ucsd_ros_project/coop_localization/config/kinect_calib_jonathan.yml");
	// Read in the Kinect calibration file
	CamParam.readFromXMLFile(yml_file.c_str());


	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~~~~~~~~~~~~~~~START LOOP for the name\_map~~~~~~~~~~~~~~~~~
	std::string temp_name;
	for( map<int, boost::tuple<std::string, int, std::string> >::iterator ii=name_map.begin(); ii!=name_map.end(); ++ii)
	{
		// (*ii).second.get<0>()) means: Get the 1st variable of the map-value's tuple 
		if (temp_name != (*ii).second.get<0>())  // If it is a new robot name
		{
			temp_name = (*ii).second.get<0>();  
			pub_map[temp_name] = nh.advertise< coop_localization::PoseRelative>("/"+temp_name+"/pose_measured", 10,true);
		}			
	}// ~~~~~~~~~~~~~~~~END LOOP for the name\_map~~~~~~~~~~~~~~~~~~
	 // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        image_transport::ImageTransport it(nh);

	image_transport::Subscriber sub = it.subscribe("/camera/rgb/image_color", 1, image_cb);//Kinect

        cv::namedWindow(WINDOW, CV_WINDOW_AUTOSIZE); 
	MDetector.setThresholdParams(7,7);
	ros::spin();
	cv::destroyWindow(WINDOW);
 }

