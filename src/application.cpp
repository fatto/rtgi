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


#include <iostream>

application::application(GLFWwindow* _win) : win(_win)
{
	glfwGetWindowSize(win, &w_width, &w_height);
	glfwGetFramebufferSize(win, &f_width, &f_height);

	shader_voxelize.setStage("shader/voxel.vert");
	shader_voxelize.setStage("shader/voxel.geom");
	shader_voxelize.setStage("shader/voxel.frag");
	shader_voxelize.link();

	shader_ray.setStage("shader/ray.vert");
	shader_ray.setStage("shader/ray.frag");
	shader_ray.link();


	Sphere<10, 10> sph(32.f);
	sphere.vertices(sph.vertices.data(), sph.vertices.size()*sizeof(vertex));
	sphere.indices(sph.indices.data(), sph.indices.size()*sizeof(face));
	sphere.addVertexAttrib(0, 3, sizeof(vertex), offsetof_ptr(vertex, pos));
	sphere.addVertexAttrib(1, 3, sizeof(vertex), offsetof_ptr(vertex, norm));
	// sphere.addVertexAttrib(shader.attrib("uv"), 2, sizeof(vertex), offsetof_ptr(vertex, text));


	Plane<1> fullscreen_quad(2.f);
	quad.vertices(fullscreen_quad.vertices.data(), fullscreen_quad.vertices.size()*sizeof(vertex));
	quad.indices(fullscreen_quad.indices.data(), fullscreen_quad.indices.size()*sizeof(face));
	quad.addVertexAttrib(0, 3, sizeof(vertex), offsetof_ptr(vertex, pos));


	model = glm::mat4(1.f);
	view = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.0f, -4.f));
	projection = glm::perspective(tau/6.f, float(w_width)/float(w_height), .1f, 150.f);


	float gray = 45.f / 255.f;
	glClearColor(gray, gray, gray, 1.f);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);


	tex3.alloc(256,256,256);
	tex3.clear();
	std::cout <<"texture 3d " << tex3.getUnit() << std::endl;
}

void application::update(float dt)
{
	if(glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);
	model = glm::rotate(model, dt, glm::vec3(1.f, 1.f, .0f));
}

void application::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, f_width, f_height);
	
//	shader_color.bind();
//	shader_color.load("model", model);
//	shader_color.load("view", view);
//	shader_color.load("projection", projection);
//	cube.draw();
	
	// background.bind();
	{
		glViewport(0,0,256,256);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		tex3.clear();
		shader_voxelize.bind();
		shader_voxelize.load("model", model);
		shader_voxelize.load("view", view);
		shader_voxelize.load("projection", glm::ortho(-32.f, 32.f, -32.f, 32.f, -32.f, 32.f));
		tex3.bindImage(GL_R32UI, GL_WRITE_ONLY);
		sphere.draw();
	}
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	{
		glViewport(0,0, f_width, f_height);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		shader_ray.bind();
		//shader_ray.load("MVP", glm::rotate(glm::mat4(1.f), tau/4.f, glm::vec3(1.f,0.f,0.f)));
		tex3.bindImage(GL_R32UI, GL_READ_ONLY);
		quad.draw();
	}
}