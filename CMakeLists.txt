cmake_minimum_required(VERSION 2.8)
project( opencv_executable )
find_package( OpenCV REQUIRED )
add_executable( executable ConnectThree.cpp )
target_link_libraries( executable ${OpenCV_LIBS} ${CATKIN_LIBRARIES})



