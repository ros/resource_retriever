cmake_minimum_required(VERSION 3.10)
project(libcurl_vendor C)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_vendor_package REQUIRED)

find_package(CURL QUIET)

ament_vendor(libcurl_vendor
  SATISFIED ${CURL_FOUND}
  VCS_URL https://github.com/curl/curl.git
  VCS_VERSION curl-7_81_0
  CMAKE_ARGS
    -DENABLE_MANUAL:BOOL=OFF
    -DCURL_ENABLE_SSL:BOOL=ON  # default, but just to make clear
    -DUSE_LIBIDN2:BOOL=OFF
)

ament_package(
  CONFIG_EXTRAS "libcurl_vendor-extras.cmake.in"
)
