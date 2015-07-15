cmake_minimum_required(VERSION 2.8.3)
project(test_pkg)

## Find catkin macros and libraries
find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  geometry_msgs
  nav_msgs
  move_base_msgs
  actionlib
  genmsg
)

find_package(OpenCV REQUIRED)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)

#add_message_files( DIRECTORY msg FILES PoseWithName.msg )

#generate_messages(
#  DEPENDENCIES
#  std_msgs
#  geometry_msgs
#)

catkin_package(
  #INCLUDE_DIRS include
  LIBRARIES turtlebot_deployment
  CATKIN_DEPENDS roscpp cv_bridge opencv2 std_msgs geometry_msgs nav_msgs move_base_msgs actionlib message_runtime
  DEPENDS
)



###########
## Build ##
###########

file(GLOB_RECURSE HEADER_FILES include/*.h)

#add_executable(simple_deployment src/simple_deployment.cpp src/agent.cpp src/VoronoiDiagramGenerator.cpp ${HEADER_FILES})
#add_dependencies(simple_deployment turtlebot_deployment_gencpp)
#target_link_libraries(simple_deployment
#  ${OpenCV_LIBS}
#  ${catkin_LIBRARIES}
#)

add_executable(quadcopter_ekf src/quadcopter_ekf.cpp)
add_dependencies(quadcopter_ekf ${catkin_EXPORTED_TARGETS})
target_link_libraries(quadcopter_ekf
${catkin_LIBRARIES}
)

add_executable(sample src/sample.cpp)
add_dependencies(sample ${catkin_EXPORTED_TARGETS})
target_link_libraries(sample
${catkin_LIBRARIES}
)
