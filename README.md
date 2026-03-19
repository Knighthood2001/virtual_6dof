# 确定雷达安装位置的仿真
痛点: 雷达安装位置的仿真通常需要手动调整雷达的位姿参数，且难以在运行时动态改变，每次仿真都需要重新进行调整，调整好后，重新运行仿真。

将6自由度雷达平台模块化，使其能够动态“嵌入”到其他仿真模型中，并支持运行时调整安装位姿

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
