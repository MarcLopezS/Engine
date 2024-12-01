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

	GLint status;
	glGetProgramiv(id_program, GL_LINK_STATUS, &status);
	
	if (status == GL_FALSE)
	{
		GLchar strInfoLog[1024] = { 0 };
		glGetProgramInfoLog(id_program, sizeof(strInfoLog) - 1, nullptr, strInfoLog);
		LOG("Shader linking error: %s", strInfoLog);
	}
	
	return ret;

}

update_status ModuleRenderExercise::Update()
{
	float4x4 model = float4x4::FromTRS(float3(2.0f, 0.0f, 0.0f), float4x4::RotateZ(static_cast<float>(M_PI) / 4.0f), float3(2.0f, 1.0f, 1.0f));
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

float4x4 ModuleRenderExercise::LookAt(const float3& eye, const float3& target, const float3& up)
{
	float3 forward = (target - eye).Normalized();
	float3 right = forward.Cross(up).Normalized();
	float3 up_corrected = right.Cross(forward).Normalized();

	float4x4 camera_matrix = float4x4::identity;
	camera_matrix.SetCol3(0, right);
	camera_matrix.SetCol3(1, up_corrected);
	camera_matrix.SetCol3(2, -forward);

	float4x4 translation_matrix = float4x4::Translate(-eye);
	float4x4 view_matrix = camera_matrix.Transposed() * translation_matrix;

	return view_matrix;

}

float4x4 ModuleRenderExercise::CalcProjectionMatrix(const float aspectRatio, const float nearPlane, const float farPlane)
{
	Frustum frustum;
	frustum.type = FrustumType::PerspectiveFrustum;

	frustum.pos = float3::zero; 
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;

	frustum.nearPlaneDistance = nearPlane;
	frustum.farPlaneDistance = farPlane;
	frustum.verticalFov = M_PI / 4.0f; //45 degrees
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * aspectRatio);

	return frustum.ProjectionMatrix();
}
//function called one time at destruction of vertex buffer
void ModuleRenderExercise::DestroyVBO()
{
	glDeleteBuffers(1, &vbo);
}
