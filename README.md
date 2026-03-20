# virtual_6dof_simulation_package
痛点: 一些末端设备的安装位置仿真通常需要手动调整末端设备的位姿参数，且难以在运行时动态改变，每次仿真都需要重新进行调整，调整好后，重新运行仿真。
解决方案
将6自由度的虚拟关节导入，使其能够动态“嵌入”到其他仿真模型中，并支持运行时调整安装位姿。

# 安装步骤

```shell
cd ~/catkin_ws/src
git clone https://github.com/Knighthood2001/virtual_6dof.git
cd ..
catkin_make
```
这时候这个ros功能包就安装好了，可以使用了。

# 使用方法
## 直接使用
我在这里提供了例子，在[example/demo.xacro](example/demo.xacro)中，你可以直接运行
```bash
roslaunch virtual_6dof demo.launch
```

然后你就可以驱动其进行运动了

控制 X 轴平移：
```bash
rostopic pub /virtual_6dof_x_trans_joint_position_controller/command std_msgs/Float64 "data: 0.5" -1
```
控制 Y 轴平移：

```bash
rostopic pub /virtual_6dof_y_trans_joint_position_controller/command std_msgs/Float64 "data: 0.5" -1
```
控制 Z 轴平移：

```bash
rostopic pub /virtual_6dof_z_trans_joint_position_controller/command std_msgs/Float64 "data: 1.0" -1
```
控制 Roll 轴旋转：

```bash
rostopic pub /virtual_6dof_roll_joint_position_controller/command std_msgs/Float64 "data: 0.8" -1
```
控制 Pitch 轴旋转：

```bash
rostopic pub /virtual_6dof_pitch_joint_position_controller/command std_msgs/Float64 "data: 0.4" -1
```
控制 Yaw 轴旋转：

```bash
rostopic pub /virtual_6dof_yaw_joint_position_controller/command std_msgs/Float64 "data: 1.2" -1
```

省去了每次设置安装位置的时候需要重启仿真，重新调整安装位置的问题。

## 在你自己的项目中导入我的模块
在这边我写了一个简单的例子，在example文件夹中。例如example是你自己的一个项目，你需要确定你的雷达的安装位置，你可以通过以下步骤：
1. 设置xacro文件
在[example/demo.xacro](example/demo.xacro)中，你通过设置

```xml
<xacro:include filename="$(find virtual_6dof)/urdf/virtual_6dof.xacro" />
```

导入virtual_6dof模块，并通过以下方式设置安装位置
```xml
<xacro:virtual_6dof 
    parent_link="world"              
    end_link="simple_lidar_link"> 
    <origin xyz="0 0 1" rpy="0 0 0"/>
    <origin xyz="1 0 0" rpy="0 0 0"/>
</xacro:virtual_6dof>
```

2. 设置launch文件
在你的launch文件中，你需要导入virtual_6dof的launch文件
```xml
<include file="$(find virtual_6dof)/launch/virtual_6dof_control.launch" />
```

然后你就能使用了。使用方式和上面的一样。




