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

	shader.setStage("shader/voxel.vert");
	shader.setStage("shader/voxel.geom");
	shader.setStage("shader/voxel.frag");
	shader.link();


	Sphere<30, 30> sph(1.5f);
	sphere.vertices(sph.vertices.data(), sph.vertices.size()*sizeof(vertex));
	sphere.indices(sph.indices.data(), sph.indices.size()*sizeof(face));
	sphere.addVertexAttrib(shader.attrib("position"), 3, sizeof(vertex), offsetof_ptr(vertex, pos));
	std::cout << shader.attrib("position") << std::endl;
	std::cout << shader.attrib("normal") << std::endl;
	sphere.addVertexAttrib(shader.attrib("normal"), 3, sizeof(vertex), offsetof_ptr(vertex, norm));
	// sphere.addVertexAttrib(shader.attrib("uv"), 2, sizeof(vertex), offsetof_ptr(vertex, text));

	
	model = glm::mat4(1.f);
	view = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.0f, -4.f));
	projection = glm::perspective(tau/6.f, float(w_width)/float(w_height), .1f, 150.f);


	float gray = 45.f / 255.f;
	glClearColor(gray, gray, gray, 1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	Texture3 test_tex;
	test_tex.alloc(256,256,256);
	test_tex.clear();
	std::cout << test_tex.getUnit() << std::endl;
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
	shader.bind();
	shader.load("model", model);
	shader.load("view", view);
	shader.load("projection", projection);
//	terrain.draw();
	sphere.draw();
}