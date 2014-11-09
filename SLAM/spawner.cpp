#include <ros/ros.h>
#include <turtlesim/Spawn.h>
#include <turtlesim/Pose.h>
#include <turtlesim/Velocity.h>
#include <stdlib.h>
#include <math.h>


class SpawnerClass {
public: //Things that can be seen outside of class
  SpawnerClass();
  turtlesim::Pose pose1, pose2;
  private: //Things only seen by the class
  ros::Subscriber sub1, sub2;
  void poseCallback1(const turtlesim::PoseConstPtr& msg);
  void poseCallback2(const turtlesim::PoseConstPtr& msg);
};


SpawnerClass::SpawnerClass() {
  ros::NodeHandle n1, n2;
  //define sub as a subscriber to pose
  sub1 = n1.subscribe("/turtle1/pose", 1000, &SpawnerClass::poseCallback1, this);
  sub2 = n2.subscribe("/turtle2/pose", 1000, &SpawnerClass::poseCallback2, this);
  //ROS_INFO("The turtle is currently at x=%.3f y=%.3f", pose1.x, pose1.y);
  
}


void SpawnerClass::poseCallback1(const turtlesim::PoseConstPtr& msg) {  
  pose1 = *msg;
  ROS_INFO("The turtle is currently at x=%.3f y=%.3f", pose1.x, pose1.y);
}

void SpawnerClass::poseCallback2(const turtlesim::PoseConstPtr& msg) {
  pose2 = *msg;
  ROS_INFO("The turtle is currently at x=%.3f y=%.3f", pose2.x, pose2.y);
}


int main(int argc, char **argv) {

  ros::init(argc, argv, "spawner");
  ros::NodeHandle n1, n2;
  ros::Publisher pub1, pub2;
  
  //define srv as calling the existing service Spawn
  ros::ServiceClient spawnClient = n2.serviceClient<turtlesim::Spawn>("/spawn");
  turtlesim::Spawn srv;

  //seeds the random variable generator
  srand(time(0));

  //defines the request variables
  srv.request.x = 10 * float(rand())/float(RAND_MAX);
  srv.request.y = 10 * float(rand())/float(RAND_MAX);
  srv.request.theta = 2 * 3.14159 * float(rand())/float(RAND_MAX);
  srv.request.name = "turtle2";

  //waits half a second
  ros::Duration(0.5).sleep();

  bool success = spawnClient.call(srv);
  if(success) {
    ROS_INFO("Spawned a turtle named %s at (%f,%f).",
	     srv.response.name.c_str(), srv.request.x, srv.request.y);
  } else {
    ROS_INFO("Spawn failed.");
  }  

  //Calls the function SpawnerClass to subscribe to both
  SpawnerClass hi;        

  //Creates pubs, sets spin rate
  //define pub as a publisher outputting command_velocity
  pub1 = n1.advertise<turtlesim::Velocity>("/turtle1/command_velocity", 1000);
  pub2 = n2.advertise<turtlesim::Velocity>("/turtle2/command_velocity", 1000);

  ros::Rate rate(60);
 
  turtlesim::Velocity vel1, vel2;
  double dx, dy;

  
  while (ros::ok()) {

  ROS_INFO("IN WHILE LOOP The turtle is currently at x=%.3f y=%.3f", hi.pose1.x, hi.pose1.y);

    dx = hi.pose2.x - hi.pose1.x;
    dy = hi.pose2.y - hi.pose1.y;


    ROS_INFO("p1 is %f %f %f", hi.pose1.x, hi.pose1.y, hi.pose1.theta);
    ROS_INFO("p2 is %f %f %f", hi.pose2.x, hi.pose2.y, hi.pose2.theta);
    float theta1 = atan2(dy,dx); //between -pi and pi
    float theta2 = theta1 + 3.14159; //between 0 and 2*pi
    float delta = sqrt(pow(dx,2) + pow(dy,2));
    vel1.linear = fmin(2,delta/5);    
    vel2.linear = vel1.linear;


    //depends on the robot's current theta!
    if (dx < 0 && dy < 0) {
      vel1.angular = theta1 + 2*3.14159 - hi.pose1.theta;
      vel2.angular = theta2 + 2*3.14159 - hi.pose2.theta;
    }
    else if (dx < 0 ) {
      vel1.angular = theta1 - hi.pose1.theta;
      vel2.angular = theta2 - hi.pose2.theta;
    }

    pub1.publish(vel1);
    pub2.publish(vel2); 
    ros::spinOnce();
    rate.sleep();
    } 
}
