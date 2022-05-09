set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

catkin_add_gtest(${PROJECT_NAME}_utest test.cpp)
if(TARGET ${PROJECT_NAME}_utest)
  target_link_libraries(${PROJECT_NAME}_utest ${PROJECT_NAME})
endif()

catkin_add_nosetests(test.py)
