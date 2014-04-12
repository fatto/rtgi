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

#include <vector>
#include <iostream>
#include <cassert>

#include "texture.hpp"
#include "lodepng.h"

Texture2::Texture2(GLuint _unit) : unit(_unit)
{
	glGenTextures(1, &handle);
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}
Texture2::~Texture2()
{
	glDeleteTextures(1, &handle);
}

Texture2::Texture2(Texture2&& t) : handle(t.handle), unit(t.unit)
{
	t.handle = 0;
}
Texture2& Texture2::operator=(Texture2&& t)
{
	assert(this != &t);
	glDeleteTextures(1, &handle);

	handle = t.handle;
	unit = t.unit;
	t.handle = 0;
	return *this;
}
void Texture2::file(const std::string& _filename)
{
	std::vector<GLubyte> image;
	GLuint width, height;
	unsigned error = lodepng::decode(image, width, height, _filename.c_str());
	if(error)
		std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
	glGenerateMipmap(GL_TEXTURE_2D);
}
void Texture2::bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, handle);
}
GLuint Texture2::getUnit()
{
	return unit;
}


Texture3::Texture3(GLuint _unit) : unit(_unit)
{
	glGenTextures(1, &handle);
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_3D, handle);
}
Texture3::~Texture3()
{
	glDeleteTextures(1, &handle);
}

Texture3::Texture3(Texture3&& t) : handle(t.handle), unit(t.unit)
{
	t.handle = 0;
}
Texture3& Texture3::operator=(Texture3&& t)
{
	assert(this != &t);
	glDeleteTextures(1, &handle);

	handle = t.handle;
	unit = t.unit;
	t.handle = 0;
	return *this;
}
void Texture3::alloc(GLsizei _size_x, GLsizei _size_y, GLsizei _size_z)
{
	bind();
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32UI, _size_x, _size_y, _size_z, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	clear();
	glGenerateMipmap(GL_TEXTURE_3D);
}
void Texture3::clear()
{
	glClearTexImage(handle, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
}
void Texture3::bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_3D, handle);
}
void Texture3::bindImage(GLenum _format, GLenum _access, GLint _level)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindImageTexture(unit, handle, _level, GL_TRUE, 0, _access, _format);
}
GLuint Texture3::getUnit()
{
	return unit;
}



TextureBuffer::TextureBuffer(GLuint _unit) : unit(_unit)
{
	glGenTextures(1, &handle);
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_BUFFER, handle);
}
TextureBuffer::~TextureBuffer()
{
	glDeleteTextures(1, &handle);
}

TextureBuffer::TextureBuffer(TextureBuffer&& t) : handle(t.handle), unit(t.unit)
{
	t.handle = 0;
}
TextureBuffer& TextureBuffer::operator=(TextureBuffer&& t)
{
	assert(this != &t);
	glDeleteTextures(1, &handle);

	handle = t.handle;
	unit = t.unit;
	t.handle = 0;
	return *this;
}
void TextureBuffer::buffer(GLuint _buffer, GLenum _format)
{
	bind();
	glTexBuffer(GL_TEXTURE_BUFFER, _format, _buffer);
}
void TextureBuffer::bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_BUFFER, handle);
}
void TextureBuffer::bindImage(GLenum _format, GLenum _access, GLint _level)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindImageTexture(unit, handle, _level, GL_TRUE, 0, _access, _format);
}
GLuint TextureBuffer::getUnit()
{
	return unit;
}