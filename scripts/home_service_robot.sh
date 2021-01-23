cd $(pwd)/../..
catkin_make

echo "Launching turtlebot_world..."
xterm -e "source devel/setup.bash; roslaunch turtlebot_gazebo turtlebot_world.launch world_file:=$(pwd)/src/worlds/project_world.world" &
sleep 5
echo "Launching turtlebot_world is finished"

echo "Launching amcl_demo..."
xterm -e "source devel/setup.bash; roslaunch turtlebot_gazebo amcl_demo.launch map_file:=$(pwd)/src/worlds/project_world.yaml" &
sleep 5
echo "Launching amcl_demo is finished"

echo "Launching view_navigation..."
xterm -e "source devel/setup.bash; roslaunch turtlebot_rviz_launchers view_navigation.launch" &
sleep 5
echo "Launching amcl_demo is finished"

echo "Running add_markers node..."
xterm -e "source devel/setup.bash; rosrun add_markers add_markers _param:=home_service_robot" &
sleep 5
echo "Running add_markers node is finished"

echo "Running pick_objects node..."
xterm -e "source devel/setup.bash; rosrun pick_objects pick_objects _param:=home_service_robot" &
sleep 2
echo "Running pick_objects node is finished"


