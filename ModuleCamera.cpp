#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "Application.h"
#include "Math/TransformOps.h"
#include "Geometry/Frustum.h"
#include "Globals.h"

#define MAX_KEYS 300


ModuleCamera::ModuleCamera()
{
	position = float3(0.0f, 3.0f, 6.0f);
	target = float3(0.0f, 0.0f, 0.0f);
}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	LOG("Creating Camera context");
	bool ret = true;
	LookAt(position, target, float3::unitY);
	CalcProjMatrix(16.0f / 9.0f, 0.1f, 100.0f);

	return ret;
}

//Catch input through moduleInput and do some actions
update_status ModuleCamera::Update()
{
	float deltaTime = App->GetDeltaTime();
	const float cameraSpeed = 1.0f;

	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		MoveCamera(-position * cameraSpeed * deltaTime);
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) 
		MoveCamera(position * cameraSpeed * deltaTime);
	/*if (App->GetModuleInput()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		MoveCamera(-viewMatrix.Col3(2).xyz() * cameraSpeed * deltaTime);
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		MoveCamera(viewMatrix.Col3(2).xyz() * cameraSpeed * deltaTime);
	*/
	return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp()
{
	return true;
}

//Comtrol movement of the camera. It seems that the camera is flipping and looking to the opsite site. That's the reason why sometimes it sees only black
void ModuleCamera::MoveCamera(const float3& movement)
{

	position += movement;

	LookAt(position, target, float3::unitY);
}

void ModuleCamera::LookAt(const float3& eye, const float3& target, const float3& up)
{
	float3 forward = (target - eye).Normalized();
	float3 right = forward.Cross(up).Normalized();
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


