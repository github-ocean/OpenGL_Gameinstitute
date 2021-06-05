#include <iostream>
#include <sstream>

#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"

const char* APP_TITLE = "Modern OpenGL Tutorial";
const int gWindowWidth = 800;
const int gWindowHight = 600;
bool gFullscreen = false;
GLFWwindow* gWindow = NULL;

const GLchar* vertexShaderSrc =
"#version 330 core\n"
"layout (location = 0) in vec3 pos;"
"layout (location = 1) in vec3 color;"
"out vec3 vert_color;"
"void main()"
"{"
"	vert_color = color;"	
"	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);"
"}";

const GLchar* fragmentShaderSrc =
"#version 330 core\n"
"in vec3 vert_color;"
"out vec4 frag_color;"
"void main()"
"{"
"	frag_color = vec4(vert_color, 1.0f);"
"}";

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void showFPS(GLFWwindow* window);
bool initOpenGL();

int main()
{
	if (!initOpenGL())
	{
		std::cerr << "OpenGL Initialization Failed." << std::endl;
		return -1;
	}	

	GLfloat Vertices[] = {
			// POSITION				COLOR
		 0.0f,  0.5f, 0.0f,		1.0f, 0.0f, 0.0f, // Top
		 0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f, // Right
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f, // Left
	};

	GLuint vbo, vao;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(1);

	// POSITION
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, NULL);
	glEnableVertexAttribArray(0);

	// COLOR
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);

	GLint result;
	GLchar infoLog[512];
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertexShaderSrc, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vs, sizeof(infoLog), NULL, infoLog);
		std::cout << "Error! vertex shader failed to compile. " << infoLog << std::endl;
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fs, sizeof(infoLog), NULL, infoLog);
		std::cout << "Error! Fragment Shader faild to compile. " << infoLog << std::endl;
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vs);
	glAttachShader(shaderProgram, fs);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
		std::cout << "Error! Shader Program linker failure " << infoLog << std::endl;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);

	// main loop
	while (!glfwWindowShouldClose(gWindow))
	{
		showFPS(gWindow);
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(gWindow);
	}

	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	glfwTerminate();
	return 0;
}

bool initOpenGL()
{

	if (!glfwInit())
	{
		std::cerr << "Faild to create GLFW window." << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	if (gFullscreen)
	{
		GLFWmonitor* pMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* pVmode = glfwGetVideoMode(pMonitor);
		if (pVmode != NULL)
		{
			gWindow = glfwCreateWindow(pVmode->width, pVmode->height, APP_TITLE, pMonitor, NULL);
		}
	}
	else
	{
		gWindow = glfwCreateWindow(gWindowWidth, gWindowHight, APP_TITLE, NULL, NULL);
	}

	if (gWindow == NULL)
	{
		std::cerr << "Faild to create FLFW Window." << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(gWindow);

	glfwSetKeyCallback(gWindow, glfw_onKey);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "GLEW initialization faild." << std::endl;
		return false;
	}

	glClearColor(0.23f, 0.38f, 0.47f, 1.0f);
	return true;
}

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void showFPS(GLFWwindow* window)
{
	static double previousSecond = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime();  // returns number of seconds since FLFW started, as a double.

	elapsedSeconds = currentSeconds - previousSecond;

	// limit text update 4 times per second.
	if (elapsedSeconds > 0.25)
	{
		previousSecond = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		std::ostringstream outs;
		outs.precision(3);
		outs << std::fixed
			<< APP_TITLE << "  "
			<< "FPS: " << fps << "  "
			<< "Frame Time: " << msPerFrame << " (ms)";
		glfwSetWindowTitle(window, outs.str().c_str());

		frameCount = 0;
	}
	frameCount++;
}

