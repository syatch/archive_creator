#set version of cmake
cmake_minimum_required(VERSION 3.10)

#name of project
project(archive_creator CXX)

#add subdirectory
#add_subdirectory()

#add library
#target_link_libraries()

#create archive_creator from main.cpp archive_creator.cpp and archive_creator.hpp
add_executable(archive_creator main.cpp archive_creator.cpp archive_creator.hpp)

