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

#include <sstream>
#include <fstream>
#include <iostream>

#include "shader.hpp"

Shader::Shader()
{
	// stage[GL_VERTEX_SHADER] = 0;
	// stage[GL_GEOMETRY_SHADER] = 0;
	// stage[GL_FRAGMENT_SHADER] = 0;
	ext_enum = std::unordered_map<std::string, GLenum>{
		{ { "vert", GL_VERTEX_SHADER } },
		{ { "geom", GL_GEOMETRY_SHADER } },
		{ { "frag", GL_FRAGMENT_SHADER } }
	};
	program = glCreateProgram();
}
Shader::~Shader()
{
	//glDetachShader(program, vertex);
	//glDetachShader(program, fragment);
	//glDeleteShader(vertex);
	//glDeleteShader(fragment);
	for(auto& s : stage)
	{
		// std::cout << "stage " << s.second << std::endl;
		glDetachShader(program, s.second);
		glDeleteShader(s.second);
	}

	// std::cout << "program " << program << std::endl;
	glDeleteProgram(program);
}

Shader::Shader(Shader&& s) : program(s.program),/* vertex(s.vertex), fragment(s.fragment),*/ uniforms(std::move(s.uniforms)), stage(std::move(s.stage))
{
	// s.program = 0;
	// s.vertex = 0;
	// s.fragment = 0;
}
Shader& Shader::operator=(Shader&& s)
{
	assert(this != &s);
	// glDetachShader(program, vertex);hader.setStage("shader/uv.vert");
	// shader.setStage("shader/uv.frag");
	// shader.link();
	// glDetachShader(program, fragment);
	// glDeleteShader(vertex);
	// glDeleteShader(fragment);
	// glDeleteProgram(program);
	for(auto& st : stage)
	{
		// std::cout << "stage " << st.second << std::endl;
		glDetachShader(program, st.second);
		glDeleteShader(st.second);
	}

	// std::cout << "program " << program << std::endl;
	glDeleteProgram(program);

	program = s.program;
	// vertex = s.vertex;
	// fragment = s.fragment;
	uniforms = std::move(s.uniforms);
	stage = std::move(s.stage);

	s.program = 0;
	// s.vertex = 0;
	// s.fragment = 0;
	return *this;
}

void Shader::setStage(const std::string& _filename)
{
	std::string source(static_cast<std::stringstream const&>(std::stringstream() << std::ifstream(_filename).rdbuf()).str());
	GLenum type = ext_enum.at(_filename.substr(_filename.find_last_of(".") + 1));
	GLuint handle = compile_shader(source, type);
	if(!handle)
	{
		std::cout << "failed to compile " << _filename << std::endl;
		return;
	}
	if(stage[type] != 0)
	{
		glDetachShader(program, stage[type]);
		glDeleteShader(stage[type]);
	}
	glAttachShader(program, handle);
	stage[type] = handle;
	// auto banana = stage.size();
	// std::cout << banana << std::endl;
}
// void setVertex(const std::string& _filename)
// {
// 	std::string source(static_cast<std::stringstream const&>(std::stringstream() << std::ifstream(_filename).rdbuf()).str());
// 	vertex = compile_shader(source, GL_VERTEX_SHADER);
// 	if(!vertex)
// 	{
// 		std::cout << "failed to compile " << _filename << std::endl;
// 	}
// 	// vert_init = true;
// }
// void setFragment(const std::string& _filename)
// {
// 	std::string source(static_cast<std::stringstream const&>(std::stringstream() << std::ifstream(_filename).rdbuf()).str());
// 	fragment = compile_shader(source, GL_FRAGMENT_SHADER);
// 	if(!fragment)
// 	{
// 		std::cout << "failed to compile " << _filename << std::endl;
// 	}
// 	// frag_init = true;
// }
void Shader::link()
{
	// if(!vertex || !fragment)
	// {
	// 	std::cout << "invalid shader stage" << std::endl;
	// 	return;
	// }
	// glAttachShader(program, vertex);
	// glAttachShader(program, fragment);
	glLinkProgram(program);

	GLint status = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if(!status)
	{
		std::cout << "failed to link" << std::endl;
		GLchar log[256];
		GLsizei slen = 0;
		glGetProgramInfoLog(program, 256, &slen, log);
		std::cout << log << std::endl;
	}
	else
	{
		load_uniform();
		load_attrib();
		// prog_init = true;
	}
}

void Shader::bind()
{
	glUseProgram(program);
}
void Shader::load(const std::string& _name, const GLint _data)
{
	glUniform1i(uniforms.at(_name), _data);
}
void Shader::load(const std::string& _name, const glm::vec3& _data)
{
	glUniform3fv(uniforms.at(_name), 1, &_data[0]);
}
void Shader::load(const std::string& _name, const glm::mat4& _data)
{
	glUniformMatrix4fv(uniforms.at(_name), 1, GL_FALSE, &_data[0][0]);
}
GLint Shader::attrib(const std::string& _name)
{
	return glGetAttribLocation(program, _name.c_str());
}
GLuint Shader::compile_shader(const std::string& source, GLenum type) const
{
#if defined(__APPLE_CC__)
	std::string _source("#version 330\n" + source);
#else
	std::string _source("#version 440\n" + source);
#endif
	GLuint handle = glCreateShader(type);
	const GLchar* c_str = _source.c_str();
	glShaderSource(handle, 1, &c_str, NULL);
	glCompileShader(handle);

	{
		GLchar log[256];
		GLsizei slen = 0;
		glGetShaderInfoLog(handle, 256, &slen, log);
		if(slen)
			std::cout << log << std::endl;
	}

	GLint status = 0;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
	return status? handle : 0;
}
void Shader::load_uniform()
{
	GLint uniforms_size = 0;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniforms_size);
	for(GLint i = 0; i < uniforms_size; ++i)
	{
		std::string name_uniform;
		{
			GLchar name[256];
			GLint array_size = 0, actual_length = 0;
			GLenum type = 0;
			glGetActiveUniform(program, i, 256, &actual_length, &array_size, &type, name);
			name_uniform = std::string{name, size_t(actual_length)};
		}
		GLuint location = glGetUniformLocation(program, name_uniform.c_str());
		if(location != GLuint(-1))
			uniforms[name_uniform] = location;
	}
}

void Shader::load_attrib()
{
	GLint attrubutes_size = 0;
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &attrubutes_size);
	for (GLint i = 0; i < attrubutes_size; ++i)
	{
		std::string name_attrib;
		{
			GLchar name[256];
			GLint array_size = 0, actual_length = 0;
			GLenum type = 0;
			glGetActiveAttrib(program, i, 256, &actual_length, &array_size, &type, name);
			name_attrib = std::string{ name, size_t(actual_length) };
		}
		GLuint location = glGetAttribLocation(program, name_attrib.c_str());
		if (location != GLuint(-1))
			attribs[name_attrib] = location;
	}
}