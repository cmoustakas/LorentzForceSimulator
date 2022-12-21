// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstring>

/**
#include <cuda.h>
#include <src/flux_calculation_kernel_CUDA/flux_kernel.cuh>
**/

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;
bool reset_sig = false;
bool pause_sig = false;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Include imgui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"


#include <common/texture.hpp>
#include <common/objloader.hpp>
#include <common/text2D.hpp>


#include "setup_openGL.hpp"
#include "sphere.hpp"
#include "camera_setup.hpp"
#include "arrow.hpp"

//using namespace glm;

#define SUCCESS 1
#define FAIL    0
#define WIDTH   640
#define HEIGHT  480
#define LAYOUT0 0
#define LAYOUT1 1
#define PI 3.14
#define E_O 2
#define AC 1
#define DC 0


int main( void )
{
	// First of all take the filepath that we interested in:
	std::filesystem::path cwd = std::filesystem::current_path();
	cwd.replace_filename("src");
	
    // First and foremost create a sphere
    sphere *sphere_obj = new sphere();

	// Init window
	init_openGL_window();
	
	//Create VAO
	GLuint vertex_array_ID;
	create_bind_VAO(vertex_array_ID);
	
	//Create my pipeline Links from Shaders
	std::string fragment_shader_path = "/shaders/TextureFragmentShader.glsl";
	std::string vertex_shader_path = "/shaders/TransformVertexShader.glsl";
	GLuint program_Id = load_shaders(
    								(cwd.string() + fragment_shader_path).c_str(),
    								(cwd.string() + vertex_shader_path).c_str()  );
	

    //Link my matrix_Id with the uniform model, model view projection
	GLuint matrix_Id    = glGetUniformLocation(program_Id, "model_mat");
	GLuint view_Id 		= glGetUniformLocation(program_Id,"view_mat");
	GLuint projec_Id    = glGetUniformLocation(program_Id,"projection_mat");
	GLuint rotation_Id  = glGetUniformLocation(program_Id,"rotation_mat");
	GLuint translate_Id = glGetUniformLocation(program_Id,"translation_mat");
	
	std::string fragment_shader_path_arr = "/shaders/ArrowFragmentShader.glsl";
	std::string vertex_shader_path_arr   = "/shaders/ArrowVertexShader.glsl";

    //Create arrow objects
    GLuint arrow_shader_Id = load_shaders(
    									  (cwd.string() + fragment_shader_path_arr).c_str(),
    									  (cwd.string() + vertex_shader_path_arr).c_str()  );


	std::string fragment_shader_path_txt = "/shaders/TextFragmentShader.glsl";
	std::string vertex_shader_path_txt   = "/shaders/TextVertexShader.glsl";

    //Create a Text Renderer;
    text_render *text = new text_render(
        load_shaders((cwd.string() + fragment_shader_path_txt).c_str(),
                     (cwd.string() + vertex_shader_path_txt).c_str()),
		(cwd.remove_filename()).string()                                
    );

	
	//Create texture_wire and link it to the uniform of the fragment shader
	
	GLuint texture_wire = loadDDS(
    (cwd.string() + "/textures_n_objects/copper.dds").c_str());
	GLuint texture_sphere = loadDDS(
    (cwd.string() + "/textures_n_objects/sphere.dds").c_str());

	GLuint texture_ID  = glGetUniformLocation(program_Id, "myTextureSampler");
	
	//Load my wire
	std::vector<glm::vec3> vertices_wire;
	std::vector<glm::vec2> uvs_wire;
	std::vector<glm::vec3> normals_wire; 
	
    bool res = loadOBJ((cwd.string() + "/textures_n_objects/wire.obj").c_str(),
                        vertices_wire, uvs_wire, normals_wire);

	
	std::vector<glm::vec3> vertices_sphere;
	std::vector<glm::vec2> uvs_sphere;
	std::vector<glm::vec3> normals_sphere;
	res = loadOBJ((cwd.string() + "/textures_n_objects/mysphere.obj").c_str(),
                   vertices_sphere,uvs_sphere,normals_sphere);


                   
	//Bind Buffers 
	GLuint vertex_buffer_wire;
	GLuint vertex_buffer_uvs;
	GLuint vertex_buffer_sphere;
	GLuint vertex_buffer_uvs_sphere;

	if( create_bind_VBO(vertex_buffer_wire,vertices_wire) == FAIL ||
		create_bind_VBO(vertex_buffer_uvs,uvs_wire) == FAIL ||
		create_bind_VBO(vertex_buffer_sphere,vertices_sphere) == FAIL ||
		create_bind_VBO(vertex_buffer_uvs_sphere,uvs_sphere) == FAIL
	) 
		return -1;
	
	

    //Create a camera;    
    camera *camera_obj = new camera();
    const glm::mat4	ProjectioImGui_ImplGlfw_InitnMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 3000.0f);
    const glm::mat4 ViewMatrix = camera_obj->get_camera_model();

    std::string path_obj_arrow = "/textures_n_objects/current_arrow.obj";
	path_obj_arrow = cwd.string() + path_obj_arrow;
    arrow *I_arrow = new arrow(path_obj_arrow.c_str(),arrow_shader_Id,glm::vec3(1.0f,1.0f,0.2f));
    arrow *B_arrow = new arrow(path_obj_arrow.c_str(),arrow_shader_Id,glm::vec3(0.0f,1.0f,0.0f));
    arrow *E_arrow = new arrow(path_obj_arrow.c_str(),arrow_shader_Id,glm::vec3(1.0f,0.0f,0.0f));
    arrow *u_arrow = new arrow(path_obj_arrow.c_str(),arrow_shader_Id,glm::vec3(0.0f,1.0f,1.0f));
    arrow *F_arrow = new arrow(path_obj_arrow.c_str(),arrow_shader_Id,glm::vec3(1.0f,1.0f,1.0f));


    //Set initial states for my arrows
    I_arrow->update_position_arrow(glm::vec3(10.0f,0.0f,10.0f) );
    //I_arrow->rotate_arrow(glm::vec3(0.3f,0.7f,0.0f));
    B_arrow->update_position_arrow(sphere_obj->get_position());
    E_arrow->update_position_arrow(sphere_obj->get_position());
    u_arrow->update_position_arrow(sphere_obj->get_position());
    F_arrow->update_position_arrow(sphere_obj->get_position());

    // Okay lets fire up my timer!
    float         dt = 0.01;
    float  time_line = 0.0f;
    float time_quant = 0.01f;
	
	float I_current = 0;
	unsigned short int current_type = DC; // BY default DC 
	float frequency = 2.0f; //	rad/sec
	float amplitude = 2.0f; //	Ampere

	// Init GUI
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window,true);
	ImGui_ImplOpenGL3_Init("#version 330");
	
    bool show_GUI = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	
	do{



		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

		

        // 3. Show my GUI
        if (show_GUI)
        {
            ImGui::Begin("Simulator Control Panel", &show_GUI);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			
			if (ImGui::Button("Pause/Play"))
				pause_sig = !pause_sig;
		    
			if (ImGui::Button("DC --"))
                current_type = DC;
			
			ImGui::SameLine();

			if(ImGui::Button("AC  ~"))
				current_type = AC;
			
			
			ImGui::SliderFloat("Amplitude", &amplitude, -5.0f, 5.0f);
			if(current_type == AC)
				ImGui::SliderFloat("Frequency", &frequency,1.0f, 100.0f);
			
            ImGui::End();
        }

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Use our shader
		glUseProgram(program_Id);		
        camera_obj->poll_input_Events(dt);
		
        if(reset_sig){
            sphere_obj->reset();
            time_line = 0.0f;
            reset_sig = false;
        }


		glm::mat4 translate_mat_wire = glm::mat4(1.0);
		glm::mat4 rotation_mat_wire  = glm::mat4(1.0);
		glm::mat4 ModelMatrix_wire   = glm::mat4(1.0);
        glm::mat4 ProjectionMatrix 	 = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
        glm::mat4 ViewMatrix         = camera_obj->get_camera_model();

        // Send my matrices to the corresponding Uniform of GPU 
		glUniformMatrix4fv(matrix_Id   , 1, GL_FALSE, &ModelMatrix_wire[0][0]);
		glUniformMatrix4fv(projec_Id   , 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniformMatrix4fv(view_Id	   , 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(rotation_Id , 1, GL_FALSE, &rotation_mat_wire[0][0]);
		glUniformMatrix4fv(translate_Id, 1, GL_FALSE, &translate_mat_wire[0][0]);


        //Bind Texture of wire
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_wire);
		glUniform1i(texture_ID, 0);

        //Send the vertices of the wire 
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_wire);
		glVertexAttribPointer(LAYOUT0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

		// Send the UVs of the wire 
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_uvs);
		glVertexAttribPointer(LAYOUT1,2,GL_FLOAT,GL_FALSE,0,(void*)0);		

        // Render the object 
		glDrawArrays(GL_TRIANGLES, 0, vertices_wire.size() );


		/** 
        SPHERE
        */

		// Bind the texture to the sphere
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_sphere);		
		glUniform1i(texture_ID, 0);

		// If pause signal is not rised make the calculations
		if(!pause_sig){

			if(current_type == AC)
				I_current = amplitude*sin(frequency*time_line);
			else if(current_type == DC)
				I_current = amplitude;
		
			sphere_obj->calculate_B(I_current, dt);
			time_line = time_line + time_quant;
		}

		glm::mat4 rotation_matrix = glm::mat4(1.0f);
		glm::mat4 translate_matrix = glm::translate(glm::mat4(1.0f),
                                                   sphere_obj->get_position() );

        glm::mat4 model_matrix_sphere = glm::mat4(1.0f);

		glUniformMatrix4fv(matrix_Id,    1, GL_FALSE, &model_matrix_sphere[0][0]);
		glUniformMatrix4fv(projec_Id,    1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniformMatrix4fv(view_Id,      1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(rotation_Id,  1, GL_FALSE, &rotation_matrix[0][0]);
		glUniformMatrix4fv(translate_Id, 1, GL_FALSE, &translate_matrix[0][0]);
        
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer_sphere);
		glVertexAttribPointer(LAYOUT0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_uvs_sphere);
		glVertexAttribPointer(LAYOUT1,2,GL_FLOAT,GL_FALSE,0,(void*)0);
        
        //Render the sphere 
		glDrawArrays(GL_TRIANGLES, 0, vertices_sphere.size() );

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

        
        //Render my text
        std::vector<glm::vec3> push_vector {sphere_obj->get_B_field(),
                                            sphere_obj->get_E_field(), 
                                            sphere_obj->get_Force(),
                                            sphere_obj->get_velocity()};


        text->update_metrics_to_screen(push_vector);

        //Current arrow:
        I_arrow->update_scale(glm::vec3(0.5f, I_current , 0.5f));
        I_arrow->render_arrow(camera_obj->get_camera_model());

        //B field arrow:
        B_arrow->update_scale(glm::vec3(glm::length(push_vector.at(0)), 0.5f ,0.5f ));
        B_arrow->update_position_arrow(sphere_obj->get_position());
        B_arrow->rotate_arrow(push_vector.at(0));
        B_arrow->render_arrow(camera_obj->get_camera_model());

        //E field arrow
		//E_arrow->update_scale(glm::vec3(0.5f, 0.5f ,glm::length(push_vector.at(1)) ));
        E_arrow->update_position_arrow(sphere_obj->get_position());
        E_arrow->rotate_arrow(push_vector.at(1));
        E_arrow->render_arrow(camera_obj->get_camera_model());

        //F field arrow
		F_arrow->update_scale(glm::vec3(0.5f, 0.5f ,0.5f ));
        F_arrow->update_position_arrow(sphere_obj->get_position());
        F_arrow->rotate_arrow(push_vector.at(2));
        F_arrow->render_arrow(camera_obj->get_camera_model());

        //Velocity
		u_arrow->update_scale(glm::vec3(0.5f, 0.5f ,0.5f ));
        u_arrow->update_position_arrow(sphere_obj->get_position());
        u_arrow->rotate_arrow(push_vector.at(3));
        u_arrow->render_arrow(camera_obj->get_camera_model());


		
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );



	
	glDeleteBuffers(1, &vertex_buffer_wire);
	glDeleteBuffers(1, &vertex_buffer_uvs);
	glDeleteBuffers(1, &vertex_buffer_sphere);
	glDeleteBuffers(1, &vertex_buffer_uvs_sphere);
    delete text;

	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


	glDeleteProgram(program_Id);
	//glDeleteTextures(1, &texture_wire);
	glDeleteVertexArrays(1, &vertex_array_ID);
	glfwTerminate();


	return 0;
}