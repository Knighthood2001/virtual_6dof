#!/usr/bin/env python3
"""
按 prefix 逐个生成 controllers yaml，+ 一个共享 yaml。
用法: python3 gen_controllers_yaml.py p1 p2 ...
输出:
  config/virtual_6dof_controllers_p1.yaml
  config/virtual_6dof_controllers_p2.yaml
  config/virtual_6dof_controllers_shared.yaml
"""

import os
import sys

PID = {
    'x_trans': {'p': 800.0, 'i': 0.0, 'd': 80.0},
    'y_trans': {'p': 800.0, 'i': 0.0, 'd': 80.0},
    'z_trans': {'p': 800.0, 'i': 0.0, 'd': 80.0},
    'roll':    {'p': 300.0, 'i': 0.0, 'd': 30.0},
    'pitch':   {'p': 300.0, 'i': 0.0, 'd': 30.0},
    'yaw':     {'p': 300.0, 'i': 0.0, 'd': 30.0},
}

SECTION_COMMENT = {
    'x_trans': 'X轴平移控制器',
    'y_trans': 'Y轴平移控制器',
    'z_trans': 'Z轴平移控制器',
    'roll':    'Roll旋转控制器',
    'pitch':   'Pitch旋转控制器',
    'yaw':     'Yaw旋转控制器',
}


def fmt_pid(pid):
    return '{{p: {p}, i: {i}, d: {d}}}'.format(
        p=pid['p'], i=pid['i'], d=pid['d'])


def make_platform_yaml(prefix):
    """单个 prefix 的 yaml：只包含该 prefix 的位置控制器"""
    lines = []
    for joint_key, pid in PID.items():
        lines.append('# {name}'.format(name=SECTION_COMMENT[joint_key]))
        lines.append('{p}_virtual_6dof_{k}_joint_position_controller:'.format(
            p=prefix, k=joint_key))
        lines.append('  type: position_controllers/JointPositionController')
        lines.append('  joint: {p}_virtual_6dof_{k}_joint'.format(
            p=prefix, k=joint_key))
        lines.append('  pid: {pid}'.format(pid=fmt_pid(pid)))
        lines.append('')
    while lines and lines[-1] == '':
        lines.pop()
    return '\n'.join(lines) + '\n'


def make_shared_yaml(prefixes):
    """共享 yaml：joint_state_controller + gazebo_ros_control PID gains"""
    lines = []
    lines.append('# 关节状态控制器（共享，只 spawn 一次）')
    lines.append('joint_state_controller:')
    lines.append('  type: joint_state_controller/JointStateController')
    lines.append('  publish_rate: 50.0')
    lines.append('')

    lines.append('# gazebo_ros_control 级别的 PID gains（消除 "No p gain specified" 警告）')
    lines.append('gazebo_ros_control:')
    lines.append('  pid_gains:')
    for prefix in prefixes:
        for joint_key, pid in PID.items():
            joint_name = '{p}_virtual_6dof_{k}_joint'.format(p=prefix, k=joint_key)
            lines.append('    {jn}: {pid}'.format(jn=joint_name, pid=fmt_pid(pid)))
    lines.append('')
    while lines and lines[-1] == '':
        lines.pop()
    return '\n'.join(lines) + '\n'


def main():
    prefixes = sys.argv[1:]
    if not prefixes:
        print('Usage: gen_controllers_yaml.py <prefix> [<prefix> ...]',
              file=sys.stderr)
        sys.exit(1)

    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_dir = os.path.normpath(os.path.join(script_dir, '..', 'config'))
    os.makedirs(output_dir, exist_ok=True)

    # 1) 每个 prefix 一份平台 yaml
    for prefix in prefixes:
        path = os.path.join(output_dir,
                            'virtual_6dof_controllers_{p}.yaml'.format(p=prefix))
        with open(path, 'w') as f:
            f.write(make_platform_yaml(prefix))
        print('生成: {}'.format(path))

    # 2) 共享 yaml
    shared_path = os.path.join(output_dir, 'virtual_6dof_controllers_shared.yaml')
    with open(shared_path, 'w') as f:
        f.write(make_shared_yaml(prefixes))
    print('生成: {}'.format(shared_path))


if __name__ == '__main__':
    main()