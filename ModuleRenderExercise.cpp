#include "ModuleRenderExercise.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"
#include "ModuleDebugDraw.h"
#include "ModuleCamera.h"
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
	
	LOG("Reading Shaders");
	std::string vertexShaderContent = program->readShaderSource(vertexShader);
	std::string fragmentShaderContent = program->readShaderSource(fragmentShader);
	id_program = program->CreateProgram(vertexShaderContent, fragmentShaderContent);

	GLint status;
	glGetProgramiv(id_program, GL_LINK_STATUS, &status);
	
	if (status == GL_FALSE)
	{
		GLchar strInfoLog[1024] = { 0 };
		glGetProgramInfoLog(id_program, sizeof(strInfoLog) - 1, nullptr, strInfoLog);
		LOG("Shader linking error: %s", strInfoLog);
	}
	glEnable(GL_DEPTH_TEST);

	return ret;

}

update_status ModuleRenderExercise::Update()
{
	float4x4 model = float4x4::FromTRS(float3(0.0f, 1.0f, -1.0f), float4x4::RotateZ(static_cast<float>(M_PI) / 4.0f), float3(1.0f, 1.0f, 1.0f));
	view = App->GetModuleCamera()->GetViewMatrix();
	proj = App->GetModuleCamera()->GetProjMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	App->GetModuleDebugDraw()->Draw(view, proj, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	glUseProgram(id_program); // Restore triangle shader
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	GLint modelLoc = glGetUniformLocation(id_program, "model");
	GLint viewLoc = glGetUniformLocation(id_program, "view");
	GLint projLoc = glGetUniformLocation(id_program, "proj");

	if (modelLoc == -1 || viewLoc == -1 || projLoc == -1) {
		LOG("Error: Uniforms shaders location not found.");
	}

	glUniformMatrix4fv(modelLoc, 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(viewLoc, 1, GL_TRUE, &view[0][0]);
	glUniformMatrix4fv(projLoc, 1, GL_TRUE, &proj[0][0]);
	
	glDrawArrays(GL_TRIANGLES, 0, 3);

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

}

//function called one time at destruction of vertex buffer
void ModuleRenderExercise::DestroyVBO()
{
	glDeleteBuffers(1, &vbo);
}
