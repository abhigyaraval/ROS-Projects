#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
		ROS_INFO_STREAM("Robot attemping to follow the white ball!");

		ball_chaser::DriveToTarget srv;
		srv.request.linear_x = lin_x;
		srv.request.angular_z = ang_z;

		// Call the drive_bot service and pass the requested velocitires
		if (!client.call(srv))
			ROS_ERROR("Failed to call service drive_bot");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
		bool ball_detected = false;
    	int white_pixel = 252;
		int first_idx = 0;

		// Loop through the image
		for (int i = 0; i < img.height*img.step; i=i+3)
		{
				if (img.data[i] >= white_pixel &&
							img.data[i+1] >= white_pixel &&
								img.data[i+2] >= white_pixel)
								{
									ball_detected = true;
									first_idx = i;
									break;
								}
		}

		if (!ball_detected){
			drive_robot(0.0, 0.0);
		}

		else {
			int location = int(first_idx/3) % img.width;
			int bound1 = img.width/3;
			int bound2 = img.width/3*2;
			ROS_INFO("first_idx: %1.2f, bound1: %1.2f, location: %1.2f, bound2: %1.2f\n", (float)first_idx, (float)bound1, (float)location, (float)bound2);
			if (location > bound1 && location < bound2){ // move forward
				ROS_INFO("Ball in the CENTER. Moving forward now!");
				drive_robot(0.5, 0.0);
			}
			else if (location < bound1){ // ball in left side of frame
				ROS_INFO("Ball in LEFT side. Moving left now!");
				drive_robot(0.2, 0.8);
			}
			else if (location > bound2){ // ball in right side of frame
				ROS_INFO("Ball in RIGHT side. Moving right now!");
				drive_robot(0.2, -0.8);
			}
		}


    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
