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
#include "buffer.hpp"

Buffer::Buffer(GLenum _type, GLenum _usage) : type(_type), usage(_usage)
{
	glGenBuffers(1, &handle);
}
Buffer::~Buffer()
{
	glDeleteBuffers(1, &handle);
}

Buffer::Buffer(Buffer&& b) : type(b.type), usage(b.usage), handle(b.handle)
{
	b.handle = 0;
}
Buffer& Buffer::operator=(Buffer&& b)
{
	assert(this != &b);
	glDeleteBuffers(1, &handle);
	
	type = b.type;
	usage = b.usage;
	handle = b.handle;
	b.handle = 0;
	return *this;
}

void Buffer::bind()
{
	glBindBuffer(type, handle);
}
void Buffer::unbind()
{
	glBindBuffer(type, 0);
}
void Buffer::data(void* _data, size_t _size)
{
	bind();
	glBufferData(type, _size, _data, usage);
	unbind();
}
GLuint Buffer::getName()
{
	return handle;
}