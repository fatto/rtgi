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

#include "geometry.hpp"

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

void Geometry::draw()
{
	if(!vert_init || !ind_init || !vao_init || !size)
	{
		std::cout << "incomplete geometry status" << std::endl;
		return;
	}
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, GLsizei(size), GL_UNSIGNED_INT, 0);
}

void Geometry::vertices(void* _data, size_t _byte_size)
{
	vert.data(_data, _byte_size);
	vert_init = true;
}
void Geometry::indices(void* _data, size_t _byte_size)
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