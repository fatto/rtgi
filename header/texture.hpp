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

#ifndef rtgi_texture_hpp
#define rtgi_texture_hpp

#include <string>

#include "glwrap.hpp"

class Texture2
{
	GLuint handle;
	GLuint unit;
public:
	Texture2(GLuint _unit = 0);
	~Texture2();

	Texture2(const Texture2&) = delete;
	Texture2& operator=(const Texture2&) = delete;

	Texture2(Texture2&& t);
	Texture2& operator=(Texture2&& t);

	void file(const std::string& _filename);
	void bind();
	GLuint getUnit();
};

class Texture3
{
	GLuint handle;
	GLuint unit;
public:
	Texture3(GLuint _unit = 0);
	~Texture3();

	Texture3(const Texture3&) = delete;
	Texture3& operator=(const Texture3&) = delete;

	Texture3(Texture3&& t);
	Texture3& operator=(Texture3&& t);

	void alloc(GLsizei _size_x, GLsizei _size_y, GLsizei _size_z);
	void clear();
	void bind();
	void bindImage(GLenum _format, GLenum _access = GL_READ_WRITE, GLint _level = 0);
	void unbind();
	GLuint getUnit();
};

class TextureBuffer
{
	GLuint handle;
	GLuint unit;
public:
	TextureBuffer(GLuint _unit = 0);
	~TextureBuffer();

	TextureBuffer(const TextureBuffer&) = delete;
	TextureBuffer& operator=(const TextureBuffer&) = delete;

	TextureBuffer(TextureBuffer&& t);
	TextureBuffer& operator=(TextureBuffer&& t);

	void buffer(GLuint _buffer, GLenum _format);
	void bind();
	void bindImage(GLenum _format, GLenum _access = GL_READ_WRITE, GLint _level = 0);
	GLuint getUnit();
};

#endif