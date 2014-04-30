#include "framebuffer.hpp"

#include <cassert>
#include <vector>
#include <iostream>

FrameBuffer::FrameBuffer()
{
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
}
FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &fb);
}

FrameBuffer::FrameBuffer(FrameBuffer&& f) : fb(f.fb), colors(f.colors), depth(f.depth)
{
	f.fb = 0;
}
FrameBuffer& FrameBuffer::operator=(FrameBuffer&& f)
{
	assert(this != &f);
	glDeleteFramebuffers(1, &fb);

	fb = f.fb;
	colors = f.colors;
	depth = f.depth;

	f.fb = 0;

	return *this;
}

void FrameBuffer::addColor(const GLuint _handle)
{
	glBindTexture(GL_TEXTURE_2D, _handle);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colors, _handle, 0);
	++colors;
}

void FrameBuffer::setDepth(const GLuint _handle)
{
	glBindTexture(GL_TEXTURE_2D, _handle);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _handle, 0);
}

void FrameBuffer::check()
{
	std::vector<GLenum> draws;
	for(size_t i = 0; i < colors; ++i)
		draws.push_back(GL_COLOR_ATTACHMENT0 + i);

	glDrawBuffers(draws.size(), draws.data());

	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "GL_FRAMEBUFFER_COMPLETE fail" << std::endl;
}

void FrameBuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
}
void FrameBuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}