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

#ifndef rtgi_shader_hpp
#define rtgi_shader_hpp

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
// #include <regex>

#include "glwrap.hpp"

class Shader
{
private:
	GLuint program;
	//GLuint vertex, fragment;
	std::unordered_map<std::string, GLuint> uniforms;
	std::unordered_map<std::string, GLuint> attribs;
	std::unordered_map<GLenum, GLuint> stage;
	// bool vert_init = false, frag_init = false, pro_init = false;
	std::unordered_map<std::string, GLenum> ext_enum;
	// const std::regex file_extension("\.([A-Za-z0-9]+)");
public:
	Shader();
	~Shader();

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	Shader(Shader&& s);
	Shader& operator=(Shader&& s);

	void setStage(const std::string& _filename);
	// void setVertex(const std::string& _filename);
	void link();

	void bind();
	void load(const std::string& _name, const GLint _data);
	void load(const std::string& _name, const glm::vec3& _data);
	void load(const std::string& _name, const glm::mat4& _data);
	GLint attrib(const std::string& _name);
private:
	GLuint compile_shader(const std::string& source, GLenum type) const;
	void load_uniform();
	void load_attrib();
};

#endif