ament_add_gtest(${PROJECT_NAME}_test test.cpp)

if(TARGET ${PROJECT_NAME}_test)
  target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME})
endif()
