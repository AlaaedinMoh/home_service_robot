#!/bin/sh

world_pkg=$(rospack find world_pkg)
ws_path=$(echo ${world_pkg}/../..)
cd $ws_path
catkin_make

echo "Launching turtlebot_world..."
xterm -e "cd ${ws_path}; source /devel/setup.bash; roslaunch turtlebot_gazebo turtlebot_world.launch world_file:=${world_pkg}/worlds/project_world.world" &
sleep 5
echo "Launching turtlebot_world is finished"


echo "Launching amcl_demo..."
xterm -e "cd ${ws_path}; source devel/setup.bash; roslaunch turtlebot_gazebo amcl_demo.launch map_file:=${world_pkg}/worlds/project_world.yaml" &
sleep 2
echo "Launching amcl_demo is finished"

echo "Launching view_navigation..."
xterm -e "cd ${ws_path}; source devel/setup.bash; roslaunch turtlebot_rviz_launchers view_navigation.launch" &
sleep 5
echo "Launching amcl_demo is finished"

echo "Running add_markers node..."
xterm -e "cd ${ws_path}; source devel/setup.bash; rosrun add_markers add_markers _param:=add_markers" &
echo "Running add_markers node is finished"
