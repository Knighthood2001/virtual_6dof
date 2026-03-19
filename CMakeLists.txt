cmake_minimum_required(VERSION 3.0.2)
project(virtual_6dof)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找依赖包
find_package(catkin REQUIRED COMPONENTS
  gazebo_ros
  geometry_msgs
  pcl_ros
  roscpp
  sensor_msgs
  std_msgs
  urdf
  xacro
)

# 声明catkin包
catkin_package(
  INCLUDE_DIRS
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS gazebo_ros geometry_msgs pcl_ros roscpp sensor_msgs std_msgs urdf xacro
  DEPENDS
)

# 包含目录
include_directories(
  ${catkin_INCLUDE_DIRS}
)

# 编译运动控制节点
add_executable(virtual_6dof_control_node src/virtual_6dof_control_node.cpp)
target_link_libraries(virtual_6dof_control_node ${catkin_LIBRARIES})

# 安装节点
install(TARGETS virtual_6dof_control_node 
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

# 安装其他文件
install(DIRECTORY urdf launch config
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)