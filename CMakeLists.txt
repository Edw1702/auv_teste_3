cmake_minimum_required(VERSION 3.8)
project(auv_teste_3)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(behaviortree_cpp REQUIRED)

add_executable(executar_teste3 src/executar_teste3.cpp)
ament_target_dependencies(executar_teste3 rclcpp behaviortree_cpp)

install(TARGETS
  executar_teste3
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()