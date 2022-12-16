#ifndef CAMERA_SETUP_H
#define CAMERA_SETUP_H

#include <GLFW/glfw3.h>
extern GLFWwindow* window;

extern bool reset_sig;
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unistd.h>

#define MAX_LEN 200

class camera{
	glm::vec3 position;
	glm::vec3 up;  
	glm::vec3 direction; 
	glm::mat4 cameraModel;
	float speed = 20.0f;
	float angle ;
	public:
		camera();
		void poll_input_Events(float deltaTime);
		glm::vec3 get_position();
		glm::vec3 get_up_vector();
		glm::vec3 get_direction();
		glm::mat4 get_camera_model();
};

camera::camera(){
	
	position = glm::vec3(0.0f,0.0f,-80.0f);
	up = glm::vec3(0.0f,-1.0f,0.0f);
	direction = -glm::normalize(position);

	cameraModel = glm::lookAt(position, position+direction, up);
	angle = PI;
}

glm::vec3 camera::get_position(){    return position;}
glm::vec3 camera::get_up_vector(){   return up;}
glm::vec3 camera::get_direction(){   return direction;}
glm::mat4 camera::get_camera_model(){return cameraModel;} 

void camera::poll_input_Events(float deltaTime){
	
		if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
			position +=  direction * deltaTime * speed;
			if(glm::length(position) > MAX_LEN)
				position -=  direction * deltaTime * speed;
		}
		
		if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
			position -=  direction * deltaTime * speed;
			if(glm::length(position) > MAX_LEN)
				position +=  direction * deltaTime * speed;
		}
	
		// Strafe right
		if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
			angle = angle - 0.03f;
			
			position = glm::vec3(glm::length(position)*sin(angle),
								 0.0f,
								glm::length(position)*cos(angle));
			
			direction = - glm::normalize(position);

		}
		// Strafe left
		if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
			angle = angle + 0.03f;
			
			position = glm::vec3(glm::length(position)*sin(angle),
								 0.0f,
								glm::length(position)*cos(angle));
			
			direction = - glm::normalize(position);

		}

		if(glfwGetKey( window, GLFW_KEY_SPACE) == GLFW_PRESS )	
			reset_sig = true;
		
		
		

			
	cameraModel = glm::lookAt(position, position+direction, up);

}

#endif
