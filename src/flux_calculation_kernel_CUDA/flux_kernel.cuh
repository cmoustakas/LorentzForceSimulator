#ifndef FLUX_KERNEL_CUH
#define FLUX_KERNEL_CUH

    __global__ void matrix_multiplication(float *flux, float *vertices, float *normals, float *position, uint size, float scale_factor);
    void kernel_caller(float E_field_factor,
                       std::vector<glm::vec3> normal_vectors,
                       std::vector<glm::vec3> vertices,
                       glm::vec3 position_of_sphere);

#endif