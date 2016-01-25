cmake_minimum_required(VERSION 2.8.3)
project(turtlebot_camera_localization)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  image_transport
  cv_bridge
  tf
  turtlesim
)

find_package(OpenCV REQUIRED)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${aruco_INCLUDE_DIRS}
  /usr/local/include/aruco
#  /home/evan/OpenCV/aruco-1.3.0/src
)

catkin_package(
#  INCLUDE_DIRS
#  LIBRARIES
#  DEPENDS
  CATKIN_DEPENDS roscpp cv_bridge opencv2 std_msgs image_transport tf
)

add_executable(camera_localization src/main.cpp)
target_link_libraries(camera_localization
  ${OpenCV_LIBS}
  ${catkin_LIBRARIES}
  ${aruco_LIBRARIES}
  /usr/local/lib/libaruco.so
#  /home/evan/OpenCV/aruco-1.3.0/build/src/libaruco.so

)

add_executable(tf_broadcaster src/tf_broadcaster.cpp)
target_link_libraries(tf_broadcaster ${catkin_LIBRARIES})


add_executable(turtlebot_camera_localization src/main.cpp)
target_link_libraries(turtlebot_camera_localization
  ${OpenCV_LIBS}
  ${catkin_LIBRARIES}
  /home/evangravelle/catkin_ws/src/Aruco/build/src/libaruco.so
#  /home/evan/OpenCV/aruco-1.3.0/build/src/libaruco.so
)
