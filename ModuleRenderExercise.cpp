#include "ModuleRenderExercise.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"
#include "ModuleDebugDraw.h"
#include "ModuleCamera.h"
#include "ModuleTexture.h"
#include "Application.h"
#include "Model.h"
#include <fstream> 
#include <sstream>
#include <iostream>
#include "glew-2.1.0/include/GL/glew.h"
#include "Math/float4.h"
#include "Math/float3x4.h"
#include "Math/TransformOps.h"
#include "Geometry/Ray.h"
#include "Geometry/Frustum.h"
#include "imgui.h"


ModuleRenderExercise::ModuleRenderExercise()
{
	_vertexShader = "../Vertex_Shader.glsl";
	_fragmentShader = "../Fragment_Shader.glsl";
	_program = new ModuleProgram(_vertexShader, _fragmentShader);
	_objectPosition = float3(0.0f, 1.0f, -1.0f);
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

	_renderer = SDL_CreateRenderer(App->GetWindow()->window, -1, flags);

	if (_renderer == nullptr)
	{
		LOG("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	//Create Model
	_model = new Model();
	
	LOG("Created Initial Model Instance -------------------")
	if (!_model->LoadModel("../Resources/Models/BakerHouse.gltf")) {
		LOG("Error: Could not load the model.");
		ret = false;
	}
	else
		LOG("End Created Initial Model Instance -------------------")

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

	App->GetModuleTexture()->SetFilters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	App->GetModuleTexture()->SetWrapMode(GL_REPEAT, GL_REPEAT);

	glEnable(GL_DEPTH_TEST);

	return ret;

}

update_status ModuleRenderExercise::Update()
{
	//get model, view and projection matrix
	float4x4 model = float4x4::FromTRS(_objectPosition, float4x4::RotateZ(0), float3(1.0f, 1.0f, 1.0f));
	_view = App->GetModuleCamera()->GetViewMatrix();
	_proj = App->GetModuleCamera()->GetProjMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	App->GetModuleDebugDraw()->Draw(_view, _proj, SCREEN_WIDTH, SCREEN_HEIGHT);

	glUseProgram(_id_program); // To avoid overlap with DebugDraw

	GLint modelLoc = glGetUniformLocation(_id_program, "model");
	GLint viewLoc = glGetUniformLocation(_id_program, "view");
	GLint projLoc = glGetUniformLocation(_id_program, "proj");

	if (modelLoc == -1 || viewLoc == -1 || projLoc == -1 ) {
		LOG("Error: Uniforms shaders location not found.");
	}

	glUniformMatrix4fv(modelLoc, 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(viewLoc, 1, GL_TRUE, &_view[0][0]);
	glUniformMatrix4fv(projLoc, 1, GL_TRUE, &_proj[0][0]);

	_model->Draw(_id_program);

	return UPDATE_CONTINUE;
}

update_status ModuleRenderExercise::PostUpdate()
{
	return UPDATE_CONTINUE;
}

bool ModuleRenderExercise::CleanUp()
{
	DestroyModel();

	if (_renderer) {
		SDL_DestroyRenderer(_renderer);
		_renderer = nullptr;
	}

	if (_program)
	{
		delete _program;
		_program = nullptr;
	}

	if (_id_program != 0) {
		glDeleteProgram(_id_program);
		_id_program = 0;
	}
	return true;
}

void ModuleRenderExercise::LoadNewModel(const std::string& fileName)
{
	LOG("Loading new model process -------------------");
	
	DestroyModel();

	_model = new Model();
	if (!_model->LoadModel(fileName))
	{
		LOG("Error: Failed to load model: %s", fileName.c_str());
		delete _model;
		_model = nullptr;
	}
	else
	{
		LOG("New model loaded successfully: %s", fileName.c_str());
	}

	LOG("End of Loading new model process -------------------");
}

void ModuleRenderExercise::DrawPropertiesWindow()
{
	if (_model) {

		if (ImGui::CollapsingHeader("Geometry"))
		{
			ImGui::Text("Triangle Count: %zu", _model->GetTotalTriangles());
			ImGui::Text("Vertex Count: %zu", _model->GetTotalVertices());
		}
		if (ImGui::CollapsingHeader("Texture"))
		{
			ImGui::Text("Texture Size: ");
			ImGui::BulletText("Width: %d", App->GetModuleTexture()->GetWidth());
			ImGui::BulletText("Height: %d", App->GetModuleTexture()->GetHeight());

		}
	}
	else {
		ImGui::Text("No model loaded.");
	}

}

void ModuleRenderExercise::DestroyModel()
{
	LOG("Destroying actual model ...")
	if (_model) {
		delete _model;
		_model = nullptr;
	}
}