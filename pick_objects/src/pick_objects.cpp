#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/tf.h>

// Define a client for to send goal requests to the move_base server through a SimpleActionClient
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

void sendGoalPose(MoveBaseClient &ac, move_base_msgs::MoveBaseGoal &goal, float pose[3])
{
  goal.target_pose.pose.position.x = pose[0];
  goal.target_pose.pose.position.y = pose[1];
  goal.target_pose.pose.orientation = tf::createQuaternionMsgFromYaw(pose[2]);

  ROS_INFO("Sending position. x=%f\t,y=%f\t,w=%f",
  goal.target_pose.pose.position.x,
  goal.target_pose.pose.position.y,
  goal.target_pose.pose.orientation.w
  );
  ac.sendGoal(goal);

  // Wait an infinite time for the results
  ac.waitForResult();

  // Check if the robot reached its goal
  if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
    ROS_INFO("Hooray, the base moved 1 meter forward");
  else
    ROS_INFO("The base failed to move forward 1 meter for some reason");
}

int main(int argc, char** argv){
  // Initialize the simple_navigation_goals node
  ros::init(argc, argv, "simple_navigation_goals");

  //tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);

  // Wait 5 sec for move_base action server to come up
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
  }

  move_base_msgs::MoveBaseGoal goal;

  // set up the frame parameters
  goal.target_pose.header.frame_id = "map";
  goal.target_pose.header.stamp = ros::Time::now();

  float pickup_pos[3] = {-4.0, 4.0, 3.14159};
  float drop_pos[3] = {4.0, 4.0, 0.0};

  sendGoalPose(ac, goal, pickup_pos);
  ros::Duration(5).sleep();
  sendGoalPose(ac, goal, drop_pos);
  return 0;
}