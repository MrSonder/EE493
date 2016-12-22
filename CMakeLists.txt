cmake_minimum_required(VERSION 2.8)
project( opencv_executable )
find_package( OpenCV REQUIRED )
FIND_LIBRARY(WIRINGPI_LIBRARY wiringPi /home/pi/wiringPi)

add_executable( executable MotorDriveFunctional.cpp )
target_link_libraries( executable ${OpenCV_LIBS} ${CATKIN_LIBRARIES} ${WIRINGPI_LIBRARY} )



