#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "Application.h"
#include "Math/TransformOps.h"
#include "Geometry/Frustum.h"
#include "Globals.h"

ModuleCamera::ModuleCamera()
{
	position = float3(0.0f, 3.0f, 6.0f);
	target = float3(0.0f, 0.0f, 0.0f);
	forward = -float3::unitZ;
	up = float3::unitY;
	right = float3::unitX;
}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	LOG("Creating Camera context");
	bool ret = true;
	LookAt(position, target, up);
	CalcProjMatrix(16.0f / 9.0f, 0.1f, 100.0f);

	return ret;
}

//Catch input through moduleInput and do some actions
update_status ModuleCamera::Update()
{
	float deltaTime = App->GetDeltaTime();
	float cameraSpeed = 2.0f;

	//detect if shift key is pressed, then multiply speed movement
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT ||
		App->GetModuleInput()->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT)
		cameraSpeed *= 2.0f;

	//Movement
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		position += forward * cameraSpeed * deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) 
		position += -forward * cameraSpeed * deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)//rotation focused ATM (not what we want?)
		position += up * cameraSpeed * deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)//rotation focused ATM (not what we want?)
		position += -up * cameraSpeed * deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		position += -right * cameraSpeed * deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		position += right * cameraSpeed * deltaTime;

	LookAt(position, position + forward, up);

	
	return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp()
{
	return true;
}


void ModuleCamera::LookAt(const float3& eye, const float3& target, const float3& up)
{
	forward = (target - eye).Normalized();
	right = forward.Cross(up).Normalized();
	float3 up_corrected = right.Cross(forward).Normalized();

	float4x4 camera_matrix = float4x4::identity;
	camera_matrix.SetCol3(0, right);
	camera_matrix.SetCol3(1, up_corrected);
	camera_matrix.SetCol3(2, -forward);

	float4x4 translation_matrix = float4x4::Translate(-eye);
	viewMatrix = camera_matrix.Transposed() * translation_matrix;

}

void ModuleCamera::CalcProjMatrix(const float aspectRatio, const float nearPlane, const float farPlane)
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

	projMatrix = frustum.ProjectionMatrix();
	
}


