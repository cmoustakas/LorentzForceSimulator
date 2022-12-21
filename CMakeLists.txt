# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (Lorentz LANGUAGES   CXX C )
#project (Lorentz LANGUAGES  CUDA CXX C )

#enable_language(CUDA)
#include(CheckLanguage)
#check_language(CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


find_package(OpenGL REQUIRED)


if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()



# Compile external dependencies 
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)


if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)



include_directories(
	
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
	-w
)




# Executable
add_executable(LorentzForceSimulator 
	src/main.cpp
	src/shaders/TextureFragmentShader.glsl
	src/shaders/TransformVertexShader.glsl
	src/setup_openGL.cpp
	src/setup_openGL.hpp
	src/sphere.hpp
	src/camera_setup.hpp
	src/arrow.hpp
	src/arrow.cpp
	
	common/texture.cpp
	common/texture.hpp
	common/objloader.cpp
	common/objloader.hpp
	common/text2D.hpp
	common/text2D.cpp

	src/flux_calculation_kernel_CUDA/flux_kernel.cuh
	src/flux_calculation_kernel_CUDA/flux_kernel.cu
	
	src/imgui/imconfig.h
	src/imgui/imgui.cpp
	src/imgui/imgui.h
	src/imgui/imgui_demo.cpp
	src/imgui/imgui_draw.cpp
	src/imgui/imgui_impl_glfw.cpp
	src/imgui/imgui_impl_glfw.h
	src/imgui/imgui_internal.h
	src/imgui/imgui_tables.cpp
	src/imgui/imgui_widgets.cpp
	src/imgui/imstb_rectpack.h
	src/imgui/imstb_textedit.h
	src/imgui/imstb_truetype.h
	src/imgui/imgui_impl_opengl3.h
	src/imgui/imgui_impl_opengl3.cpp
	src/imgui/imgui_impl_opengl3_loader.h

)
target_link_libraries(LorentzForceSimulator
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(LorentzForceSimulator PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/LorentzForceSimulator/")


