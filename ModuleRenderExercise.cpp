#include "ModuleRenderExercise.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"
#include "Application.h"
#include <fstream> 
#include <sstream>
#include <iostream>
#include "SDL.h"
#include "glew-2.1.0/include/GL/glew.h"

ModuleRenderExercise::ModuleRenderExercise()
{
	//mirar esta parte
	vertexShader = "../Vertex_Shader.glsl";
	fragmentShader = "../Fragment_Shader.glsl";

	program = new ModuleProgram(vertexShader, fragmentShader);
}

ModuleRenderExercise::~ModuleRenderExercise()
{
}

bool ModuleRenderExercise::Init()
{
	LOG("Creating Renderer context");
	bool ret = true;
	Uint32 flags = 0;

	if (VSYNC == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
	}

	renderer = SDL_CreateRenderer(App->GetWindow()->window, -1, flags);

	if (renderer == nullptr)
	{
		LOG("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	CreateTriangleVBO();
	
	std::string vertexShaderContent = program->readShaderSource(vertexShader);
	std::string fragmentShaderContent = program->readShaderSource(fragmentShader);
	id_program = program->CreateProgram(vertexShaderContent, fragmentShaderContent);
	
	return ret;

}

update_status ModuleRenderExercise::Update()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(id_program);
	RenderVBO();

	return UPDATE_CONTINUE;
}

bool ModuleRenderExercise::CleanUp()
{
	LOG("Destroying VBO");
	DestroyVBO();
	return true;
}

void ModuleRenderExercise::RenderVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

//Function called one time at creation of vertex buffer
void ModuleRenderExercise::CreateTriangleVBO()
{
	float vertex_data[] = {
		-1.0f, -1.0f,  0.0f,
		 1.0f, -1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

}

//function called one time at destruction of vertex buffer
void ModuleRenderExercise::DestroyVBO()
{
	glDeleteBuffers(1, &vbo);
}
