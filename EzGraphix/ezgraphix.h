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

struct _EZobject;
typedef struct _EZobject EZobject;

// Window Functions
void ezTitle(const char* title);
void ezSize(const int width, const int height);
void ezClose(void);

int ezGetWidth(void);
int ezGetHeight(void);

// Callback Functions
void ezSetKeyCallback(EZkeyfun keyFn);
void ezSetOutOfMemoryCallback(EZmemerrfun keyFn);

// Object Functions
// EZ objects are allocated on the heap and in GL memory, so make sure to delete them via ezDelete() when you're done with them
EZobject* ezCreateRect(float x0, float y0, float x1, float y1);
void ezColour(EZobject* object, float r, float g, float b);
void ezDelete(EZobject* object);

// Draw Functions
void ezDraw(EZobject* object);

#ifdef __cplusplus
}
#endif