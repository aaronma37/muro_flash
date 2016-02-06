#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/image_encodings.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <tf/transform_broadcaster.h>
#include <geometry_msgs/Pose.h>
image_transport::Subscriber sub;
image_transport::Publisher pub;
static const char WINDOW1[]="/Projector/image";

void imageProcessing(const sensor_msgs::ImageConstPtr& raw_image){

	cv_bridge::CvImagePtr cv_ptr_raw;

	try {
  		cv_ptr_raw=cv_bridge::toCvCopy(raw_image,sensor_msgs::image_encodings::BGR8);
	}
	catch (cv_bridge::Exception& e) {

		ROS_ERROR("Cannot convert image");
		return;
	}

	cv::Point pt;
	pt.x=10;
	pt.y=8;
 
	cv::Point pt1;
	pt1.x=20;
	pt1.y=16;
	cv::rectangle(cv_ptr_raw->image,pt,pt1,cv::Scalar(0,0,255),1,8);

	cv::circle(cv_ptr_raw->image, pt, 3.0, cv::Scalar(0, 255, 0), 5); 

    cv::imshow(WINDOW1, cv_ptr_raw->image);
    pub.publish(cv_ptr_raw->toImageMsg());

}

int main (int argc,char **argv){

	ros::init(argc,argv,"projector");

	ros::NodeHandle n;
	image_transport::ImageTransport it(n);
	cv::namedWindow(WINDOW1,CV_WINDOW_AUTOSIZE);

	pub=it.advertise("/projector/output",1);
	sub = it.subscribe("/usb_cam/image_raw",1,imageProcessing);

	ros::spin();

	cv::destroyWindow(WINDOW1);

}

