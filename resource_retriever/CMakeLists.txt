cmake_minimum_required(VERSION 3.5)
project(resource_retriever)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

find_package(ament_cmake REQUIRED)
find_package(ament_index_cpp REQUIRED)
# TODO(wjwwood): remove libcurl_vendor and just use system curl when possible
find_package(libcurl_vendor REQUIRED)

# TODO(wjwwood): split cpp and python apis into separate packages and fix python api

add_library(${PROJECT_NAME} src/retriever.cpp)
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(${PROJECT_NAME}
  ament_index_cpp
  libcurl_vendor
)

ament_export_interfaces(${PROJECT_NAME})

if(BUILD_TESTING)
  # TODO(wjwwood): add ament linters

  # TODO(wjwwood): reenable tests from ROS 1
  # add_subdirectory(test EXCLUDE_FROM_ALL)
endif()

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

install(
  DIRECTORY include/
  DESTINATION include
)

ament_package()
