// The MIT License (MIT)

// Copyright (c) 2014 Marco Fattorel

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef rtgi_application_hpp
#define rtgi_application_hpp

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "glwrap.hpp"
#include "geometry.hpp"
#include "texture.hpp"
#include "shader.hpp"

#include <array>
#include <cassert>

class application
{
	GLFWwindow* win;
	int w_width, w_height;
	int f_width, f_height;
	Geometry sphere;
	Geometry quad;
	Geometry text;

	//// Shader
	Shader text_shader;
	// rendering
	Shader render_shader;
	// svo construction
	Shader voxelize_shader;
	Shader node_flag_shader;
	Shader node_alloc_shader;
	Shader node_init_shader;
	Shader leaf_store_shader;
	Shader mipmap_node_shader;
	// deferred
	Shader pass_shader;
	Shader deferred_shader;
	// debug
	Shader octree_to_3D_shader;
	// Shader shader_ray;

	// voxel dimension
	int voxel_dim = 128;
	int octree_level = 7;
	unsigned int num_voxel_frag = 0;
	// voxel creation buffers
	// GLuint voxelTex = 0;   //3D texture
	// GLuint voxelPosTex = 0;  //texture for voxel fragment list (position)
	// GLuint voxelPosTbo = 0;  //texture buffer object for voxel fragment list (position)
	// GLuint voxelKdTex = 0;  //texture for voxel fragment list (diffuse)
	// GLuint voxelKdTbo = 0;  //texture buffer object for voxel fragment list (diffuse)
	// GLuint voxelNrmlTex = 0;  //texture for voxel fragment list (normal)
	// GLuint voxelNrmlTbo = 0;  //texture buffer object for voxel fragment list (normal)

	//// Texture
	Texture2 fontmap;


	glm::mat4 model, view, projection;
	float delta = 0.f;
public:
	application(GLFWwindow* _win);
	void update(float dt);
	void draw();
};

#endif
