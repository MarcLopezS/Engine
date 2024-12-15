#pragma once
#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);


void log(const char file[], int line, const char* format, ...);

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Deletes an array of buffers
#define RELEASE_ARRAY( x ) \
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }

// Convert from Degrees to radians
#define DEG_TO_RAD(degrees) ((degrees) * M_PI / 180.0f)

// Configuration -----------
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_SIZE 2.0f
#define FULLSCREEN false
#define VSYNC true
#define TITLE "ScareCrow Engine"
#define AUTHOR "Marc Lopez Serrano"
#define LICENSE "Apache-2.0 license"
#define MATERIAL_PATH "../Resources/Models/"

//version of GLSL
#define GLSL_VERSION "#version 460"
#define TINY_GLTF_VERSION "v.2.9.3"
#define MATHGEOLIB_VERSION "v.1.5"
