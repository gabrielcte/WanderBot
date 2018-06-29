/*
 * Stopper.cpp
 *
 *  Created on: Oct 27, 2016
 *      Author: viki
 *      Modificado por: Gabriel
 */

#include "Stopper.h"
#include "geometry_msgs/Twist.h"

Stopper::Stopper()
{
    keepMoving = true;

    // Advertise a new publisher for the robot's velocity command topic
    commandPub = node.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/teleop", 10);

    // Subscribe to the simulated robot's laser scan topic
    laserSub = node.subscribe("scan", 1, &Stopper::scanCallback, this);
}

// Send a velocity command
void Stopper::moveForward() {
    geometry_msgs::Twist msg; // The default constructor will set all commands to 0
    msg.linear.x = FORWARD_SPEED;
    commandPub.publish(msg);
}

// Send a Angular velocity command
void Stopper::turnRight() {
    geometry_msgs::Twist msg; 
    msg.angular.z = TURN_SPEED*-1;
    commandPub.publish(msg);
}
void Stopper::turnLeft() {
    geometry_msgs::Twist msg; 
    msg.angular.z = TURN_SPEED;
    commandPub.publish(msg);
}

// Process the incoming laser scan message
void Stopper::scanCallback(const sensor_msgs::LaserScan::ConstPtr& scan)
{

float L = 0.0;
float R = 0.0;

bool isObstacleInFront = false;

    // Find the closest range between the defined minimum and maximum angles
    int minIndex = ceil((MIN_SCAN_ANGLE - scan->angle_min) / scan->angle_increment);
    int maxIndex = floor((MAX_SCAN_ANGLE - scan->angle_min) / scan->angle_increment);

    for (int currIndex = minIndex + 1; currIndex <= maxIndex; currIndex++) {
        if (scan->ranges[currIndex] < MIN_DIST_FROM_OBSTACLE) {
        	isObstacleInFront = true;
            break;
        }
    }

    if (isObstacleInFront) {
        ROS_INFO("Looking a way !");
             for (int a=0; a<=1000000; a++ ){
                   turnRight();
                   if(R > scan->angle_increment){}
                   if(R < scan->angle_increment) {R = scan->angle_increment;} 
      }
   
          for (int b=0; b<=1000000 ; b++ ){
                   turnLeft();
       }
      
       
          for (int c=0; c<=1000000 ; c++ ){
                   turnLeft();
                   if(L > scan->angle_increment ){}
                   if(L < scan->angle_increment ) {L = scan->angle_increment;} 
      }

          for (int b=0; b<=1000000 ; b++ ){
                   turnRight();
       }
      
     if (L>R) {for (int c=0; c<=2000000 ; c++ ){
                   turnLeft();}moveForward();}
     if (L<R) {for (int c=0; c<=2000000 ; c++ ){
                   turnRight();}moveForward();}   


}  
} 

void Stopper::startMoving()
{
    ros::Rate rate(10);
    ROS_INFO("Start moving");

    // Keep spinning loop until user presses Ctrl+C or the robot got too close to an obstacle
    while (ros::ok() && keepMoving) {
        moveForward();
        ros::spinOnce(); // Need to call this function often to allow ROS to process incoming messages
        rate.sleep();
    }
}

