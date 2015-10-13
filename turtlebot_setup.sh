#Define turtlebot_name in each turtlebot's .bashrc, like so: 
#export TURTLEBOT_NAME=robotname

export TURTLEBOT_3D_SENSOR=kinect

#export ROS_MASTER_URI=http://${TURTLEBOT_NAME}bot.dynamic.ucsd.edu:11311
#export ROS_MASTER_URI=http://calipso.dynamic.ucsd.edu:11311
#export ROS_MASTER_URI=http://lambrusco.ucsd.edu:11311
#export ROS_MASTER_URI=http://mencia.ucsd.edu:11311
#export ROS_MASTER_URI=http://abrusco.ucsd.edu:11311
export ROS_MASTER_URI=http://132.239.20.206:11311 # This is moscatto

export ROS_HOSTNAME=${TURTLEBOT_NAME}bot.dynamic.ucsd.edu

export ROBOT=${TURTLEBOT_NAME}                                                                                                  

source /opt/ros/indigo/setup.bash
source ~/catkin_ws/devel/setup.bash

alias catkin_make_turtlebot='catkin_make --pkg turtlebot_deployment turtlebot_move_to_goal simple_navigation_goals test_pkg'
