目前在example/dual_demo.xacro中
```yaml
  <xacro:virtual_6dof prefix="p1" parent_link="world" end_link="simple_link1">
    <origin xyz="0 0 0" rpy="0 0 0"/>
    <origin xyz="1 0 0" rpy="0 0 0"/>
  </xacro:virtual_6dof>
  <xacro:virtual_6dof prefix="p2" parent_link="world" end_link="simple_link2">
    <origin xyz="0 0 0" rpy="0 0 0"/>
    <origin xyz="-1 0 0" rpy="0 0 0"/>
  </xacro:virtual_6dof>
```

一旦把内容换成
```yaml
  <xacro:virtual_6dof prefix="p1" parent_link="world" end_link="simple_link1">
    <origin xyz="0 0 0" rpy="0 0 0"/>
    <origin xyz="1 0 1" rpy="0 0 0"/>
  </xacro:virtual_6dof>
  <xacro:virtual_6dof prefix="p2" parent_link="world" end_link="simple_link2">
    <origin xyz="0 0 0" rpy="0 0 0"/>
    <origin xyz="-1 0 1" rpy="0 0 0"/>
  </xacro:virtual_6dof>
```
就会报错

> [ERROR] [1780993298.414097094, 11.598000000]: Ignoring transform for child_frame_id "p2_virtual_6dof_z_trans_link" from authority "unknown_publisher" because of a nan value in the transform (-nan -nan -nan) (0.000000 0.000000 0.000000 1.000000)