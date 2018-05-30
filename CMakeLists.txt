cmake_minimum_required(VERSION 3.5)
project(mpu_test)
add_definitions(-std=c++0x -O2 -g)
set(CMAKE_CXX_STANDARD 11)
set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

include_directories(
    ./libmpu9150/wiringPi
    ./libmpu9150
    )
add_executable(mpu_test mpu_test.cpp ./libmpu9150/mpu9150.cpp)
target_link_libraries(mpu_test wiringPi)
