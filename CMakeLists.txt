cmake_minimum_required(VERSION 3.0.2)
project(virtual_6dof)

find_package(catkin REQUIRED COMPONENTS
  gazebo_ros
  geometry_msgs
  sensor_msgs
  std_msgs
  urdf
  xacro
)

catkin_package(
  CATKIN_DEPENDS gazebo_ros geometry_msgs sensor_msgs std_msgs urdf xacro
)

# 安装资源文件
install(DIRECTORY urdf launch config
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)