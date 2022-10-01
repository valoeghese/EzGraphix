#pragma once

// Include gl stuff
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef __cplusplus
extern "C" {
#endif

// Setup Values and Types
#define EZ_OK 1
#define EZ_ERR 0

#define EZ_GENERIC_ERROR_CODE -1
#define EZ_SUCCESS_ERROR_CODE 0
#define EZ_SHADER_ERROR_CODE 10
#define EZ_LINK_ERROR_CODE 11
#define EZ_GLFW_INIT_ERROR_CODE 69
#define EZ_GLEW_INIT_ERROR_CODE 420
#define EZ_OUT_OF_HEAP_MEMORY_ERROR_CODE 690

typedef void (*EZkeyfun)(int key, int action);
typedef int (*EZmemerrfun)(void);
typedef void (*EZresizefun)(int width, int height);
typedef void (*EZmousefun)(double mouseX, double mouseY);
typedef void (*EZclickfun)(int button, int action);

struct _EZobject;
typedef struct _EZobject EZobject;

// ================
// Window Functions
// ================

// Sets the title of the window
void ezTitle(const char* title);

// Sets the size of the window
void ezDisplaySize(const int width, const int height);

// Closes the application immediately and exits
void ezClose(void);

// Gets the window width
int ezGetWidth(void);

// Gets the window height
int ezGetHeight(void);

// ==================
// Callback Functions
// ==================

// Sets the function to run when a key is pressed
// Must follow the pattern:
// void functionName(int key, int action)
void ezSetKeyFunction(EZkeyfun function);

// Sets the function to run when the mouse is moved on the window
// Must follow the pattern:
// void functionName(double mouseX, double mouseY)
void ezSetMouseFunction(EZmousefun function);

// Sets the function to run when the mouse is moved on the window
// Must follow the pattern:
// void functionName(int button, int action)
void ezSetClickFunction(EZclickfun function);

// Sets the function to run when the window is resized
// DO NOT CALL ezDisplaySize FROM THIS
// Must follow the pattern:
// void functionName(int width, int height)
void ezSetResizeFunction(EZresizefun function);

// Sets the function to run when a new object cannot be allocated on the heap
// Does not handle other out of memory issues.
// Must follow the pattern:
// int functionName(void)
void ezSetOutOfMemoryCallback(EZmemerrfun function);

// ================
// Object Functions
// ================

// Creates a rectangle object of the given width and height
// EZ objects are allocated on the heap and in GL memory, so make sure to delete them via ezDelete() when you're done with them
EZobject* ezCreateRect(float width, float height);

// Moves an object to the given position
void ezMove(EZobject* object, float x, float y);

// Resizes an object to the given width and height
void ezResize(EZobject* object, float width, float height);

// Sets the colour of an object
void ezColour(EZobject* object, float r, float g, float b);

// Deletes an object from memory
void ezDelete(EZobject* object);

// ==============
// Draw Functions
// ==============

// Sets the background colour
void ezBackgroundColour(float r, float g, float b);

// Draws an object
void ezDraw(EZobject* object);

#ifdef __cplusplus
}
#endif