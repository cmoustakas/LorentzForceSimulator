#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;



// Output data ; will be interpolated for each fragment.
out vec2 UV;

// Values that stay constant for the whole mesh.
uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;
// Default values to translation and rotation uniform
uniform mat4 translation_mat; 
uniform mat4 rotation_mat; 

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  projection_mat * view_mat * translation_mat * model_mat *  vec4(vertexPosition_modelspace,1);
	
	// UV of the vertex. No special space for this one.
	UV = vertexUV;
}

