#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include <math.h>
#include <tf/tf.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/Twist.h"
#include "nav_msgs/Odometry.h"


bool isPicked = false;
bool isDelivered = false;
const float tolerance = 0.5;
geometry_msgs::PoseWithCovarianceStamped robotPose;

ros::Publisher marker_pub;
visualization_msgs::Marker* marker_ptr;

bool comparePose(geometry_msgs::Pose pose1, geometry_msgs::Pose pose2)
{
  return (fabs(pose1.position.x - pose2.position.x) <= tolerance
      && fabs(pose1.position.y - pose2.position.y) <= tolerance);
}

void setMarkerPose(float x, float y, float w)
{
    marker_ptr->pose.position.x = x;
    marker_ptr->pose.position.y = y;
    marker_ptr->pose.position.z = 0.5;
    marker_ptr->pose.orientation = tf::createQuaternionMsgFromYaw(w);
}

bool StartHomeSvcRobotRoutine(ros::Subscriber* subscriber, visualization_msgs::Marker* marker)
{
  while (marker_pub.getNumSubscribers() < 2)
  {
    if (!ros::ok())
    {
      return false;
    }
    ROS_WARN_ONCE("Waiting for all subscribers to the marker");
    sleep(1);
  }
  ROS_INFO("All subscribers established. Publishing marker...");
  setMarkerPose(-4, 4, 3.1415);
  marker_pub.publish(*marker);
  
  return true;
}

void velocityCallback(const nav_msgs::Odometry::ConstPtr& odom)
{
  // ROS_INFO("robot velocity: x= %f ,y=%f", odom->twist.twist.linear.x, odom->twist.twist.linear.y);
  if(fabs(odom->twist.twist.linear.x) < 0.0001 && fabs(odom->twist.twist.linear.y) < 0.0001)
  {
    if(!isDelivered && comparePose(robotPose.pose.pose, marker_ptr->pose))
    {
      if(!isPicked)
      {
        ROS_INFO("In pickup position...");
        marker_ptr->action = visualization_msgs::Marker::DELETE;
        setMarkerPose(4, 4, robotPose.pose.pose.orientation.w);
        marker_pub.publish(*marker_ptr);
        isPicked = true;
      }
      else
      {
        ROS_INFO("In drop position...");
        marker_ptr->action = visualization_msgs::Marker::ADD;
        marker_pub.publish(*marker_ptr);
        isDelivered = true;
      }
    }
  }
}

void amclPoseCallback(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& amclPoseMsg)
{
  robotPose = *amclPoseMsg;
}

bool simulateMarkerPoses()
{
  setMarkerPose(-4, 4, 1);
  marker_ptr->action = visualization_msgs::Marker::ADD;
  while (marker_pub.getNumSubscribers() < 1)
  {
    if (!ros::ok())
    {
      return false;
    }
    ROS_WARN_ONCE("Waiting for all subscribers to the marker");
    sleep(1);
  }
  ROS_INFO("Subscriber established. Publishing marker...");
  marker_pub.publish(*marker_ptr);
  ros::Duration(5).sleep();
  marker_ptr->action = visualization_msgs::Marker::DELETE;
  marker_pub.publish(*marker_ptr);
  ros::Duration(5).sleep();
  marker_ptr->action = visualization_msgs::Marker::ADD;
  setMarkerPose(4, 4, 1);
  marker_pub.publish(*marker_ptr);
  return true;
}

int main( int argc, char** argv )
{
  std::string param;
  ros::init(argc, argv, "add_markers");
  ros::NodeHandle n("~");
  ros::Rate r(1);
  n.getParam("param", param);
  marker_pub = n.advertise<visualization_msgs::Marker>("/visualization_marker", 1);
  ros::Subscriber pose_subscriber;
  ros::Subscriber vel_subscriber;
  // Set our initial shape type to be a cube
  uint32_t shape = visualization_msgs::Marker::CUBE;
  visualization_msgs::Marker marker;
  // Set the marker type. Initially this is CUBE, and cycles between that and SPHERE, ARROW, and CYLINDER
  marker.type = shape;
  // Set the frame ID and timestamp.  See the TF tutorials for information on these.
  marker.header.frame_id = "/map";
  marker.header.stamp = ros::Time::now();
  // Any marker sent with the same namespace and id will overwrite the old one
  marker.ns = "add_markers";
  marker.id = 0;
  marker.lifetime = ros::Duration();
  // Set the scale of the marker -- 1x1x1 here means 1m on a side
  marker.scale.x = 0.15;
  marker.scale.y = 0.15;
  marker.scale.z = 0.15;
 
  // Set the color -- be sure to set alpha to something non-zero!
  marker.color.r = 1.0f;
  marker.color.g = 0.0f;
  marker.color.b = 0.0f;
  marker.color.a = 1.0;
  marker_ptr = &marker;
  bool ok = false;
  if(param.compare("home_service_robot") == 0)
  {
    // ROS_INFO("Starting home service robot routine...");
    pose_subscriber = n.subscribe("/amcl_pose", 100, amclPoseCallback);
    vel_subscriber = n.subscribe("/odom", 10, velocityCallback);
    ok = StartHomeSvcRobotRoutine(&pose_subscriber, &marker);
  }
  else
  {
    // ROS_INFO("Starting add markers routine...");
    ok = simulateMarkerPoses();
  }
  
  if(!ok)
  {
    ROS_INFO("Something went wrong on adding markers. Shutting down now...");
    ros::Duration(5).sleep();
    return 0;
  }
  ros::spin();
   return 0;
 }