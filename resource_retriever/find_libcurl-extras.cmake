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


function(locate_chocolatey_curl output_var)
  set(location "${output_var}-NOTFOUND")

  # Chocolatey package is compiled with mingw, which uses these suffixes for libraries
  list(APPEND CMAKE_FIND_LIBRARY_SUFFIXES ".dll.a" ".a")

  # Find the chocolatey curl executable - ignore curl shipped with Windows 10
  set(_old_ignore_paths "${CMAKE_IGNORE_PATH}")
  list(APPEND CMAKE_IGNORE_PATH "C:/Windows/System32")
  find_program(_curl_program NAMES curl curl.exe)
  # Undo ignoring of the system paths
  set(CMAKE_IGNORE_PATH "${_old_ignore_paths}")

  if(_curl_program)
    # Check if this is a shimgen executable
    execute_process(COMMAND "${_curl_program}" --shimgen-noop OUTPUT_VARIABLE shimgen_output)
    message(STATUS "Shimgen: ${shimgen_output}")

    # Look for output that says the real path to executable
    string(REGEX MATCH "path to executable: ([^\r\n]+)" regex_match "${shimgen_output}")
    if(regex_match)
      message(STATUS "Found chocolatey curl: '${CMAKE_MATCH_1}'")
      # Get .../bin directory
      get_filename_component(dir_containing_curl "${CMAKE_MATCH_1}" DIRECTORY)
      # The prefix to look for libcurl is the directory containing bin
      get_filename_component(dir_containing_bin "${dir_containing_curl}" DIRECTORY)
      set(location "${dir_containing_bin}")
    endif()
  endif()

  set("${output_var}" "${location}" PARENT_SCOPE)
endfunction()


macro(find_curl_win32)
  find_package(CURL QUIET)

  if(NOT CURL_FOUND)
    locate_chocolatey_curl(choco_curl_location)
    if(choco_curl_location)
      message(STATUS "Looking for CURL in ${choco_curl_location}")
      # Add extra search paths to find_path/find_library to assist FindCURL.cmake
      list(APPEND CMAKE_INCLUDE_PATH "${choco_curl_location}")
      list(APPEND CMAKE_LIBRARY_PATH "${choco_curl_location}")
      # Also set HINTS in case the chocolatey package ever provides curlConfig.cmake
      find_package(CURL REQUIRED HINTS "${choco_curl_location}")
    endif()
  endif()
endmacro()


macro(find_curl)
  if(WIN32)
    find_curl_win32()
  endif()

  if(NOT CURL_FOUND)
    find_package(CURL REQUIRED)
  endif()
endmacro()

find_curl()
