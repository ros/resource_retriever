cmake_minimum_required(VERSION 3.20)
project(resource_retriever)

# The C++/C standard comes from ament_ros_defaults (cxx_std_20 / c_std_17),
# linked PRIVATE below so it applies to this library's own translation units
# only and is not propagated to downstream consumers.
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake_ros REQUIRED)
find_package(ament_cmake_ros_core REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(CURL REQUIRED)

# TODO(wjwwood): split cpp and python apis into separate packages
add_library(${PROJECT_NAME}
  src/retriever.cpp
  src/exception.cpp
  src/plugins/retriever_plugin.cpp
  src/plugins/filesystem_retriever.cpp
  src/plugins/curl_retriever.cpp
)
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)
target_link_libraries(${PROJECT_NAME} PRIVATE
  ament_index_cpp::ament_index_cpp
  CURL::libcurl
)
# ament_ros_defaults is an INTERFACE target providing cxx_std_20 / c_std_17.
# Linked PRIVATE so the standard applies to our own sources without leaking
# into the exported interface (a SHARED library does not propagate PRIVATE
# link requirements to consumers).
target_link_libraries(${PROJECT_NAME} PRIVATE ament_cmake_ros_core::ament_ros_defaults)

# Causes the visibility macros to use dllexport rather than dllimport,
# which is appropriate when building the dll but not consuming it.
target_compile_definitions(${PROJECT_NAME} PRIVATE "RESOURCE_RETRIEVER_BUILDING_LIBRARY")

# Export old-style CMake variables
ament_export_include_directories("include/${PROJECT_NAME}")

# Export modern CMake targets
ament_export_targets(${PROJECT_NAME} HAS_LIBRARY_TARGET)

ament_python_install_package(${PROJECT_NAME}
  PACKAGE_DIR src/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_lint_auto REQUIRED)

  ament_lint_auto_find_test_dependencies()

  ament_add_gtest(${PROJECT_NAME}_test test/test.cpp)
  target_link_libraries(${PROJECT_NAME}_test
    ${PROJECT_NAME}
    ament_cmake_ros_core::ament_ros_defaults
  )

  find_package(ament_cmake_pytest REQUIRED)
  ament_add_pytest_test(resource_retriever_python_test test/test.py
    APPEND_ENV PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}
    TIMEOUT 60
  )

  install(
    FILES test/test.txt
    DESTINATION share/${PROJECT_NAME}/test
  )
endif()

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  DIRECTORY include/
  DESTINATION include/${PROJECT_NAME}
)

ament_package()
