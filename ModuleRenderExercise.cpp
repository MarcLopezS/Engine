#include "ModuleRenderExercise.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"
#include "ModuleDebugDraw.h"
#include "ModuleCamera.h"
#include "ModuleTexture.h"
#include "Application.h"
#include <fstream> 
#include <sstream>
#include <iostream>
#include "glew-2.1.0/include/GL/glew.h"
#include "Math/float4.h"
#include "Math/float3x4.h"
#include "Math/TransformOps.h"
#include "Geometry/Ray.h"
#include "Geometry/Frustum.h"


ModuleRenderExercise::ModuleRenderExercise()
{
	_vertexShader = "../Vertex_Shader.glsl";
	_fragmentShader = "../Fragment_Shader.glsl";
	_program = new ModuleProgram(_vertexShader, _fragmentShader);
}

ModuleRenderExercise::~ModuleRenderExercise()
{
	delete _program;
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

	_renderer = SDL_CreateRenderer(App->GetWindow()->window, -1, flags);

	if (_renderer == nullptr)
	{
		LOG("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	CreateQuadVBO();

	
	if (!App->GetModuleTexture()->LoadTexture("../Resources/Test-image-Baboon.jpeg")) {
		LOG("Error: Could not load texture");
		return false;
	}
	
	LOG("Reading Shaders");
	std::string vertexShaderContent = _program->readShaderSource(_vertexShader);
	std::string fragmentShaderContent = _program->readShaderSource(_fragmentShader);
	_id_program = _program->CreateProgram(vertexShaderContent, fragmentShaderContent);

	GLint status;
	glGetProgramiv(_id_program, GL_LINK_STATUS, &status);
	
	if (status == GL_FALSE)
	{
		GLchar strInfoLog[1024] = { 0 };
		glGetProgramInfoLog(_id_program, sizeof(strInfoLog) - 1, nullptr, strInfoLog);
		LOG("Shader linking error: %s", strInfoLog);
	}
	glEnable(GL_DEPTH_TEST);

	return ret;

}

update_status ModuleRenderExercise::Update()
{
	float4x4 model = float4x4::FromTRS(float3(0.0f, 1.0f, -1.0f), float4x4::RotateZ(0), float3(1.0f, 1.0f, 1.0f));
	_view = App->GetModuleCamera()->GetViewMatrix();
	_proj = App->GetModuleCamera()->GetProjMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	App->GetModuleDebugDraw()->Draw(_view, _proj, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	glUseProgram(_id_program); // Restore Quad shader

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	App->GetModuleTexture()->Bind(0);

	GLint modelLoc = glGetUniformLocation(_id_program, "model");
	GLint viewLoc = glGetUniformLocation(_id_program, "view");
	GLint projLoc = glGetUniformLocation(_id_program, "proj");

	if (modelLoc == -1 || viewLoc == -1 || projLoc == -1) {
		LOG("Error: Uniforms shaders location not found.");
	}

	glUniformMatrix4fv(modelLoc, 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(viewLoc, 1, GL_TRUE, &_view[0][0]);
	glUniformMatrix4fv(projLoc, 1, GL_TRUE, &_proj[0][0]);
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	return UPDATE_CONTINUE;
}

update_status ModuleRenderExercise::PostUpdate()
{
	return UPDATE_CONTINUE;
}

bool ModuleRenderExercise::CleanUp()
{
	LOG("Destroying VBO");
	DestroyVBO();
	return true;
}


//Function called one time at creation of vertex buffer
void ModuleRenderExercise::CreateQuadVBO()
{
	float vertex_data[] = {
		//position			 //UV
		-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f,  0.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 2, 
		2, 3, 0   
	};

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER,sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

	glGenBuffers(1, &_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

}

//function called one time at destruction of vertex buffer
void ModuleRenderExercise::DestroyVBO()
{
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ebo);

}
