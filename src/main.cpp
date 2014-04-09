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

#include "pipelineGL.hpp"
#include "glwrap.hpp"

#include <iostream>
#include <string>
#include <chrono>

#if defined(_MSC_VER)
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glew32.lib")

#if defined(_DEBUG)
#pragma comment(lib,"glfw3d.lib")
#else
#pragma comment(lib,"glfw3.lib")
#endif

#endif

static void error_callback(int error, const char* description)
{
	std::cout << error << " " << description << std::endl;
}

int main(int argc, char*argv[])
{
	using timer = std::chrono::high_resolution_clock;
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	
	if(!glfwInit())
		exit(EXIT_FAILURE);
	
#if defined(__APPLE_CC__)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif
	window = glfwCreateWindow(800, 600, "cg-lab", NULL, NULL);
	if(!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

#if defined(_MSC_VER)
	glewExperimental = true;
	if (glewInit() != GL_NO_ERROR)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
#endif

	// std::cout << glGetString(GL_RENDERER) << std::endl;
	// std::cout << glGetString(GL_VERSION) << std::endl;
	// std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	
	pipelineGL gl(window);
	auto old_time = timer::now();
	auto current_time = old_time;
	float ratio_timer = float(timer::period::num)/float(timer::period::den);
	while (!glfwWindowShouldClose(window))
	{
		current_time = timer::now();
		float delta = (current_time - old_time).count() * ratio_timer;
		old_time = current_time;
//		glfwSetWindowTitle(window, std::to_string(delta).c_str());

		gl.update(delta);
		gl.draw();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwDestroyWindow(window);
	glfwTerminate();
	
	exit(EXIT_SUCCESS);
}
