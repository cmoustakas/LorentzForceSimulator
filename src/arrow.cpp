#include <stdio.h>

#include <vector>
#include <string>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
extern GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common/objloader.hpp>

#include "setup_openGL.hpp"

#include "arrow.hpp"

//#include "setup_openGL.hpp"

#define SUCCESS 1
#define FAIL    0
#define WIDTH   640
#define HEIGHT  480
#define LAYOUT0 0
#define LAYOUT1 1

arrow::arrow(const char* path_of_obj, GLuint program_Id, glm::vec3 color){
    m_projection_matrix =  glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

    //Bind VAO
    GLuint vao_arrow_ID;
	create_bind_VAO(vao_arrow_ID);

    m_program_Id = program_Id;

	m_vec_coords = glm::vec3(0.0f,1.0f,0.0f);

    //Link my matrix_Id with the uniform model, model view projection
	m_matrix_Id    = glGetUniformLocation(m_program_Id, "model_mat");
	m_view_Id 	   = glGetUniformLocation(m_program_Id,"view_mat");
	m_project_Id   = glGetUniformLocation(m_program_Id,"projection_mat");
	m_rotation_Id  = glGetUniformLocation(m_program_Id,"rotation_mat");
	m_translate_Id = glGetUniformLocation(m_program_Id,"translation_mat");
    m_scale_Id     = glGetUniformLocation(m_program_Id,"scale_mat");
    
    //Load my arrow coords
	std::vector<glm::vec3> vertices_arrow;
	std::vector<glm::vec2> uvs_arrow;
	std::vector<glm::vec3> normals_arrow; 
	
    bool res = loadOBJ(path_of_obj,vertices_arrow, uvs_arrow, normals_arrow);
    size_of_vertices = vertices_arrow.size();

    //Load my arrow color
    
    glm::vec3 m_color_of_array = color;
    std::vector<glm::vec3> vertices_arrow_color(size_of_vertices, m_color_of_array);

    //Bind VBO
	if( create_bind_VBO(m_vertex_buffer_arrow,vertices_arrow) == FAIL ||
        create_bind_VBO(m_vertex_buffer_color,vertices_arrow_color) == FAIL )
        print_error("Cant Init arrow object \n");
    
	

}

void arrow::update_scale(glm::vec3 scale){
    m_scale_matrix  = glm::scale(glm::mat4(1.0f),scale);
}

void arrow::rotate_arrow(glm::vec3 reference_vec){
    glm::vec3 axis = glm::cross(m_vec_coords,reference_vec);
	float angle = acos(	( (m_vec_coords.x * reference_vec.x)  +  (m_vec_coords.y * reference_vec.y) + (m_vec_coords.z * reference_vec.z) ) /
						(glm::length(m_vec_coords) * glm::length(reference_vec))
					   );

	//m_vec_coords = reference_vec;
	m_rotation_matrix = glm::rotate(glm::mat4(1.0f),angle,axis);
}

void arrow::update_position_arrow(glm::vec3 new_position){
    m_translate_matrix = glm::translate(glm::mat4(1.0f),new_position );
}

void arrow::render_arrow(glm::mat4 view_mat){
	
    glUseProgram(m_program_Id);
	
	m_view_matrix = view_mat;

     // Send my matrices to corresponding the Uniform of GPU 
	glUniformMatrix4fv(m_matrix_Id, 1, GL_FALSE, &m_model_matrix[0][0]);
	glUniformMatrix4fv(m_project_Id, 1, GL_FALSE, &m_projection_matrix[0][0]);
	glUniformMatrix4fv(m_view_Id, 1, GL_FALSE, &m_view_matrix[0][0]);
	glUniformMatrix4fv(m_rotation_Id, 1, GL_FALSE, &m_rotation_matrix[0][0]);
	glUniformMatrix4fv(m_translate_Id, 1, GL_FALSE, &m_translate_matrix[0][0]);
   	glUniformMatrix4fv(m_scale_Id, 1, GL_FALSE, &m_scale_matrix[0][0]);
    
    glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_arrow);
	glVertexAttribPointer(LAYOUT0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_color);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    
	glDrawArrays(GL_TRIANGLES, 0, size_of_vertices); // 12*3 indices starting at 0 -> 12 triangles

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

		// Swap buffers
	//glfwSwapBuffers(window);

}
