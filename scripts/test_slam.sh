#!/bin/sh

world_pkg=$(rospack find world_pkg)
ws_path=$(echo ${world_pkg}/../..)
cd $ws_path
catkin_make
echo "Launching turtlebot_world..."
xterm -e "cd ${ws_path}; source /devel/setup.bash; roslaunch turtlebot_gazebo turtlebot_world.launch world_file:=${world_pkg}/worlds/project_world.world" &
sleep 5
echo "Launching turtlebot_world is finished"
echo "Launching gmapping_demo..."
xterm -e "cd ${ws_path}; source devel/setup.bash; roslaunch turtlebot_gazebo gmapping_demo.launch" &
sleep 5
echo "Launching gmapping_demo is finished"
echo "Launching view_navigation..."
xterm -e "cd ${ws_path}; source devel/setup.bash; roslaunch turtlebot_rviz_launchers view_navigation.launch" &
sleep 5
echo "Launching view_navigation is finished"
echo "Launching turtlebot_teleop..."
xterm -e "cd ${ws_path}; source devel/setup.bash; roslaunch turtlebot_teleop keyboard_teleop.launch" &
echo "Launching turtlebot_teleop is finished"

