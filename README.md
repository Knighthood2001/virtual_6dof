# virtual_6dof

将 6 自由度虚拟运动平台**嵌入**到任意 Gazebo 仿真模型中，支持运行时动态调整位姿、多个平台共存复用。

---

## 痛点与解决方案

| 痛点 | 解决方案 |
|------|----------|
| 末端设备安装位置每次仿真都要手动调 | 通过 xacro 参数动态传入安装位置 |
| 仿真运行中无法改变安装位姿 | 运行时通过 topic / rqt 调整 |
| 同一仿真需要多个 6DOF 平台 | 引入 `prefix` 参数，同一宏可实例化多次，名字互不冲突 |

---

## 项目结构

```
virtual_6dof/
├── urdf/
│   └── virtual_6dof.xacro      # 核心宏：6自由度运动平台
├── launch/
│   └── virtual_6dof_control.launch   # per-platform 控制器加载
├── config/
│   ├── virtual_6dof_controllers_p1.yaml   # 自动生成
│   ├── virtual_6dof_controllers_p2.yaml   # 自动生成
│   └── virtual_6dof_controllers_shared.yaml  # 自动生成
├── scripts/
│   └── gen_controllers_yaml.py    # 生成控制器配置 yaml
├── example/
│   ├── demo.xacro                 # 单平台示例
│   ├── dual_demo.xacro            # 双平台示例
│   ├── demo.launch
│   └── dual_demo.launch
├── rviz/
│   └── result.rviz
└── CMakeLists.txt
```

---

## 安装

```bash
cd ~/catkin_ws/src
git clone https://github.com/Knighthood2001/virtual_6dof.git
cd ..
catkin_make
source devel/setup.bash
```

---

## 快速启动

### 单平台 demo

```bash
# 1) 生成控制器配置
python3 $(rospack find virtual_6dof)/scripts/gen_controllers_yaml.py p1

# 2) 启动
roslaunch virtual_6dof example/demo.launch
```

### 双平台 dual_demo

```bash
python3 $(rospack find virtual_6dof)/scripts/gen_controllers_yaml.py p1 p2
roslaunch virtual_6dof example/dual_demo.launch
```

> **注意**：每次切换 demo/dual_demo 之前，需要重新运行 `gen_controllers_yaml.py`，因为不同 launch 文件对应的 prefix 列表不同。

---

## 控制方式

### 命令行

| 关节 | Topic |
|------|-------|
| X轴平移 | `/p1_virtual_6dof_x_trans_joint_position_controller/command` |
| Y轴平移 | `/p1_virtual_6dof_y_trans_joint_position_controller/command` |
| Z轴平移 | `/p1_virtual_6dof_z_trans_joint_position_controller/command` |
| Roll | `/p1_virtual_6dof_roll_joint_position_controller/command` |
| Pitch | `/p1_virtual_6dof_pitch_joint_position_controller/command` |
| Yaw | `/p1_virtual_6dof_yaw_joint_position_controller/command` |

示例（X轴移动 0.5m）：

```bash
rostopic pub /p1_virtual_6dof_x_trans_joint_position_controller/command \
    std_msgs/Float64 "data: 0.5" -1
```

双平台时同理，只是把 `p1` 换成 `p2`：

```bash
rostopic pub /p2_virtual_6dof_x_trans_joint_position_controller/command \
    std_msgs/Float64 "data: 0.5" -1
```


## 在你自己的项目中复用

### 1. xacro：导入宏 + 声明末端连杆

```xml
<?xml version="1.0"?>
<robot name="my_robot" xmlns:xacro="http://wiki.ros.org/xacro">

  <!-- 颜色材质（按需定义） -->
  <material name="red"><color rgba="1 0 0 1"/></material>
  <!-- 引入 6DOF 平台宏 -->
  <xacro:include filename="$(find virtual_6dof)/urdf/virtual_6dof.xacro" />
  <!-- 定义 world 连杆 -->
  <link name="world"/>
  <!-- 你的末端执行器（雷达等） -->
  <link name="my_sensor_link">
    <visual>
      <geometry><cylinder length="0.1" radius="0.05"/></geometry>
      <material name="red"/>
    </visual>
    <collision><geometry><sphere radius="0.02"/></geometry></collision>
    <inertial>
      <mass value="0.1"/>
      <inertia ixx="1e-5" ixy="0" ixz="0" iyy="1e-5" iyz="0" izz="1e-5"/>
    </inertial>
  </link>

  <!-- gazebo_ros_control 插件（整个 URDF 只加一次） -->
  <gazebo>
    <plugin name="gazebo_ros_control" filename="libgazebo_ros_control.so"/>
  </gazebo>
  <!-- 安装 6DOF 平台 -->
  <xacro:virtual_6dof
    prefix="sensor1"
    parent_link="world"
    end_link="my_sensor_link">
    <!-- base_joint 安装偏移 -->
    <origin xyz="0 0 0" rpy="0 0 0"/>
    <!-- end_effector_joint 安装偏移 -->
    <origin xyz="0 0 0" rpy="0 0 0"/>
  </xacro:virtual_6dof>

</robot>
```

### 2. launch：生成 yaml + 加载控制器

```xml
<?xml version="1.0"?>
<launch>
  <include file="$(find gazebo_ros)/launch/empty_world.launch"/>
  <param name="robot_description"
         command="$(find xacro)/xacro '$(find your_pkg)/urdf/my_robot.xacro'"/>
  <node name="spawn_model" pkg="gazebo_ros" type="spawn_model"
        args="-urdf -model my_robot -param robot_description" output="screen"/>
  <!-- 共享配置 -->
  <rosparam command="load"
            file="$(find virtual_6dof)/config/virtual_6dof_controllers_shared.yaml"/>
  <node name="state_controller_spawner" pkg="controller_manager" type="spawner"
        args="joint_state_controller"/>
  <!-- 该平台的控制器 -->
  <include file="$(find virtual_6dof)/launch/virtual_6dof_control.launch">
    <arg name="prefix" value="sensor1"/>
  </include>
  <node name="robot_state_publisher" pkg="robot_state_publisher"
        type="robot_state_publisher" output="screen"/>
</launch>
```

### 3. 启动前先生成 yaml

```bash
# 生成 controller 配置（prefix 和 xacro 中的 prefix 一致）
python3 $(rospack find virtual_6dof)/scripts/gen_controllers_yaml.py sensor1

# 启动
roslaunch your_pkg my_robot.launch
```

---

## 多平台复用

xacro 中调用两次宏，使用不同 prefix：

```xml
<xacro:virtual_6dof prefix="sensor1" parent_link="world" end_link="sensor_link1">
  <origin xyz="0 0 0" rpy="0 0 0"/>
  <origin xyz="1 0 0" rpy="0 0 0"/>
</xacro:virtual_6dof>

<xacro:virtual_6dof prefix="sensor2" parent_link="world" end_link="sensor_link2">
  <origin xyz="0 0 0" rpy="0 0 0"/>
  <origin xyz="-1 0 0" rpy="0 0 0"/>
</xacro:virtual_6dof>
```

launch 中 include 两次：

```xml
<include file="$(find virtual_6dof)/launch/virtual_6dof_control.launch">
  <arg name="prefix" value="sensor1"/>
</include>
<include file="$(find virtual_6dof)/launch/virtual_6dof_control.launch">
  <arg name="prefix" value="sensor2"/>
</include>

<node name="lock_joints" pkg="virtual_6dof" type="lock_joints.py">
  <param name="prefixes" value="sensor1 sensor2"/>
</node>
```

生成 yaml 时传入所有 prefix：

```bash
python3 $(rospack find virtual_6dof)/scripts/gen_controllers_yaml.py sensor1 sensor2
```

---

## 参数说明

### xacro 宏参数

| 参数 | 说明 |
|------|------|
| `prefix` | 所有 link/joint/ transmission 名字的前缀，保证多平台不冲突 |
| `parent_link` | 平台的父连杆（通常是 `world` 或你机器人的某个 link） |
| `end_link` | 平台的末端执行器连杆（需要你先在 xacro 中定义） |
| `origin_offset` (block) | base_joint 的安装偏移 |
| `end_effector_offset` (block) | end_effector_joint 的安装偏移 |

### PID 控制器参数

在 [scripts/gen_controllers_yaml.py](scripts/gen_controllers_yaml.py) 的 `PID` 字典中修改：

```python
PID = {
    'x_trans': {'p': 800.0, 'i': 0.0, 'd': 80.0},   # 平移关节
    'y_trans': {'p': 800.0, 'i': 0.0, 'd': 80.0},
    'z_trans': {'p': 800.0, 'i': 0.0, 'd': 80.0},
    'roll':    {'p': 300.0, 'i': 0.0, 'd': 30.0},   # 旋转关节
    'pitch':   {'p': 300.0, 'i': 0.0, 'd': 30.0},
    'yaw':     {'p': 300.0, 'i': 0.0, 'd': 30.0},
}
```

---

## 关节运动范围

| 关节 | 类型 | 范围 |
|------|------|------|
| X/Y/Z 平移 | prismatic | ±50.0 m |
| Roll/Pitch/Yaw | revolute | ±3.14 / ±3.14 / ±3.14 rad |

> 如需调整范围，修改 [urdf/virtual_6dof.xacro](urdf/virtual_6dof.xacro) 中的 `<limit>` 标签。
