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

#include "application.hpp"

#include <glm/gtc/noise.hpp>
#include <vector>
#include <cstdint>

#include <iostream>

application::application(GLFWwindow* _win) :
		win(_win),
		counter_frag(GL_ATOMIC_COUNTER_BUFFER, GL_STATIC_READ),
		counter_node(GL_ATOMIC_COUNTER_BUFFER, GL_STATIC_READ),
		buffer_voxel_position(GL_TEXTURE_BUFFER, GL_STATIC_COPY),
		voxel_position(0),
		buffer_voxel_diffuse(GL_TEXTURE_BUFFER, GL_STATIC_COPY),
		voxel_diffuse(1),
		buffer_voxel_normal(GL_TEXTURE_BUFFER, GL_STATIC_COPY),
		voxel_normal(2),		
		buffer_octree_buffer(GL_TEXTURE_BUFFER, GL_STATIC_COPY),
		octree_buffer(3),
		buffer_octree_diffuse_r(GL_TEXTURE_BUFFER, GL_STATIC_COPY),
		octree_diffuse_r(4),
		buffer_octree_diffuse_g(GL_TEXTURE_BUFFER, GL_STATIC_COPY),
		octree_diffuse_g(5),
		buffer_octree_diffuse_b(GL_TEXTURE_BUFFER, GL_STATIC_COPY),
		octree_diffuse_b(6),
		buffer_octree_diffuse_a(GL_TEXTURE_BUFFER, GL_STATIC_COPY),
		octree_diffuse_a(7)
{
	glfwGetWindowSize(win, &w_width, &w_height);
	glfwGetFramebufferSize(win, &f_width, &f_height);

	voxelize_shader.setStage("shader/voxel.vert");
	voxelize_shader.setStage("shader/voxel.geom");
	voxelize_shader.setStage("shader/voxel.frag");
	voxelize_shader.link();

	flag_shader.setStage("shader/flag.comp");
	flag_shader.link();
	alloc_shader.setStage("shader/alloc.comp");
	alloc_shader.link();
	store_shader.setStage("shader/store.comp");
	store_shader.link();

	render_voxel_shader.setStage("shader/render_voxel.vert");
	render_voxel_shader.setStage("shader/render_voxel.geom");
	render_voxel_shader.setStage("shader/render_voxel.frag");
	render_voxel_shader.link();
	

	// octree_to_3D_shader.setStage("shader/oto3D.comp");
	// octree_to_3D_shader.link();

	// shader_ray.setStage("shader/ray.vert");
	// shader_ray.setStage("shader/ray.frag");
	// shader_ray.link();

	fontmap.file("assets/verasansmono.png");

	text_shader.setStage("shader/text.vert");
	text_shader.setStage("shader/text.geom");
	text_shader.setStage("shader/text.frag");
	text_shader.link();

	text_shader.load("textcolor", glm::vec3(1.f, 1.f, 1.f));
	text_shader.load("cellsize", glm::vec2(1.f/16, (300.f/384)/6));
	text_shader.load("celloffset", glm::vec2(.5f/256.f, .5f/256.f));
	text_shader.load("rendersize", glm::vec2(.75f * 16 / f_width, .75f * 33.33 / f_height));
	text_shader.load("renderorigin", glm::vec2(-.96f, .9f));

	text.vertices(nullptr, 256);
	text.addVertexAttribI(text_shader.attrib("character"), 1, sizeof(char), 0);

	/*Sphere<100, 100> sph(.75f);
	sphere.vertices(sph.vertices.data(), sph.vertices.size()*sizeof(vertex));
	sphere.indices(sph.indices.data(), sph.indices.size()*sizeof(face));
	sphere.addVertexAttrib(0, 3, sizeof(vertex), offsetof_ptr(vertex, pos));
	sphere.addVertexAttrib(1, 3, sizeof(vertex), offsetof_ptr(vertex, norm));*/
	// sphere.addVertexAttrib(shader.attrib("uv"), 2, sizeof(vertex), offsetof_ptr(vertex, text));

	//sphere = Geometry::fromFile("assets/cornell.g3dj");
	sphere = Geometry::fromFile("assets/sphere.g3dj");

	Plane<1> fullscreen_quad(2.f);
	quad.vertices(fullscreen_quad.vertices.data(), fullscreen_quad.vertices.size()*sizeof(vertex));
	quad.indices(fullscreen_quad.indices.data(), fullscreen_quad.indices.size()*sizeof(face));
	quad.addVertexAttrib(0, 3, sizeof(vertex), offsetof_ptr(vertex, pos));

	model = glm::mat4(1.f);
	// model = glm::translate(model, glm::vec3(64.f, 64.f, 64.f));
	view = glm::lookAt(glm::vec3(1.f, 1.f, -2.5f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
	projection = glm::perspective(tau/6.f, float(w_width)/float(w_height), .1f, 4500.f);


	float gray = 45.f / 255.f;
	glClearColor(gray, gray, gray, 1.f);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// tex3.alloc(512,512,512);
	// tex3.clear();
	counter_frag.data(NULL, sizeof(GLuint));
	counter_frag.clear(GL_R32UI, GL_RED, GL_UNSIGNED_INT);

	counter_node.data(NULL, sizeof(GLuint));
	counter_node.clear(GL_R32UI, GL_RED, GL_UNSIGNED_INT);

	buffer_voxel_position.bind();
	voxel_position.buffer(buffer_voxel_position.getName(), GL_R32UI);
	buffer_voxel_diffuse.bind();
	voxel_diffuse.buffer(buffer_voxel_diffuse.getName(), GL_RGBA8UI);
	buffer_voxel_normal.bind();
	voxel_normal.buffer(buffer_voxel_normal.getName(), GL_RGBA32F);	

	buffer_octree_buffer.bind();
	octree_buffer.buffer(buffer_octree_buffer.getName(), GL_R32UI);
	buffer_octree_diffuse_r.bind();
	octree_diffuse_r.buffer(buffer_octree_diffuse_r.getName(), GL_R32UI);
	buffer_octree_diffuse_g.bind();
	octree_diffuse_g.buffer(buffer_octree_diffuse_g.getName(), GL_R32UI);
	buffer_octree_diffuse_b.bind();
	octree_diffuse_b.buffer(buffer_octree_diffuse_b.getName(), GL_R32UI);
	buffer_octree_diffuse_a.bind();
	octree_diffuse_a.buffer(buffer_octree_diffuse_a.getName(), GL_R32UI);

	size_t total_node = 1;
	size_t n_part = 1;
	for(size_t i = 1; i <= octree_level; ++i)
	{
		n_part *= 8;
		total_node += n_part;
	}
	buffer_octree_buffer.data(NULL, total_node*sizeof(uint32_t));
	buffer_octree_diffuse_r.data(NULL, total_node*sizeof(uint32_t));
	buffer_octree_diffuse_g.data(NULL, total_node*sizeof(uint32_t));
	buffer_octree_diffuse_b.data(NULL, total_node*sizeof(uint32_t));
	buffer_octree_diffuse_a.data(NULL, total_node*sizeof(uint32_t));
	
	// voxelize_shader.bind();
	// voxelize_shader.load("pixel_diagonal", float(sqrt(2.f) * 1.f / 512));
	// std::cout <<"texture 3d " << tex3.getUnit() << std::endl;
}

void application::update(float dt)
{
	if(glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);
	model = glm::rotate(model, dt, glm::vec3(0.f, 1.f, 0.f));
	delta = dt;
}

void application::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
//	shader_color.bind();
//	shader_color.load("model", model);
//	shader_color.load("view", view);
//	shader_color.load("projection", projection);
//	cube.draw();
	
	// background.bind();
	{
		glViewport(0,0,voxel_dim,voxel_dim);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	}
	// count number of fragment
	{
		// tex3.clear();
		auto ortho = glm::ortho(-half_volume_size, half_volume_size, -half_volume_size, half_volume_size, (2.f*half_volume_size)-half_volume_size, (2.f*half_volume_size)+half_volume_size);
		auto vp_x = ortho * glm::lookAt(glm::vec3((2.f*half_volume_size), 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		auto vp_y = ortho * glm::lookAt(glm::vec3(0, (2.f*half_volume_size), 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
		auto vp_z = ortho * glm::lookAt(glm::vec3(0, 0, (2.f*half_volume_size)), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		voxelize_shader.bind();
		voxelize_shader.load("store", GL_FALSE);
		voxelize_shader.load("pixel_diagonal", float(sqrt(2.f) * 1.f / voxel_dim));
		voxelize_shader.load("voxel_dim", GLuint(voxel_dim));
		voxelize_shader.load("model", model);
		voxelize_shader.load("vp_x", vp_x);
		voxelize_shader.load("vp_y", vp_y);
		voxelize_shader.load("vp_z", vp_z);
		//voxelize_shader.load("view", glm::mat4(1.f));
		// voxelize_shader.load("projection", glm::ortho(-256.f, 256.f, -256.f, 256.f, -256.f, 256.f));
		// voxelize_shader.load("projection", glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f));
		//////////////////counter_frag.bind();
		counter_frag.bindBase();
		// tex3.bindImage(GL_R32UI, GL_WRITE_ONLY);
		sphere.draw();
	}
	// reserve num_frag space and store position, difusse and normal for every fragment of voxel
	glMemoryBarrier( GL_ATOMIC_COUNTER_BARRIER_BIT );
	GLuint num_frag = 0;
	{
		counter_frag.get(&num_frag, sizeof(GLuint));
		counter_frag.clear(GL_R32UI, GL_RED, GL_UNSIGNED_INT);

		if(max_frag < num_frag)
		{
			buffer_voxel_position.data(NULL, num_frag*sizeof(uint8_t)* 4);
			buffer_voxel_diffuse.data(NULL, num_frag*sizeof(uint8_t)* 4);
			buffer_voxel_normal.data(NULL, num_frag*sizeof(uint32_t)* 4);
			max_frag = num_frag;
		}

		buffer_voxel_position.clear(GL_R32UI, GL_RED, GL_UNSIGNED_INT);
		buffer_voxel_diffuse.clear(GL_RGBA8UI, GL_RGBA, GL_UNSIGNED_BYTE);
		buffer_voxel_normal.clear(GL_RGBA32F, GL_RGBA, GL_FLOAT);
		voxelize_shader.load("store", GL_TRUE);
		voxelize_shader.load("textured", GL_FALSE);
		voxelize_shader.load("bump", GL_FALSE);
		voxel_position.bindImage(GL_RGB10_A2UI);
		voxel_diffuse.bindImage(GL_RGBA8UI);
		voxel_normal.bindImage(GL_RGBA32F); // GL_RGB10_A2 scaling with 1.0/1023?
		//counter_frag.bindBase();
		sphere.draw();
	}
	// dispatch octree building
	{
		std::vector<GLuint> alloc_list;
		alloc_list.push_back(1);
		int data_width = 1024;
		int data_height = (num_frag+1023) / data_width;
		int group_dim_x = data_width/8;
		int group_dim_y = (data_height+7)/8;

		unsigned int node_offset = 0;
		unsigned int alloc_offset = 1;

		// clear every octree-buffer
		buffer_octree_buffer.clear(GL_R32UI, GL_RED, GL_UNSIGNED_INT);
		buffer_octree_diffuse_r.clear(GL_R32UI, GL_RED, GL_UNSIGNED_INT);
		buffer_octree_diffuse_g.clear(GL_R32UI, GL_RED, GL_UNSIGNED_INT);
		buffer_octree_diffuse_b.clear(GL_R32UI, GL_RED, GL_UNSIGNED_INT);
		buffer_octree_diffuse_a.clear(GL_R32UI, GL_RED, GL_UNSIGNED_INT);

		for(size_t i = 0; i < octree_level; ++i)
		{
			// flag nodes
			flag_shader.bind();
			flag_shader.load("voxel_frag_count", num_frag);
			flag_shader.load("level", GLuint(i));
			flag_shader.load("voxel_dim", GLuint(voxel_dim));
			voxel_position.bindImage(GL_RGB10_A2UI, GL_READ_ONLY);
			octree_buffer.bindImage(GL_R32UI);
			glDispatchCompute(group_dim_x, group_dim_y, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

			// alloc node tile
			int group_dim = (alloc_list[i]+63)/64;
			alloc_shader.bind();
			alloc_shader.load("start", GLuint(node_offset));
			alloc_shader.load("alloc_start", GLuint(alloc_offset));
			alloc_shader.load("num", GLuint(alloc_list[i]));
			octree_buffer.bindImage(GL_R32UI);
			counter_node.bindBase();
			glDispatchCompute(group_dim, 1, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

			// no need to init tile, we have already clean up everything with zeros


			// get the number of node tiles to allocate in the next level
			GLuint tile_allocated;
			counter_node.get(&tile_allocated, sizeof(GLuint));
			counter_node.clear(GL_R32UI, GL_RED, GL_UNSIGNED_INT);

			GLuint node_allocated = tile_allocated * 8;
			alloc_list.push_back(node_allocated);

			node_offset += alloc_list[i];
			alloc_offset += node_allocated;
		}
		// flag nonempty leaves
		flag_shader.bind();
		flag_shader.load("voxel_frag_count", num_frag);
		flag_shader.load("level", GLuint(octree_level));
		flag_shader.load("voxel_dim", GLuint(voxel_dim));
		voxel_position.bindImage(GL_RGB10_A2UI, GL_READ_ONLY);
		octree_buffer.bindImage(GL_R32UI);
		glDispatchCompute(group_dim_x, group_dim_y, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// store surfaceinformation (color, normal etc) into octree leaves
		store_shader.bind();
		store_shader.load("voxel_frag_count", num_frag);
		store_shader.load("level", GLuint(octree_level));
		store_shader.load("voxel_dim", GLuint(voxel_dim));
		voxel_position.bindImage(GL_RGB10_A2UI, GL_READ_ONLY);
		voxel_diffuse.bindImage(GL_RGBA8UI, GL_READ_ONLY);
		octree_buffer.bindImage(GL_R32UI, GL_READ_ONLY);
		octree_diffuse_r.bindImage(GL_R32UI);
		octree_diffuse_g.bindImage(GL_R32UI);
		octree_diffuse_b.bindImage(GL_R32UI);
		octree_diffuse_a.bindImage(GL_R32UI);
		glDispatchCompute(group_dim_x, group_dim_y, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// TODO mip-mapping
	}
	{
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glViewport(0,0, f_width, f_height);
	}
	{
		render_voxel_shader.bind();
		render_voxel_shader.load("model", glm::mat4(1.f));
		render_voxel_shader.load("view", view);
		render_voxel_shader.load("projection", projection);
		render_voxel_shader.load("voxel_dim", GLuint(voxel_dim));
		render_voxel_shader.load("level", GLuint(octree_level));
		render_voxel_shader.load("half_cube", 1.f/float(voxel_dim));
		octree_buffer.bindImage(GL_R32UI, GL_READ_ONLY);
		octree_diffuse_r.bindImage(GL_R32UI, GL_READ_ONLY);
		octree_diffuse_g.bindImage(GL_R32UI, GL_READ_ONLY);
		octree_diffuse_b.bindImage(GL_R32UI, GL_READ_ONLY);
		octree_diffuse_a.bindImage(GL_R32UI, GL_READ_ONLY);

		dummy.drawArray(voxel_dim * voxel_dim * voxel_dim);
	}
	// glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	// {
	// 	glViewport(0,0, f_width, f_height);
	// 	glEnable(GL_DEPTH_TEST);
	// 	glDepthMask(GL_TRUE);
	// 	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	// 	shader_ray.bind();
	// 	shader_ray.load("mvp_inverse", glm::inverse(projection * view /** model*/));
	// 	//shader_ray.load("MVP", glm::rotate(glm::mat4(1.f), tau/4.f, glm::vec3(1.f,0.f,0.f)));
	// 	tex3.bindImage(GL_R32UI, GL_READ_ONLY);
	// 	quad.draw();
	// }
	{
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		fontmap.bind();

		text_shader.bind();
		std::string test = "s: " + std::to_string(delta) + " fragments: " + std::to_string(num_frag) + " " + std::to_string(max_frag);
		text.vertices(test.data(), test.size());
		text.drawArray(test.size());
		glDisable(GL_BLEND);
	}
}

