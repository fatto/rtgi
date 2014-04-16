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

#include "application.hpp"
#include "glwrap.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <csignal>

#if defined(_MSC_VER)
#pragma comment(lib,"opengl32.lib")
// #pragma comment(lib,"glew32.lib")

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

static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg)
{
	std::stringstream stringStream;
	std::string sourceString;
	std::string typeString;
	std::string severityString;
 
	// The AMD variant of this extension provides a less detailed classification of the error,
	// which is why some arguments might be "Unknown".
	switch (source) {
		case GL_DEBUG_SOURCE_API: {
			sourceString = "API";
			break;
		}
		case GL_DEBUG_SOURCE_APPLICATION: {
			sourceString = "Application";
			break;
		}
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
			sourceString = "Window System";
			break;
		}
		case GL_DEBUG_SOURCE_SHADER_COMPILER: {
			sourceString = "Shader Compiler";
			break;
		}
		case GL_DEBUG_SOURCE_THIRD_PARTY: {
			sourceString = "Third Party";
			break;
		}
		case GL_DEBUG_SOURCE_OTHER: {
			sourceString = "Other";
			break;
		}
		default: {
			sourceString = "Unknown";
			break;
		}
	}
 
	switch (type) {
		case GL_DEBUG_TYPE_ERROR: {
			typeString = "Error";
			break;
		}
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
			typeString = "Deprecated Behavior";
			break;
		}
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
			typeString = "Undefined Behavior";
			break;
		}
		/*case GL_DEBUG_TYPE_PORTABILITY_ARB: {
			typeString = "Portability";
			break;
		}*/
		case GL_DEBUG_TYPE_PERFORMANCE: {
			typeString = "Performance";
			break;
		}
		case GL_DEBUG_TYPE_OTHER: {
			typeString = "Other";
			break;
		}
		default: {
			typeString = "Unknown";
			break;
		}
	}
 
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH: {
			severityString = "High";
			break;
		}
		case GL_DEBUG_SEVERITY_MEDIUM: {
			severityString = "Medium";
			break;
		}
		case GL_DEBUG_SEVERITY_LOW: {
			severityString = "Low";
			break;
		}
		default: {
			severityString = "Unknown";
			break;
		}
	}
 
	stringStream << "OpenGL Error: " << msg;
	stringStream << " [Source = " << sourceString;
	stringStream << ", Type = " << typeString;
	stringStream << ", Severity = " << severityString;
	stringStream << ", ID = " << id << "]";
 
	return stringStream.str();
}
static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam)
{
	std::string error = FormatDebugOutput(source, type, id, severity, message);
	std::cout << error << std::endl;
	if(severity == GL_DEBUG_SEVERITY_HIGH)
		raise(SIGABRT);
}

int main(int argc, char*argv[])
{
	using timer = std::chrono::high_resolution_clock;
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	
	if(!glfwInit())
		return EXIT_FAILURE;
	
#if defined(__APPLE_CC__)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
	window = glfwCreateWindow(800, 600, "cg-lab", NULL, NULL);
	if(!window)
	{
		glfwTerminate();
		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

// #if defined(_MSC_VER)
// 	glewExperimental = true;
// 	if (glewInit() != GL_NO_ERROR)
// 	{
// 		glfwTerminate();
// 		return EXIT_FAILURE;
// 	}
// #else
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(DebugCallback, NULL);
// #endif

	// std::cout << glGetString(GL_RENDERER) << std::endl;
	// std::cout << glGetString(GL_VERSION) << std::endl;
	// std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	
	application app(window);
	auto old_time = timer::now();
	auto current_time = old_time;
	float ratio_timer = float(timer::period::num)/float(timer::period::den);
	while (!glfwWindowShouldClose(window))
	{
		current_time = timer::now();
		float delta = (current_time - old_time).count() * ratio_timer;
		old_time = current_time;
//		glfwSetWindowTitle(window, std::to_string(delta).c_str());

		app.update(delta);
		app.draw();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwDestroyWindow(window);
	glfwTerminate();
	
	return EXIT_SUCCESS;
}
