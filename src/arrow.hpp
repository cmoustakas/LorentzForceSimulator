#ifndef ARROW_H
#define ARROW_H
    class arrow{
        glm::mat4 m_scale_matrix = glm::mat4(1.0f);
        glm::mat4 m_rotation_matrix = glm::mat4(1.0f); 
		glm::mat4 m_translate_matrix = glm::mat4(1.0f);

        glm::mat4 m_view_matrix ;
        glm::mat4 m_model_matrix;
        glm::mat4 m_projection_matrix;
        glm::vec3 m_color_of_array;
        
        // Shaders program
        GLuint m_program_Id;
        // Uniform Id
        GLuint m_matrix_Id;
        GLuint m_view_Id;
        GLuint m_project_Id;
        GLuint m_rotation_Id;
        GLuint m_translate_Id;
		GLuint m_scale_Id;
        // Vertex Buffers binders
        GLuint m_vertex_buffer_arrow;
	    GLuint m_vertex_buffer_color;
        glm::vec3 m_vec_coords;
        unsigned int size_of_vertices;
        
        public:
            arrow(const char* path_of_obj, GLuint program_Id, glm::vec3 color);
            void update_scale(glm::vec3 scale);
            void rotate_arrow(glm::vec3 reference_vec);
            void update_position_arrow(glm::vec3 new_position);
            void render_arrow(glm::mat4 view_mat);
    };
#endif