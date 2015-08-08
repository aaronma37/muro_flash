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
  message_generation
)

find_package(OpenCV REQUIRED)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)

#add_message_files( DIRECTORY msg FILES  pathData.msg )
                                        #PoseWithName.msg

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

add_executable(slider src/slider.cpp)
add_dependencies(slider ${catkin_EXPORTED_TARGETS})
target_link_libraries(slider
${catkin_LIBRARIES}
)
add_executable(movementTest src/movementTest.cpp)
add_dependencies(movementTest ${catkin_EXPORTED_TARGETS})
target_link_libraries(movementTest
${catkin_LIBRARIES}
)

add_executable(hover src/hover.cpp)
add_dependencies(hover ${catkin_EXPORTED_TARGETS})
target_link_libraries(hover
${catkin_LIBRARIES}
)

add_executable(flight src/flight.cpp)
add_dependencies(flight ${catkin_EXPORTED_TARGETS})
target_link_libraries(flight
${catkin_LIBRARIES}
)

add_executable(quadcopter_ekf_multiple src/quadcopter_ekf_multiple.cpp)
add_dependencies(quadcopter_ekf_multiple ${catkin_EXPORTED_TARGETS})
target_link_libraries(quadcopter_ekf_multiple
${catkin_LIBRARIES}
)

add_executable(path_follower src/path_follower.cpp)
add_dependencies(path_follower ${catkin_EXPORTED_TARGETS})
target_link_libraries(path_follower
${catkin_LIBRARIES}
)

add_executable(path_follower_test src/path_follower_test.cpp)
add_dependencies(path_follower_test ${catkin_EXPORTED_TARGETS})
target_link_libraries(path_follower_test
${catkin_LIBRARIES}
)

add_message_files(FILES Navdata.msg)
