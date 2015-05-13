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

#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <functional>
#include <regex>

#include "geometry.hpp"
#include "json11.hpp"

Geometry::Geometry() : vert(GL_ARRAY_BUFFER, GL_STATIC_DRAW), ind(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW), vao(0), size(0)
{
	glGenVertexArrays(1, &vao);
}
Geometry::~Geometry()
{
	glDeleteVertexArrays(1, &vao);
}

Geometry::Geometry(Geometry&& g) : vert(std::move(g.vert)), ind(std::move(g.ind)),
	vao(g.vao), size(g.size), vert_init(g.vert_init), ind_init(g.ind_init), vao_init(g.vao_init)
{
	g.vao = 0;
}
Geometry& Geometry::operator=(Geometry&& g)
{
	assert(this != &g);
	glDeleteVertexArrays(1, &vao);
	
	vert = std::move(g.vert);
	ind = std::move(g.ind);
	vao = g.vao;
	size = g.size;
	vert_init = g.vert_init;
	ind_init = g.ind_init;
	vao_init = g.vao_init;
	g.vao = 0;
	return *this;
}

void Geometry::draw(GLsizei _count)
{
	if(!vert_init || !ind_init || !vao_init || !size)
	{
		std::cout << "incomplete geometry status" << std::endl;
		return;
	}
	glBindVertexArray(vao);
	glDrawElementsInstanced(GL_TRIANGLES, GLsizei(size), GL_UNSIGNED_INT, 0, _count);
}

void Geometry::drawArray(GLsizei _count)
{
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, _count);
}

void Geometry::vertices(const void* _data, size_t _byte_size)
{
	if(vert_init)
	{
		vert.dataUpdate(_data, _byte_size);
	}
	else
	{
		vert.data(_data, _byte_size);
		vert_init = true;
	}
}
void Geometry::indices(const void* _data, size_t _byte_size)
{
	size = _byte_size/sizeof(GLuint); // expect gluint array
	ind.data(_data, _byte_size);

	glBindVertexArray(vao);
	ind.bind(); // element array is part of vao state
	glBindVertexArray(0);
	ind.unbind();

	ind_init = true;
}
// _index index of vertex attribute, _size number of components(1..4), _stride total vertex size, _offset byte offset into buffer
void Geometry::addVertexAttrib(GLuint _index, GLint _size, GLuint _stride, const GLvoid* _offset)
{
	vert.bind();
	glBindVertexArray(vao);
	glEnableVertexAttribArray(_index);
	glVertexAttribPointer(_index, _size, GL_FLOAT, GL_FALSE, _stride, _offset); // array buffer isn't part of vao state
	glBindVertexArray(0);
	vert.unbind();

	vao_init = true;
}

void Geometry::addVertexAttribI(GLuint _index, GLint _size, GLuint _stride, const GLvoid* _offset)
{
	vert.bind();
	glBindVertexArray(vao);
	glEnableVertexAttribArray(_index);
	glVertexAttribIPointer(_index, _size, GL_UNSIGNED_BYTE, _stride, _offset); // array buffer isn't part of vao state
	glBindVertexArray(0);
	vert.unbind();

	vao_init = true;
}

struct Attrib
{
	GLint components;
	GLenum type;
	GLint size;
};

Geometry Geometry::fromFile(const std::string& _filename)
{
	Geometry g;

	std::string source(static_cast<std::stringstream const&>(std::stringstream() << std::ifstream(_filename).rdbuf()).str());
	std::string error;
	auto json = json11::Json::parse(source, error);
	if (!error.empty())
	{
		std::cout << "failed: " << error << std::endl;
		return g;
	}

	auto version = json["version"].array_items();
	std::vector<int> version_v;
	std::transform(version.begin(), version.end(), std::back_inserter(version_v), [](const json11::Json& j){return j.integer_value(); });

	assert((version == json11::Json::array{0, 1}));

	auto meshes = json["meshes"].array_items();
	//for (size_t n_mesh = 0; n_mesh < meshes.size(); ++n_mesh)
	//{
		auto attributes = meshes[0]["attributes"].array_items(); // TODO arbitrary number of meshes
	std::vector<std::string> attributes_v;
	std::transform(attributes.begin(), attributes.end(), std::back_inserter(attributes_v), [](const json11::Json& j){return j.string_value(); });

	std::unordered_map<std::string, Attrib, std::function<size_t(std::string)>, std::function<bool(const std::string&, const std::string)>> attributes_hash(
		3,
		[](const std::string& s) {
		std::regex re("([0-9]*)$");
		std::smatch sm;
		std::regex_search(s, sm, re);
		//std::cout << s << " " << sm.prefix().str() << " " << sm.suffix().str() << std::endl;
		return std::hash<std::string>()(sm.prefix().str());
	},
		[](const std::string& me, const std::string& s)
	{
		//std::cout << "search " << me << " " << s << " " << std::boolalpha << (s.find(me) != std::string::npos || me.find(s) != std::string::npos) << std::endl;
		return s.find(me) != std::string::npos || me.find(s) != std::string::npos;
	}
	);
	attributes_hash["POSITION"] = { 3, GL_FLOAT, 3 * sizeof(float) };
	attributes_hash["NORMAL"] = { 3, GL_FLOAT, 3 * sizeof(float) };
	attributes_hash["TEXCOORD"] = { 2, GL_FLOAT, 2 * sizeof(float) };

	GLuint v_size = 0; // total vertex size
	std::vector<size_t> offset; // per attribute offset
	std::vector<GLint> components; // per attribute number of components
	for (auto& a : attributes_v)
	{
		offset.push_back(v_size);
		components.push_back(attributes_hash[a].components);
		v_size += attributes_hash[a].size;
	}

	for (GLint i = 0; i < offset.size(); ++i)
	{
		// TODO check if type is float or int
		g.addVertexAttrib(i, components[i], v_size, (void*)offset[i]);
	}

	auto vertices = meshes[0]["vertices"].array_items();
	std::vector<float> vertices_v;
	std::transform(vertices.begin(), vertices.end(), std::back_inserter(vertices_v), [](const json11::Json& j){ return j.number_value(); });

	g.vertices(vertices_v.data(), vertices_v.size()*sizeof(float));

	auto parts = meshes[0]["parts"].array_items();
	std::vector<GLuint> indices_v;
	for (auto& p : parts)
	{
		assert(p["type"] == "TRIANGLES");
		auto i = p["indices"].array_items();
		std::transform(i.begin(), i.end(), std::back_inserter(indices_v), [](const json11::Json& j){ return j.integer_value(); });
	}

	g.indices(indices_v.data(), indices_v.size()*sizeof(GLuint));

	return g;
}