#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <visualization_msgs/Marker.h>
#include <tf/tf.h>
#include "geometry_msgs/Pose.h"
#include <math.h>

// Define a client for to send goal requests to the move_base server through a SimpleActionClient
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

MoveBaseClient* ac_ptr;
move_base_msgs::MoveBaseGoal goal;
geometry_msgs::Pose currentPose;
const float tolerance = 0.5;

geometry_msgs::Pose initGoalPose(float x, float y, float w)
{
  geometry_msgs::Pose resultPose;
  resultPose.position.x = x;
  resultPose.position.y = y;
  resultPose.position.z = 0;

  resultPose.orientation = tf::createQuaternionMsgFromYaw(w);
  return resultPose;
}

void sendGoal()
{
  ac_ptr->sendGoal(goal);
  // Wait an infinite time for the results
  ac_ptr->waitForResult();
  // Check if the robot reached its goal
  if(ac_ptr->getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
    ROS_INFO("Hooray, the base moved 1 meter forward");
  else
    ROS_INFO("The base failed to move forward 1 meter for some reason");
}

void MarkerMsgCallback(const visualization_msgs::Marker::ConstPtr& marker)
{
  if(fabs(marker->pose.position.x - goal.target_pose.pose.position.x) > tolerance
  || fabs(marker->pose.position.y - goal.target_pose.pose.position.y) > tolerance)
  {
    if(marker->action == visualization_msgs::Marker::DELETE)
    {
      ROS_INFO("Picking up...");
      ros::Duration(5).sleep();
    }
    else if(marker->action == visualization_msgs::Marker::ADD)
    {
      ROS_INFO("Dropping...");
    }
    goal.target_pose.pose = marker->pose;
    sendGoal();
  }
}

void simulatePickObjects()
{
  geometry_msgs::Pose pickup_pose = initGoalPose(-4, 4, 3.14159);
  geometry_msgs::Pose drop_pose = initGoalPose(4, 4, 0);

  goal.target_pose.pose = pickup_pose;
  sendGoal();
  ros::Duration(5).sleep();
  goal.target_pose.pose = drop_pose;
  sendGoal();
}

int main(int argc, char** argv)
{
  std::string param;
  // Initialize the simple_navigation_goals node
  ros::init(argc, argv, "pick_objects");
  ros::NodeHandle n("~");
  MoveBaseClient ac("move_base", true);
  // set up the frame parameters
  goal.target_pose.header.frame_id = "/map";
  goal.target_pose.header.stamp = ros::Time::now();
  
  n.getParam("param", param);
  while(!ac.waitForServer(ros::Duration(5.0)))
  {
    ROS_INFO("Waiting for the move_base action server to come up");
  }
  ac_ptr = &ac;
  ros::Subscriber marker_sub;
  if(param.compare("home_service_robot") == 0)
  {
    std::string topicName = ros::names::clean(n.resolveName("/visualization_marker"));
    ROS_INFO("Topic to subscribe: %s", topicName.c_str());
    marker_sub = n.subscribe(topicName, 20, MarkerMsgCallback);
  }
  else
  {
    simulatePickObjects();
  }
  ros::spin();
  return 0;
}