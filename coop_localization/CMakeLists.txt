cmake_minimum_required(VERSION 2.8.3)
project(coop_localization)

# bfl (Bayesian Filtering Library) is a third party package that uses pkg-config
find_package(PkgConfig)
pkg_check_modules(BFL REQUIRED orocos-bfl)

include_directories(${BFL_INCLUDE_DIRS})
link_directories(${BFL_LIBRARY_DIRS})


find_package( catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  image_transport
  cv_bridge
  tf
  message_generation
)

find_package( OpenCV REQUIRED)

include_directories(
  include
  ${roscpp_INCLUDE_DIRS}
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${aruco_INCLUDE_DIRS}
)
add_message_files( FILES  PoseRelative.msg )
generate_messages( DEPENDENCIES  std_msgs geometry_msgs )


catkin_package(CATKIN_DEPENDS 
  roscpp 
  cv_bridge 
  opencv2 
  std_msgs 
  image_transport 
  tf 
  message_runtime
)


add_executable( exteroceptive_sensor src/exteroceptive_sensor.cpp src/name_map.cpp)
target_link_libraries( exteroceptive_sensor
  ${OpenCV_LIBS}
  ${catkin_LIBRARIES}
  $ENV{aruco_LIBRARIES}
)


add_executable( sensor_fusion src/sensor_fusion.cpp)
target_link_libraries( sensor_fusion
  ${catkin_LIBRARIES}
  ${BFL_LIBRARIES}
)


