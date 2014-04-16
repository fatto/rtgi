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

	fontmap.file("assets/verasansmono.png");
	shader_text.setStage("shader/text.vert");
	shader_text.setStage("shader/text.geom");
	shader_text.setStage("shader/text.frag");
	shader_text.link();
	shader_text.bind();
	shader_text.load("textcolor", glm::vec3(1.f, 1.f, 1.f));
	shader_text.load("cellsize", glm::vec2(1.f/16, (300.f/384)/6));
	shader_text.load("celloffset", glm::vec2(.5f/256.f, .5f/256.f));
	shader_text.load("rendersize", glm::vec2(.75f * 16 / f_width, .75f * 33.33 / f_height));
	shader_text.load("renderorigin", glm::vec2(-.96f, .9f));

	text.vertices(nullptr, 256);
	text.addVertexAttribI(shader_text.attrib("character"), 1, sizeof(char), 0);

	Sphere<100, 100> sph(256.f);
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
	// model = glm::translate(model, glm::vec3(64.f, 64.f, 64.f));
	view = glm::translate(glm::mat4(1.0f), glm::vec3(-128.0f, -128.0f, -950.f));
	projection = glm::perspective(tau/6.f, float(w_width)/float(w_height), .1f, 4500.f);


	float gray = 45.f / 255.f;
	glClearColor(gray, gray, gray, 1.f);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	tex3.alloc(512,512,512);
	tex3.clear();
	shader_voxelize.bind();
	shader_voxelize.load("pixel_diagonal", float(sqrt(2.f) * 1.f / 512));
	std::cout <<"texture 3d " << tex3.getUnit() << std::endl;
}

void application::update(float dt)
{
	if(glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);
	model = glm::rotate(model, dt, glm::vec3(1.f, 1.f, .0f));
	delta = dt;
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
		glViewport(0,0,512,512);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		tex3.clear();
		shader_voxelize.bind();
		shader_voxelize.load("model", model);
		shader_voxelize.load("view", glm::mat4());
		shader_voxelize.load("projection", glm::ortho(-256.f, 256.f, -256.f, 256.f, -256.f, 256.f));
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
		shader_ray.load("mvp_inverse", glm::inverse(projection * view /** model*/));
		//shader_ray.load("MVP", glm::rotate(glm::mat4(1.f), tau/4.f, glm::vec3(1.f,0.f,0.f)));
		tex3.bindImage(GL_R32UI, GL_READ_ONLY);
		quad.draw();
	}
	{
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		fontmap.bind();

		shader_text.bind();
		std::string test = "ms: " + std::to_string(delta);

		text.vertices(test.data(), test.size());
		text.drawArray(test.size());
		glDisable(GL_BLEND);
	}
}