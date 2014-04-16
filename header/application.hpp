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

#ifndef rtgi_application_hpp
#define rtgi_application_hpp

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "glwrap.hpp"
#include "geometry.hpp"
#include "texture.hpp"
#include "shader.hpp"

#include <array>
#include <cassert>

class application
{
	GLFWwindow* win;
	int w_width, w_height;
	int f_width, f_height;
	Geometry sphere;
	Geometry quad;
	Geometry text;
	Shader shader_voxelize;
	Shader shader_ray;
	Shader shader_text;
	Texture2 fontmap;
	Texture3 tex3;
	glm::mat4 model, view, projection;
	float delta = 0.f;
public:
	application(GLFWwindow* _win);
	void update(float dt);
	void draw();
};

#endif
