#include <stdio.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <string>
#include <cmath>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
extern GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common/text2D.hpp>

#include "setup_openGL.hpp"




// Print error message
void print_error(const char* err){
	fprintf(stderr,err);
	getchar();
}


int init_openGL_window(){
	if(!glfwInit()){
		print_error("Could not init OpenGL window\n");
		return FAIL;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//window = glfwCreateWindow(WIDTH,HEIGHT, "My OpenGL simulation ", glfwGetPrimaryMonitor() , NULL);
	window = glfwCreateWindow(WIDTH,HEIGHT, "Lorentz force simulator", NULL , NULL);


	if(window == NULL ){
		print_error("Failed to init Window\n");
		return FAIL;
	}
	glfwMakeContextCurrent(window);

	// Init GLEW to specify Targets attributes
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		print_error("Failed to init Glew\n");
		glfwTerminate();
		return FAIL;
	}

	// Just to make sure the programm can be interrupted by pressing Esc
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide mouse
	glfwPollEvents();
    glfwSetCursorPos(window, WIDTH/2, HEIGHT/2);	
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 
	glEnable(GL_CULL_FACE);

	return SUCCESS;

}

int create_bind_VAO(GLuint &vertex_array_ID){	
	glGenVertexArrays(1, &vertex_array_ID);
	glBindVertexArray(vertex_array_ID);
	return SUCCESS;
}
/**
template <typename T> int create_bind_VBO(GLuint &vertex_buffer_ID, std::vector<T> &buffer){
	glGenBuffers(1,&vertex_buffer_ID);
	glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer_ID);
	glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(T), &buffer[0], GL_STATIC_DRAW);
	return SUCCESS;
}
**/
double amplitude_of_vector(glm::vec3 v){
	return sqrt(pow(v.x,2) + pow(v.y,2) + pow(v.z,2));
}



int get_shader_code_and_compile(GLuint &shaderID, const char * path){
	// Read the Vertex Shader code from the file
	std::string shader_code;
	std::ifstream shader_stream(path, std::ios::in);
	if(shader_stream.is_open()){
		std::stringstream sstr;
		sstr << shader_stream.rdbuf();
		shader_code = sstr.str();
		shader_stream.close();
	}else{
		printf("Impossible to open %s !\n", path);
		getchar();
		return FAIL;
	}

	GLint Result = GL_FALSE;
	int info_log_len;

	// Compile Shader
	printf("Compiling shader : %s\n", path);
	char const * shader_ptr = shader_code.c_str();
	glShaderSource(shaderID, 1, &shader_ptr , NULL);
	glCompileShader(shaderID);

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &info_log_len);
	if ( info_log_len > 0 ){
		std::vector<char> error_message(info_log_len+1);
		glGetShaderInfoLog(shaderID, info_log_len, NULL, &error_message[0]);
		printf("%s\n", &error_message[0]);
		return FAIL;
	}

	return SUCCESS;
}


int load_shaders(const char *fragment_shader_path, const char* vertex_shader_path){
	
	GLuint vertex_shader_Id = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader_Id = glCreateShader(GL_FRAGMENT_SHADER);

	if(	get_shader_code_and_compile(vertex_shader_Id, vertex_shader_path) 	 == FAIL ||
	   	get_shader_code_and_compile(fragment_shader_Id,fragment_shader_path) == FAIL  )
	{
		print_error("Couldn't compile Shaders\n");
		return FAIL;
	}

	// Linking vertex shader and fragment shader to copmlete the pipeline
	GLuint program_Id = glCreateProgram();
	glAttachShader(program_Id, vertex_shader_Id);
	glAttachShader(program_Id, fragment_shader_Id);
	glLinkProgram(program_Id);

	GLint Result;
	int info_log_len;


	// Check the program
	glGetProgramiv(program_Id, GL_LINK_STATUS, &Result);
	glGetProgramiv(program_Id, GL_INFO_LOG_LENGTH, &info_log_len);
	if ( info_log_len > 0 ){
		std::vector<char> error_message(info_log_len+1);
		glGetProgramInfoLog(program_Id, info_log_len, NULL, &error_message[0]);
		printf("%s\n", &error_message[0]);
	}

	
	glDetachShader(program_Id, vertex_shader_Id);
	glDetachShader(program_Id, fragment_shader_Id);
	
	glDeleteShader(vertex_shader_Id);
	glDeleteShader(fragment_shader_Id);

	return program_Id;
	
}


text_render::text_render(unsigned int shader_ID, std::string working_dir){

	initText2D((working_dir+"/textures_n_objects/Holstein.DDS").c_str()
			   ,shader_ID);
}

void text_render::update_metrics_to_screen(std::vector<glm::vec3> metrics){
	/** metrics:
			
			1:B_field:
			2:F_Lorentz:
			3:Velocity:
	**/
	char text[256];
	int index = 1;
	for(std::vector<glm::vec3>::iterator it  = metrics.begin(); 
											   it != metrics.end(); 
											   it++)
	{
		char output[256];
		char line[256];
		char *temp;
		switch(index){
			case 1:
				temp = "(Green) B = ";
				break;
			case 2:
				temp = "(Red)   E = ";
				break;
			case 3:
				temp = "(White) F = ";
				break;
			case 4:
				temp = "(Blue)  u = ";
				break;
		}
		
		strcpy(line,temp);		
		sprintf(output,"[%.3f %.3f %.3f] ", it->x, it->y, it->z );
		strcat(line, output);
		//strcat(text,line);

		printText2D(line,1.30872, 13.119+index*20, 20);

		index++;
	}
	
}



