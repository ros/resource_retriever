# Copyright 2022 Open Source Robotics Foundation, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

find_package(CURL QUIET)

if(NOT CURL_FOUND)
  if(WIN32)
    # Chocolatey package is compiled with mingw, which uses these suffixes for libraries
    list(APPEND CMAKE_FIND_LIBRARY_SUFFIXES ".dll.a" ".a")
  endif()
  find_program(_curl_program NAMES curl curl.exe)
  if(_curl_program)
    get_filename_component(_curl_prefix "${_curl_program}" DIRECTORY)
    message(STATUS "Looking for CURL in ${_curl_prefix}")
    find_package(CURL REQUIRED HINTS "${_curl_prefix}")
  else()
    find_package(CURL REQUIRED)
  endif()
endif()
