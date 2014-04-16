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

#ifndef rtgi_geometry_hpp
#define rtgi_geometry_hpp

#include <array>
#include <cmath>

#include "glwrap.hpp"
#include "buffer.hpp"

#define offsetof_ptr(oggetto, campo) (void*)offsetof(oggetto, campo)

struct vertex
{
	std::array<GLfloat, 3> pos;
	std::array<GLfloat, 3> norm;
	std::array<GLfloat, 2> text;
};
struct face
{
	std::array<GLuint, 3> triang;
};
static const float tau = 6.28318530717959f;

class Geometry
{
	Buffer vert, ind;
	GLuint vao;
	size_t size;
	bool vert_init = false, ind_init = false, vao_init = false;
public:
	Geometry();
	~Geometry();

	Geometry(const Geometry&) = delete;
	Geometry& operator=(const Geometry&) = delete;

	Geometry(Geometry&& g);
	Geometry& operator=(Geometry&& g);
	
	void draw();
	void drawArray(GLsizei _count);

	void vertices(const void* _data, size_t _byte_size);
	void indices(const void* _data, size_t _byte_size);
	// _index index of vertex attribute, _size number of components(1..4), _stride total vertex size, _offset byte offset into buffer
	void addVertexAttrib(GLuint _index, GLint _size, GLuint _stride, const GLvoid* _offset);
	void addVertexAttribI(GLuint _index, GLint _size, GLuint _stride, const GLvoid* _offset);
};


template<GLuint rings, GLuint sectors>
struct Sphere
{
	std::array<vertex, rings * sectors> vertices;
	std::array<face, (rings-1) * sectors * 2> indices;
	
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
template<GLuint div>
struct Plane
{
	std::array<vertex, (div+1)*(div+1)> vertices;
	std::array<face, div*div*2> indices;
	Plane(GLfloat size)
	{
		auto v = vertices.begin();
		auto in = indices.begin();
		for(size_t i = 0; i <= div; ++i)
		{
			for(size_t j = 0; j <= div; ++j)
			{
				auto x = (float(i)/(float(div)))-0.5f;
				auto z = (float(j)/(float(div)))-0.5f;
				v->pos = {{ x*size, 0.f,  z*size }};
				v->norm = {{ 0.f, 1.f, 0.f }};
				v->text = {{ float(i)/float(div), float(j)/float(div) }};
				++v;
			}
		}
		for(GLuint i = 0; i < div; ++i)
		{
			for(GLuint j = 0; j < div; ++j)
			{
				in->triang = { { i + ((div + 1)*j), i + ((div + 1)*j) + 1, i + ((div + 1)*j) + (div + 1) } };
				++in;
				in->triang = { { i + ((div + 1)*j) + 1, i + ((div + 1)*j) + 1 + (div + 1), i + ((div + 1)*j) + (div + 1) } };
				++in;
			}
		}
	}
};
// struct Cube
// {
// 	std::array<vertex,8> vertices;
// 	std::array<face,12> indices;
// 	Cube(GLfloat size)
// 	{
// 		vertices = std::array<vertex, 8>
// 		{
// 			{{-size, -size, size}, {.0f, .0f, 1.f},{.0f, .0f}},
// 			{{size, -size, size}, {1.f, .0f, 1.f},{.0f, .0f}},
// 			{{size, size, size}, {1.f, 1.f, 1.f},{.0f, .0f}},
// 			{{-size, size, size}, {.0f, 1.f, 1.f},{.0f, .0f}},
// 			{{-size, -size, -size}, {.0f, .0f, .0f},{.0f, .0f}},
// 			{{size, -size, -size}, {1.f, .0f, .0f},{.0f, .0f}},
// 			{{size, size, -size}, {1.f, 1.f, .0f},{.0f, .0f}},
// 			{{-size, size, -size}, {.0f, 1.f, .0f},{.0f, .0f}}
// 		};
// 	 	indices = std::array<face, 12>
// 	 	{
// 			{{0, 1, 2}}, {{0, 2, 3}},
// 			{{1, 5, 6}}, {{1, 6, 2}},
// 			{{5, 4, 7}}, {{5, 7, 6}},
// 			{{4, 0, 3}}, {{4, 3, 7}},
// 			{{2, 6, 7}}, {{2, 7, 3}},
// 			{{0, 4, 5}}, {{0, 5, 1}}
// 		};
// 	}
// };
#endif