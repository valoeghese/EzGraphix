#include "ezgraphix.h"

#include <stdio.h>
#include <stdlib.h>

// these functions must be declared
int setup(void); // called for setup
void draw(void); // called every frame
void cleanup(void); // called at the end to clean up memory used

struct EzGlobalContext {
	GLFWwindow* window;
	EZkeyfun keyFun;
	EZmemerrfun memErrFun;
	int winWidth;
	int winHeight;
	unsigned int shaderProgram;
} g_ezCtx;

// ==================
// API Implementation
// ===================

// Struct 

struct _EZobject {
	int ibo;
	int vbo;
	int roundness;
	float r;
	float g;
	float b;
};

// Window

void ezTitle(const char* title) {
	glfwSetWindowTitle(g_ezCtx.window, title);
}

void ezSize(const int width, const int height) {
	g_ezCtx.winWidth = width;
	g_ezCtx.winHeight = height;
	glfwSetWindowSize(g_ezCtx.window, width, height);
	glViewport(0, 0, width, height); // tell gl to adapt accordingly

	// set in shader
	glUniform2f(glGetUniformLocation(g_ezCtx.shaderProgram, "window_size"), (float)width, (float)height);
}

void ezClose(void) {
	glfwSetWindowShouldClose(g_ezCtx.window, 1);
}

int ezGetWidth(void) {
	return g_ezCtx.winWidth;
}

int ezGetHeight(void) {
	return g_ezCtx.winHeight;
}

// Callbacks

void ezKeyHook(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (g_ezCtx.keyFun) {
		g_ezCtx.keyFun(key, action);
	}
}

void ezSetKeyCallback(EZkeyfun fn) {
	g_ezCtx.keyFun = fn;
	glfwSetKeyCallback(g_ezCtx.window, ezKeyHook);
}

void ezSetOutOfMemoryCallback(EZmemerrfun fn) {
	g_ezCtx.memErrFun = fn;
}

// Object Functions

EZobject* ezCreateRect(double x0, double y0, double x1, double y1) {
	EZobject* obj = malloc(sizeof(EZobject));

	if (obj == NULL) {
		fprintf(stderr, "Ran out of heap memory!\n");
		int shouldExit = 1;

		if (g_ezCtx.memErrFun) {
			shouldExit = g_ezCtx.memErrFun();
		}

		if (shouldExit) {
			exit(EZ_OUT_OF_HEAP_MEMORY_ERROR_CODE);
		}

		return NULL;
	}

	obj->r = 1.0f;
	obj->b = 1.0f;
	obj->g = 1.0f;

	// todo this is bad. *dont* do this
	glGenBuffers(1, &obj->vbo);
	glGenBuffers(1, &obj->ibo);

	const float vertices[12] = {
		x0, y1,
		x0, y0,
		x1, y0,
		x1, y1
	};

	glBindBuffer(GL_ARRAY_BUFFER, &obj->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	
	const int indices[6] = {
		0, 2, 1, /* clockwise |\ */
		0, 3, 2 /* clockwise \| */
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, &obj->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

	// (location = 0) in vec2 pos
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 0, 0);
	glEnableVertexAttribArray(0);

	return obj;
}

void ezColour(EZobject* object, float r, float g, float b) {
	object->r = r;
	object->g = g;
	object->b = b;
}

void ezDelete(EZobject* object) {
	// delete from GL memory
	glDeleteBuffers(GL_ARRAY_BUFFER, object->vbo);
	glDeleteBuffers(GL_ELEMENT_ARRAY_BUFFER, object->ibo);

	// free from heap
	free(object);
}

// Draw Functions

void ezDraw(EZobject *object) {
	glBindBuffer(GL_ARRAY_BUFFER, &object->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, &object->ibo);

	glUniform3f(glGetUniformLocation(g_ezCtx.shaderProgram, "colour"), object->r, object->g, object->b);

	// todo move this to separate method?
	int err = glGetError();

	if (err) {
		printf("OpenGL Error %d\n", err);
	}

	//glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

//===============
//    Main
//=============

// Taken from another project of mine
// Checks the given shader for errors.
// If errors are found, the error log is displayed, the window is destroyed, GLFW is shut down, and the program terminates with status -1.
static void ezCheckShaderErrors(const GLFWwindow* window, const int shader, const char* shaderType);

int main(void) {
	printf("Yeef.\n");

	// startup
	if (!glfwInit()) {
		printf("GLFW Failed to Initialise.");
		return EZ_GLFW_INIT_ERROR_CODE;
	}

	g_ezCtx.window = glfwCreateWindow(500, 500, "Window", NULL, NULL);
	glfwMakeContextCurrent(g_ezCtx.window);

	if (glewInit() != GLEW_OK) {
		printf("GLEW Failed to Initialise.");
		glfwDestroyWindow(g_ezCtx.window);
		glfwTerminate();
		return EZ_GLEW_INIT_ERROR_CODE;
	}

	// shaders
	printf("Compiling Shaders.\n");

	const char* vertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec2 pos;\n"

		"uniform vec2 window_size;\n"

		"void main() {\n"
		// map from 0,0,WIDTH,HEIGHT to -1,1,-1,1.
		//"  vec2 half_size = window_size * 0.5;\n"
		"  gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);\n"
		"}";

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	ezCheckShaderErrors(g_ezCtx.window, vertexShader, "vertex");

	const char* fragmentShaderSource =
		"#version 330 core\n"

		"uniform vec3 colour;\n"

		"void main() {\n"// TODO put back colour
		"  gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
		"}";

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	ezCheckShaderErrors(g_ezCtx.window, fragmentShader, "fragment");

	printf("Linking Shaders.\n");

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	int success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (!success) {
		printf("Shader Link Error");
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf(": %s\n", infoLog);
		glfwDestroyWindow(g_ezCtx.window);
		glfwTerminate();
		return EZ_LINK_ERROR_CODE;
	}

	// clean up memory
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Use the Shader
	glUseProgram(shaderProgram);
	g_ezCtx.shaderProgram = shaderProgram;

	// configure view port default & null default callbacks
	ezSize(500, 500);
	g_ezCtx.keyFun = NULL;
	g_ezCtx.memErrFun = NULL;

	// Set Up
	if (setup() != EZ_OK) {
		glfwDestroyWindow(g_ezCtx.window);
		glfwTerminate();
		return EZ_GENERIC_ERROR_CODE;
	}

	printf("Initialised Successfully. Starting Main Loop.\n");

	// main lööp

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	while (!glfwWindowShouldClose(g_ezCtx.window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		draw();

		glfwSwapBuffers(g_ezCtx.window);
		glfwPollEvents();
	}

	glfwDestroyWindow(g_ezCtx.window);
	glfwTerminate();
	return EZ_SUCCESS_ERROR_CODE;
}

// Checks the given shader for errors.
// If errors are found, the error log is displayed, the window is destroyed, GLFW is shut down, and the program terminates with status -1.
static void ezCheckShaderErrors(const GLFWwindow* window, const int shader, const char* shaderType) {
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		printf("%s Shader Compile Error", shaderType);

		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf(": %s\n", infoLog);

		glfwDestroyWindow(window);
		glfwTerminate();
		exit(EZ_SHADER_ERROR_CODE);
	}
}