#include "ezgraphix.h"

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

// enable optimus!
// https://stackoverflow.com/questions/6036292/select-a-graphic-device-in-windows-opengl
_declspec(dllexport) DWORD NvOptimusEnablement = 1;
_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

// these functions must be declared
int setup(void); // called for setup
void draw(void); // called every frame
void cleanup(void); // called at the end to clean up memory used

struct EzGlobalContext {
	GLFWwindow* window;
	EZkeyfun keyFun;
	EZmousefun mouseFun;
	EZclickfun clickFun;
	EZresizefun resizeFun;
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
	// internal OpenGL objects
	int ibo;
	int vbo;
	// Colour
	float r;
	float g;
	float b;
	// Position
	float x;
	float y;
	// Centre Position
	float centreX;
	float centreY;
	// Fillet Radius
	float filletRadius;
};

// Window

void ezTitle(const char* title) {
	glfwSetWindowTitle(g_ezCtx.window, title);
}

void ezDisplaySize(const int width, const int height) {
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

// Callbacks: Impl (GLFW event handlers)

void ezKeyHook(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (g_ezCtx.keyFun) {
		g_ezCtx.keyFun(key, action);
	}
}

void ezResizeHook(GLFWwindow* window, int width, int height) {
	ezDisplaySize(width, height);

	if (g_ezCtx.resizeFun) {
		g_ezCtx.resizeFun(width, height);
	}
}

void ezMouseHook(GLFWwindow* window, double mouseX, double mouseY) {
	if (g_ezCtx.mouseFun) {
		g_ezCtx.mouseFun(mouseX, (double)g_ezCtx.winHeight - mouseY);
	}
}

void ezClickHook(GLFWwindow* window, int button, int action, int mods) {
	if (g_ezCtx.clickFun) {
		g_ezCtx.clickFun(button, action);
	}
}

// Callbacks: API

void ezSetKeyFunction(EZkeyfun function) {
	g_ezCtx.keyFun = function;
	glfwSetKeyCallback(g_ezCtx.window, ezKeyHook);
}

void ezSetMouseMoveFunction(EZmousefun function) {
	g_ezCtx.mouseFun = function;
	glfwSetCursorPosCallback(g_ezCtx.window, ezMouseHook);
}

void ezSetClickFunction(EZclickfun function) {
	g_ezCtx.clickFun = function;
	glfwSetMouseButtonCallback(g_ezCtx.window, ezClickHook);
}

void ezSetResizeFunction(EZresizefun function) {
	g_ezCtx.resizeFun = function;
	glfwSetWindowSizeCallback(g_ezCtx.window, ezResizeHook);
}

void ezSetOutOfMemoryFunction(EZmemerrfun function) {
	g_ezCtx.memErrFun = function;
}

// Object Functions

EZobject* ezCreateRect(float width, float height) {
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

	// Set colour to white
	obj->r = 1.0f;
	obj->b = 1.0f;
	obj->g = 1.0f;

	// Set position
	obj->x = 0.0f;
	obj->y = 0.0f;

	// Set data used for fillet
	obj->centreX = width;
	obj->centreY = height;
	obj->filletRadius = 0;

	// this used to be bad. *do* do this
	// (i used to create an object for every draw call)
	glGenBuffers(1, &(obj->vbo));
	glGenBuffers(1, &(obj->ibo));

	const float vertices[12] = {
		0, height,
		0, 0,
		width, 0,
		width, height
	};

	glBindBuffer(GL_ARRAY_BUFFER, &(obj->vbo));
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_DYNAMIC_DRAW);
	
	const int indices[6] = {
		0, 2, 1, /* clockwise |\ */
		0, 3, 2 /* clockwise \| */
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, &(obj->ibo));
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

	// (location = 0) in vec2 pos
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 0, 0);
	glEnableVertexAttribArray(0);

	// unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return obj;
}

void ezMove(EZobject* object, float x, float y) {
	object->x = x;
	object->y = y;
}

void ezResize(EZobject* object, float width, float height) {
	glBindBuffer(GL_ARRAY_BUFFER, &(object->vbo));

	const float vertices[12] = {
		0, height,
		0, 0,
		width, 0,
		width, height
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ezColour(EZobject* object, float r, float g, float b) {
	object->r = r;
	object->g = g;
	object->b = b;
}

void ezDelete(EZobject* object) {
	// delete from GL memory
	glDeleteBuffers(1, &(object->vbo));
	glDeleteBuffers(1, &(object->ibo));

	// free from heap
	free(object);
}

// Draw Functions

void ezBackgroundColour(float r, float g, float b) {
	glClearColor(r, g, b, 1.0f);
}

void ezDraw(EZobject *object) {
	glBindBuffer(GL_ARRAY_BUFFER, &(object->vbo));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, &(object->ibo));

	glUniform3f(glGetUniformLocation(g_ezCtx.shaderProgram, "colour"), object->r, object->g, object->b);
	glUniform2f(glGetUniformLocation(g_ezCtx.shaderProgram, "position"), object->x, object->y);

	// todo move this to separate method?
	int err = glGetError();

	if (err) {
		printf("OpenGL Error %d\n", err);
	}

	//glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
		"layout(location = 0) in vec2 dimensions;\n"

		"uniform vec2 window_size;\n"
		"uniform vec2 position;\n"

		"void main() {\n"
		// map from 0,0,WIDTH,HEIGHT to -1,1,-1,1.
		"  vec2 half_size = window_size * 0.5;\n"
		"  gl_Position = vec4(((dimensions + position) / half_size) - 1, 0.0, 1.0);\n"
		"}";

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	ezCheckShaderErrors(g_ezCtx.window, vertexShader, "vertex");

	const char* fragmentShaderSource =
		"#version 330 core\n"

		"uniform vec3 colour;\n"

		"void main() {\n"
		"  gl_FragColor = vec4(colour, 1.0);\n"
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
	ezDisplaySize(500, 500);
	g_ezCtx.keyFun = NULL;
	g_ezCtx.memErrFun = NULL;

	// Default Clear Colour
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Set Up
	if (setup() != EZ_OK) {
		glfwDestroyWindow(g_ezCtx.window);
		glfwTerminate();
		return EZ_GENERIC_ERROR_CODE;
	}

	printf("Initialised Successfully. Starting Main Loop.\n");

	// main lööp

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