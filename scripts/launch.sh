#!/bin/sh

world_pkg=$(rospack find world_pkg)
echo $world_pkg
ws_path=$(echo ${world_pkg}/../..)
#echo $ws_path
#echo ${ws_path}
cd $ws_path
#cd $(rospack find world_pkg)
#cd ../..
catkin_make
#source ${ws_path}/devel/setup.bash

xterm -e "cd ${ws_path}; source /devel/setup.bash; roslaunch turtlebot_gazebo turtlebot_world.launch world_file:=${world_pkg}/worlds/project_world.world" &
sleep 5
xterm  -e  " source /opt/ros/kinetic/setup.bash; roscore" & 
sleep 5
xterm  -e  " rosrun rviz rviz"
