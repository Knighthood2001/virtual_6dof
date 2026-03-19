#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include <vector>

// 全局发布器（控制6个关节）
ros::Publisher x_pub, y_pub, z_pub, roll_pub, pitch_pub, yaw_pub;

// 候选安装位置列表（可根据需求修改）
std::vector<std::vector<float>> candidate_positions = {
  {0.0, 0.0, 1.0, 0.0, 0.0, 0.0},   // 位置1：原点，无旋转
  {2.0, 0.0, 1.5, 0.0, 0.2, 0.0},   // 位置2：X=2m，Z=1.5m，Pitch=0.2rad
  {0.0, 2.0, 1.2, 0.1, 0.0, 0.3},   // 位置3：Y=2m，Roll=0.1rad，Yaw=0.3rad
  {3.0, 1.0, 2.0, 0.0, 0.5, 0.0},   // 位置4：X=3m,Y=1m,Z=2m，Pitch=0.5rad
  {1.0, 1.0, 1.8, 0.2, 0.3, 0.2}    // 位置5：全维度偏移
};

void publishJointCmd(float x, float y, float z, float roll, float pitch, float yaw) {
  std_msgs::Float64 cmd;

  // X轴平移
  cmd.data = x;
  x_pub.publish(cmd);

  // Y轴平移
  cmd.data = y;
  y_pub.publish(cmd);

  // Z轴平移
  cmd.data = z;
  z_pub.publish(cmd);

  // Roll（绕X轴旋转）
  cmd.data = roll;
  roll_pub.publish(cmd);

  // Pitch（绕Y轴旋转）
  cmd.data = pitch;
  pitch_pub.publish(cmd);

  // Yaw（绕Z轴旋转）
  cmd.data = yaw;
  yaw_pub.publish(cmd);

  ROS_INFO("发布控制指令：X=%.2f, Y=%.2f, Z=%.2f | Roll=%.2f, Pitch=%.2f, Yaw=%.2f",
           x, y, z, roll, pitch, yaw);
}

int main(int argc, char** argv) {
  // 初始化ROS节点（节点名与项目名匹配）
  ros::init(argc, argv, "radar_6dof_control_node");
  ros::NodeHandle nh;

  // 发布话题：/radar_6dof/[控制器名]/command（与项目名强绑定）
  x_pub = nh.advertise<std_msgs::Float64>("/radar_6dof/x_trans_joint_position_controller/command", 10);
  y_pub = nh.advertise<std_msgs::Float64>("/radar_6dof/y_trans_joint_position_controller/command", 10);
  z_pub = nh.advertise<std_msgs::Float64>("/radar_6dof/z_trans_joint_position_controller/command", 10);
  roll_pub = nh.advertise<std_msgs::Float64>("/radar_6dof/roll_joint_position_controller/command", 10);
  pitch_pub = nh.advertise<std_msgs::Float64>("/radar_6dof/pitch_joint_position_controller/command", 10);
  yaw_pub = nh.advertise<std_msgs::Float64>("/radar_6dof/yaw_joint_position_controller/command", 10);

  // 等待控制器准备就绪
  ros::Duration(2.0).sleep();
  
  // 设置循环频率（10Hz）
  ros::Rate rate(10);

  ROS_INFO("等待5秒让机器人稳定...");
  ros::Duration(5.0).sleep();

  // 遍历所有候选位置，逐个控制雷达移动
  for (int i = 0; i < candidate_positions.size(); i++) {
    if (!ros::ok()) break;

    // 获取当前候选位置参数
    float x = candidate_positions[i][0];
    float y = candidate_positions[i][1];
    float z = candidate_positions[i][2];
    float roll = candidate_positions[i][3];
    float pitch = candidate_positions[i][4];
    float yaw = candidate_positions[i][5];

    // 发布控制指令
    ROS_INFO("===== 测试第%d个安装位置 =====", i+1);
    publishJointCmd(x, y, z, roll, pitch, yaw);

    // 等待5秒，让雷达稳定并完成扫描
    ROS_INFO("等待5秒让机器人移动到新位置...");
    ros::Duration(5.0).sleep();

    rate.sleep();
    ros::spinOnce();
  }

  // 遍历完成后，回到初始位置
  ROS_INFO("===== 遍历完成，返回初始位置 =====");
  publishJointCmd(0.0, 0.0, 1.0, 0.0, 0.0, 0.0);

  ROS_INFO("程序结束");
  return 0;
}
