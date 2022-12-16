#include <stdio.h>

#include <iostream>
#include <vector>

#include <cuda.h>
#include <cuda_runtime.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <math.h>
/**
 * Φ = ∯E•dS 
 * Φ ≈ Σ{E•n}
 */


__global__ void matrix_multiplication( float *flux, float *vertices, float *normals, float *position, uint size, float E_field_factor){
    int row_index = blockIdx.x*blockDim.x + threadIdx.x;
    float sum = 0;
    uint coordinates = 3; /** R^3 = {x,y,z}*/
    const int ray = 1;
    float tmp_flux[768];

    if(row_index < size){
        tmp_flux[threadIdx.x] = 0;
        
        float x = vertices[row_index*coordinates]     ;//+ position[0];
        float y = vertices[row_index*coordinates + 1] ;//+ position[1];
        float z = vertices[row_index*coordinates + 2] ;//+ position[2];
        float len = x*x + z*z;
        if(len + y*y == ray && len != 0){
        
            float E_field[3] = {E_field_factor*x/(len),
                                0,
                                E_field_factor*z/(len)}; 

            for(uint it = 0; it < 3; it ++)
                tmp_flux[threadIdx.x] = E_field[it] * normals[row_index*coordinates + it] + tmp_flux[threadIdx.x];
        }
        
    }
    __syncthreads();
    if(!threadIdx.x){
        flux[blockIdx.x] = 0;
        float limit = (blockIdx.x < 3) ?  1.0f : 0.75f;
        
        for(uint it = 0; it++ ; it < (uint)(blockDim.x*limit) ){
            flux[blockIdx.x] += tmp_flux[it];            
        }
    }

}


void kernel_caller(float E_field_factor,
                   std::vector<glm::vec3> normal_vectors,
                   std::vector<glm::vec3> vertices,
                   glm::vec3 position_of_sphere)
{
    uint sz = normal_vectors.size();

    int num_of_threads;      
    int num_of_blocks;    

    cudaOccupancyMaxPotentialBlockSize(&num_of_blocks, &num_of_threads, matrix_multiplication, 0, sz); 

    float *host_normals  = &(normal_vectors.at(0).x);
    float *host_vertices = &(vertices.at(0).x);
    float *host_position = &(position_of_sphere.x);
    float *host_flux = (float*)malloc(sizeof(float)*num_of_blocks);

    float *device_normals ; cudaMalloc((void **)&device_normals,  sz);
    float *device_vertices; cudaMalloc((void **)&device_vertices, sz);
    float *device_position; cudaMalloc((void **)&device_position, 3);
    float *device_flux    ; cudaMalloc((void **)&device_flux, num_of_blocks);



    cudaMemcpy(device_normals , host_normals , sz, cudaMemcpyHostToDevice); 
    cudaMemcpy(device_vertices, host_vertices, sz, cudaMemcpyHostToDevice);
    cudaMemcpy(device_position, host_position, 3/*{x,y,z}*/ , cudaMemcpyHostToDevice);


 
    //__global__ void matrix_multiplication( float *flux, float *vertices, float *normals, float *position, uint size, float E_field_factor){
    matrix_multiplication<<<num_of_blocks,num_of_threads>>>(
                                                          device_flux,
                                                          device_vertices,
                                                          device_normals,
                                                          device_position,
                                                          sz,
                                                          E_field_factor
                                                         );
    cudaDeviceSynchronize();
    cudaMemcpy(device_flux, host_flux, num_of_blocks, cudaMemcpyDeviceToHost);
    float d_flux = 0.0f;
    for(int i=0;i<num_of_blocks;i++)
        d_flux = host_flux[i] + d_flux;
    std::cout << "GPU: " << d_flux << std::endl;


    
    glm::vec3 E;
    float flux = 0.0f;
    const int ray = 1;
    for(int i=0;i<sz;i++){
        
        float x = vertices.at(i).x;
        float y = vertices.at(i).y;
        float z = vertices.at(i).z;

        float len = x*x + z*z;
        if(len + y*y == ray && len != 0){ 
            
            E.x = E_field_factor*x/len;
            E.z = E_field_factor*z/len;
            flux =  E.z * normal_vectors.at(i).z + E.x * normal_vectors.at(i).x + flux ;
        }
    }

    std::cout << "CPU : " << flux  << std::endl;
    
    

}