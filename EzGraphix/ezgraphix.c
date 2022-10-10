#include "ezgraphix.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
	// Used for the anchor thing. As a proportion of width/height.
	float anchorX;
	float anchorY;
	// Dimensions
	float width;
	float height;
	// Fillet Radius
	float filletRadius;
	// Texture
	int texture;
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

void ezSetShouldClose(void) {
	glfwSetWindowShouldClose(g_ezCtx.window, 1);
}

int ezGetWidth(void) {
	return g_ezCtx.winWidth;
}

int ezGetHeight(void) {
	return g_ezCtx.winHeight;
}

// Callbacks: Impl (GLFW event handlers)

static void ezKeyHook(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (g_ezCtx.keyFun) {
		g_ezCtx.keyFun(key, action);
	}
}

static void ezResizeHook(GLFWwindow* window, int width, int height) {
	ezDisplaySize(width, height);

	if (g_ezCtx.resizeFun) {
		g_ezCtx.resizeFun(width, height);
	}
}

static void ezMouseHook(GLFWwindow* window, double mouseX, double mouseY) {
	if (g_ezCtx.mouseFun) {
		g_ezCtx.mouseFun(mouseX, (double)g_ezCtx.winHeight - mouseY);
	}
}

static void ezClickHook(GLFWwindow* window, int button, int action, int mods) {
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
			cleanup();
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

	obj->anchorX = 0.0f;
	obj->anchorY = 0.0f;

	// Set data used for fillet
	obj->width = width;
	obj->height = height;
	obj->filletRadius = 0;

	// default texture
	obj->texture = 0;

	// the code for this used to be bad, but isn't anymore. *do* do this
	// (i used to create an object for every draw call)
	glGenBuffers(1, &(obj->vbo));
	glGenBuffers(1, &(obj->ibo));

	// Vertex Coords	UV Coords
	const float vertices[16] = {
		0, height,		0, 1,
		0, 0,			0, 0,
		width, 0,		1, 0,
		width, height,	1, 1
	};

	glBindBuffer(GL_ARRAY_BUFFER, &(obj->vbo));
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_DYNAMIC_DRAW);
	
	const int indices[6] = {
		0, 2, 1, /* clockwise |\ */
		0, 3, 2 /* clockwise \| */
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, &(obj->ibo));
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

	// unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return obj;
}

EZobject* ezCreateCircle(float radius) {
	// create object via ezCreateRect
	EZobject* result = ezCreateRect(radius * 2, radius * 2);
	// anchor in centre
	ezAnchor(result, 0.5f, 0.5f);
	// set fillet
	ezFilletRadius(result, radius);
	// return
	return result;
}

void ezAnchor(EZobject* object, float x, float y) {
	object->anchorX = x;
	object->anchorY = y;
}

void ezMove(EZobject* object, float x, float y) {
	object->x = x;
	object->y = y;
}

void ezResize(EZobject* object, float width, float height) {
	// set properties
	object->width = width;
	object->height = height;

	// update buffer data
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

void ezFilletRadius(EZobject* object, float radius) {
	object->filletRadius = radius;
}

void ezTexture(EZobject* object, int image) {
	object->texture = image;
}

void ezDelete(EZobject* object) {
	// delete from GL memory
	glDeleteBuffers(1, &(object->vbo));
	glDeleteBuffers(1, &(object->ibo));

	// free from heap
	free(object);
}

// Image Functions

int ezLoadImage(const char* fileName) {
	// ===========
	// STEP 1: create opengl tex obj 
	// ============

	// generate opengl texture object
	int texture;
	glGenTextures(1, &texture);
	// bind texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// only use NEAREST NEIGHBOUR!
	// if you want your textures interpolated feel free to change this to GL_LINEAR / GL_LINEAR_MIPMAP_LINEAR tho
	// could make interpolation method a parameter in a future version, or make another function to set it
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	// ===========
	// STEP 2: actually load image
	// ============
	
	// load imgae from file
	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* data = stbi_load(fileName, &width, &height, &channels, 0);

	// load image data to opengl texture object and gen mipmap
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// free loaded image data
	stbi_image_free(data);

	// unbind texture
	// probably not necessary
	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

void ezFreeImage(int image) {
	glDeleteTextures(1, &image);
}

// Draw Functions

void ezBackgroundColour(float r, float g, float b) {
	glClearColor(r, g, b, 1.0f);
}

void ezDraw(EZobject *object) {
	glBindBuffer(GL_ARRAY_BUFFER, &(object->vbo));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, &(object->ibo));

	// Attach buffer data
	// (location = 0) in vec2 pos
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, sizeof(float) * 0);
	glEnableVertexAttribArray(0);
	// (location = 1) in vec2 uv
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, sizeof(float) * 2);
	glEnableVertexAttribArray(1);

	// Set uniforms

	glUniform3f(glGetUniformLocation(g_ezCtx.shaderProgram, "colour"), object->r, object->g, object->b);
	glUniform2f(glGetUniformLocation(g_ezCtx.shaderProgram, "position"), object->x - object->anchorX * object->width, object->y - object->anchorY * object->height);

	glUniform2f(glGetUniformLocation(g_ezCtx.shaderProgram, "dimensions"), object->width, object->height);
	glUniform1f(glGetUniformLocation(g_ezCtx.shaderProgram, "filletRadius"), object->filletRadius);

	// Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object->texture);
	// 0 is treated as false, all else is true. Thus, we can define "hasTexture" as the id of the texture
	glUniform1i(glGetUniformLocation(g_ezCtx.shaderProgram, "hasTexture"), object->texture);
	// gotta set the sampler to use active texture 0
	glUniform1i(glGetUniformLocation(g_ezCtx.shaderProgram, "textureSampler"), 0);

	//glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// TODO compound objects.
//void ezDrawArrays(EZobject* objects, int length) {
//	for (int i = 0; i < length; i++) {
//		ezDraw(&(objects[i]));
//	}
//}

int ezGetOpenGLError(void) {
	// todo move this to separate method?
	return glGetError();
}

//===============
//    Main
//=============

// Taken from another project of mine
// Checks the given shader for errors.
// If errors are found, the error log is displayed, the window is destroyed, GLFW is shut down, and the program terminates with status -1.
static void ezCheckShaderErrors(const GLFWwindow* window, const int shader, const char* shaderType);

int main(void) {
	printf("Starting Up...\n");

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
		"layout(location = 0) in vec2 vertexPosition;\n"
		"layout(location = 1) in vec2 uv;\n"

		"out vec2 posPass;\n"
		"out vec2 uvPass;\n"

		"uniform vec2 window_size;\n"
		"uniform vec2 position;\n"

		"void main() {\n"
		"  posPass = vertexPosition;\n" // position relative to the shape. Will be interpolated for each pixel when passed to the fragment shader
		"  uvPass = uv;\n"
		// map from 0,0,WIDTH,HEIGHT to -1,1,-1,1.
		"  vec2 half_size = window_size * 0.5;\n"
		"  gl_Position = vec4(((vertexPosition + position) / half_size) - 1, 0.0, 1.0);\n"
		"}";

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	ezCheckShaderErrors(g_ezCtx.window, vertexShader, "vertex");

	const char* fragmentShaderSource =
		"#version 330 core\n"

		"in vec2 posPass;\n"
		"in vec2 uvPass;\n"

		"uniform vec3 colour;\n"

		"uniform vec2 dimensions;\n"
		"uniform float filletRadius;\n"

		"uniform bool hasTexture;\n"
		"uniform sampler2D textureSampler;\n"

		"void main() {\n"
		// detect edge boxes that encompass the fillet curves,
		// then if within one of those boxes do a squrared-distance calculation to the inside corner
		// this creates a smooth arc around a corner
		// ... so long as the fillet radius is <= half length of shortest dimension
		"  if (posPass.x <= filletRadius) {\n"
		"    float dx = posPass.x - filletRadius;\n"

		"    if (posPass.y <= filletRadius) {\n"
		"      float dy = posPass.y - filletRadius;\n"
		"      if (dx * dx + dy * dy > filletRadius * filletRadius) discard;\n"
		"    }\n"
		"    else if (posPass.y >= dimensions.y - filletRadius) {\n"
		"      float dy = posPass.y - (dimensions.y - filletRadius);\n"
		"      if (dx * dx + dy * dy > filletRadius * filletRadius) discard;\n"
		"    }\n"
		"  }\n"
		// and the other two corners...
		"  else if (posPass.x >= dimensions.x - filletRadius) {\n"
		"    float dx = posPass.x - (dimensions.x - filletRadius);\n"

		"    if (posPass.y <= filletRadius) {\n"
		"      float dy = posPass.y - filletRadius;\n"
		"      if (dx * dx + dy * dy > filletRadius * filletRadius) discard;\n"
		"    }\n"
		"    else if (posPass.y >= dimensions.y - filletRadius) {\n"
		"      float dy = posPass.y - (dimensions.y - filletRadius);\n"
		"      if (dx * dx + dy * dy > filletRadius * filletRadius) discard;\n"
		"    }\n"
		"  }\n"

		// If Texture, use that
		"  if (hasTexture) {\n"
		"    gl_FragColor = texture(textureSampler, uvPass);\n"
		"  } else {\n"
		// Else, use colour
		"    gl_FragColor = vec4(colour, 1.0);\n"
		"  }\n"
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