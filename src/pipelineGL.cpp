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

#include "pipelineGL.hpp"

#include <glm/gtc/noise.hpp>
#include <vector>

template<GLuint rings, GLuint sectors>
struct Sphere
{
	std::array<vertex, rings * sectors> vertices;
	std::array<face, (rings-1) * sectors * 2> indices;
	
	Sphere() = delete;
	Sphere(GLfloat radius)
	{
		GLfloat const R = 1.f/float(rings-1);
		GLfloat const S = 1.f/float(sectors-1);
		
		auto v = vertices.begin();
		auto i = indices.begin();
		for(GLuint r = 0; r < rings; ++r)
		{
			for(GLuint s = 0; s < sectors; ++s)
			{
				float const x = cos(tau * s * S) * sin((tau/2.f) * r * R);
				float const y = cos((tau/2.f) * r * R);
				float const z = sin(tau * s * S) * sin((tau/2.f) * r * R);
				v->pos = {{ x * radius, y * radius, z * radius }};
				v->norm = {{ x, y, z }};
				v->text = {{ s * S, r * R }};
				++v;
			}
		}
		for(GLuint r = 0; r < rings-1; ++r)
		{
			for(GLuint s = 0; s < sectors; ++s)
			{
				GLuint iBase = r*rings;
				GLuint i00 = iBase + s;
				GLuint i01 = iBase + (s == sectors-1 ? 0 : s+1);
				
				i->triang = {{ i00, i01 + sectors, i00 + sectors }};
				i++;
				
				i->triang = {{ i00, i01, i01 + sectors }};
				i++;
			}
		}
	}
};

pipelineGL::pipelineGL(GLFWwindow* _win) : win(_win), background("assets/background.png")
{
	glfwGetWindowSize(win, &w_width, &w_height);
	glfwGetFramebufferSize(win, &f_width, &f_height);

	shader_color.setVertex("shader/color.vert");
	shader_color.setFragment("shader/color.frag");
	shader_color.link();
	
	shader_uv.setVertex("shader/uv.vert");
	shader_uv.setFragment("shader/uv.frag");
	shader_uv.link();
	
	std::vector<vertex> cube_vertices {
		{{-.5f, -.5f, .5f}, {.0f, .0f, 1.f}},
		{{.5f, -.5f, .5f}, {1.f, .0f, 1.f}},
		{{.5f, .5f, .5f}, {1.f, 1.f, 1.f}},
		{{-.5f, .5f, .5f}, {.0f, 1.f, 1.f}},
		{{-.5f, -.5f, -.5f}, {.0f, .0f, .0f}},
		{{.5f, -.5f, -.5f}, {1.f, .0f, .0f}},
		{{.5f, .5f, -.5f}, {1.f, 1.f, .0f}},
		{{-.5f, .5f, -.5f}, {.0f, 1.f, .0f}}};
	std::vector<face> cube_indices {
		{0, 1, 2}, {0, 2, 3},
		{1, 5, 6}, {1, 6, 2},
		{5, 4, 7}, {5, 7, 6},
		{4, 0, 3}, {4, 3, 7},
		{2, 6, 7}, {2, 7, 3},
		{0, 4, 5}, {0, 5, 1}};
	
	std::vector<vertex> terrain_vertices;
	std::vector<face> terrain_indices;
	
	for(size_t i = 0; i < 10; ++i)
	{
		for(size_t j = 0; j < 10; ++j)
		{
			vertex v;
			v.pos[0] = (float(i)-5.f)/2.f;
			v.pos[1] = 0.f;
			v.pos[2] = (float(j)-5.f)/2.f;
			v.norm = {0.f, 1.f, 0.f};
			v.text = { (v.pos[0]+2.5f)/4.5f, (v.pos[2]+2.5f)/4.5f };
			terrain_vertices.push_back(v);
		}
	}
	for(GLuint i = 0; i < 9; ++i)
	{
		for(GLuint j = 0; j < 9; ++j)
		{
			terrain_indices.push_back({i+(10*j), i+(10*j)+1, i+(10*j)+10});
			terrain_indices.push_back({i+(10*j)+1, i+(10*j)+11, i+(10*j)+10});
		}
	}
	
	cube.vertices(cube_vertices.data(), cube_vertices.size()*sizeof(vertex));
	cube.indices(cube_indices.data(), cube_indices.size()*sizeof(face));
	cube.addVertexAttrib(shader_color.attrib("position"), 3, sizeof(vertex), offsetof_ptr(vertex, pos));
	cube.addVertexAttrib(shader_color.attrib("color"), 3, sizeof(vertex), offsetof_ptr(vertex, norm));

	terrain.vertices(terrain_vertices.data(), terrain_vertices.size()*sizeof(vertex));
	terrain.indices(terrain_indices.data(), terrain_indices.size()*sizeof(face));
	terrain.addVertexAttrib(shader_uv.attrib("position"), 3, sizeof(vertex), offsetof_ptr(vertex, pos));
//	terrain.addVertexAttrib(shader_uv.attrib("normal"), 3, sizeof(vertex), offsetof_ptr(vertex, norm));
	terrain.addVertexAttrib(shader_uv.attrib("uv"), 2, sizeof(vertex), offsetof_ptr(vertex, text));

	Sphere<30, 30> sph(1.5f);
	sphere.vertices(sph.vertices.data(), sph.vertices.size()*sizeof(vertex));
	sphere.indices(sph.indices.data(), sph.indices.size()*sizeof(face));
	sphere.addVertexAttrib(shader_uv.attrib("position"), 3, sizeof(vertex), offsetof_ptr(vertex, pos));
//	sphere(shader_uv.attrib("normal"), 3, sizeof(vertex), offsetof_ptr(vertex, norm));
	sphere.addVertexAttrib(shader_uv.attrib("uv"), 2, sizeof(vertex), offsetof_ptr(vertex, text));

	
	model = glm::mat4(1.f);
	view = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.0f, -4.f));
	projection = glm::perspective(tau/6.f, float(w_width)/float(w_height), .1f, 150.f);


	float gray = 45.f / 255.f;
	glClearColor(gray, gray, gray, 1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void pipelineGL::update(float dt)
{
	if(glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(win, GL_TRUE);
	model = glm::rotate(model, dt, glm::vec3(1.f, 1.f, .0f));
}

void pipelineGL::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, f_width, f_height);
	
//	shader_color.bind();
//	shader_color.load("model", model);
//	shader_color.load("view", view);
//	shader_color.load("projection", projection);
//	cube.draw();
	
	background.bind();
	shader_uv.bind();
	shader_uv.load("model", model);
	shader_uv.load("view", view);
	shader_uv.load("projection", projection);
//	terrain.draw();
	sphere.draw();
}