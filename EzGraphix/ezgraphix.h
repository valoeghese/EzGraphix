//
// The core header for EzGraphix.
// Contains the functions for all things graphics.
// The implementation of these functions can be found in ezgraphix.c
// 
// For the accompanying mathematics utility header, see ezmaths.h
//
// Author: Mekal Covic
//

#pragma once

// Include gl stuff
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef __cplusplus
extern "C" {
#endif

// ======================
// Constants & Data Types
// ======================

#define EZ_OK 1
#define EZ_CANCEL 0

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

// Mark the window as ready to exit at the end of the frame.
void ezSetShouldClose(void);

// Gets the window width
int ezGetWidth(void);

// Gets the window height
int ezGetHeight(void);

// Gets the error code of the latest OpenGL error. If there was no error, returns 0.
// If you receive an error code, search it up on the internet to see what it means and hope to gosh it's helpful.
int ezGetOpenGLError(void);

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
void ezSetMouseMoveFunction(EZmousefun function);

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
void ezSetOutOfMemoryFunction(EZmemerrfun function);

// ===============
// Image Functions
// ===============

// Loads the image into GPU memory from the given file
// If sharing your program with others, make sure to distribute your images with it.
// The images are relative to the folder the exe is in (same as if you're using fopen and stuff)
int ezLoadImage(const char* fileName);

// Frees the image from GPU memory.
// The image can no longer be used after freeing it.
void ezFreeImage(int image);

// ================
// Object Functions
// ================

// Creates a rectangle object of the given width and height
// Positioned from the bottom left.
// EZ objects are allocated on the heap and in GL memory, so make sure to delete them via ezDelete() when you're done with them
EZobject* ezCreateRect(float width, float height);

// Creates a circle object of the given radius
// Positioned from the centre.
// EZ objects are allocated on the heap and in GL memory, so make sure to delete them via ezDelete() when you're done with them
EZobject* ezCreateCircle(float radius);

// Sets the anchor position of an object.
// This determines where on the object its "position" should refer to.
// The default depends on which function you use create your object.
//
// Parameters:
//   x = the 'x' position of the anchor, as a proportion of the width, relative to the bottom left of the object.
//       For example, 0.5 at the horizontal centre.
//   y = the 'y' position of the anchor, as a proportion of the height, relative to the bottom right of the object.
//       For example, 0.5 at the vertical centre.
void ezAnchor(EZobject* object, float x, float y);

// Moves an object to the given position
void ezMove(EZobject* object, float x, float y);

// Resizes an object to the given width and height
void ezResize(EZobject* object, float width, float height);

// Sets the colour of an object
void ezColour(EZobject* object, float r, float g, float b);

// Sets the radius of the edge fillet of an object.
// Should be <= half the smallest dimension of the object.
void ezFilletRadius(EZobject* object, float radius);

// Sets the texture image of this object
// Use the id of an image loaded with ezLoadImage for an image
// Use 0 to represent no texture
void ezTexture(EZobject* object, int image);

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