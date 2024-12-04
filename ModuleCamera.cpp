#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "Application.h"
#include "Math/TransformOps.h"
#include "Globals.h"

ModuleCamera::ModuleCamera()
{
	frustum.pos = float3(0.0f, 3.0f, 6.0f);
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;
	
	yaw = -90.0f;
	pitch = 0.0f;
}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	LOG("Creating Camera context");
	bool ret = true;
	LookAt(frustum.pos, float3(0.0f, 0.0f, 0.0f), frustum.up);
	CalcProjMatrix(16.0f / 9.0f, 0.1f, 100.0f);

	return ret;
}

//Catch input through moduleInput and do some actions
update_status ModuleCamera::Update()
{
	float deltaTime = App->GetDeltaTime();
	float cameraSpeed = 2.0f;
	float rotationSpeed = 20.0f;

	//detect if shift key is pressed, then multiply speed movement
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT ||
		App->GetModuleInput()->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT) {

		cameraSpeed *= 3.0f;
		rotationSpeed *= 3.0f;
	}

	//Movement
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		frustum.pos += frustum.front * cameraSpeed * deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) 
		frustum.pos -= frustum.front * cameraSpeed * deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)//rotation focused ATM (not what we want?)
		frustum.pos += frustum.up * cameraSpeed * deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)//rotation focused ATM (not what we want?)
		frustum.pos -= frustum.up * cameraSpeed * deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		frustum.pos -= frustum.WorldRight() * cameraSpeed * deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		frustum.pos += frustum.WorldRight() * cameraSpeed * deltaTime;


	//Rotation
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		pitch += rotationSpeed * deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		pitch -= rotationSpeed * deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		yaw += rotationSpeed * deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		yaw -= rotationSpeed * deltaTime;


	//Limit pitch
	if(pitch > 89.0f) pitch = 89.0f;
	else if (pitch < -89.0f) pitch = -89.0f;

	float3 newForward;
	newForward.x = cosf(DEG_TO_RAD(yaw)) * cosf(DEG_TO_RAD(pitch));
	newForward.y = sinf(DEG_TO_RAD(pitch));
	newForward.z = sinf(DEG_TO_RAD(yaw)) * cosf(DEG_TO_RAD(pitch));

	frustum.front = newForward.Normalized();
	frustum.up = frustum.WorldRight().Cross(frustum.front).Normalized();

	
	LookAt(frustum.pos, frustum.pos + frustum.front, frustum.up);
	
	return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp()
{
	return true;
}


void ModuleCamera::LookAt(const float3& eye, const float3& target, const float3& up)
{
	float3 front = (target - eye).Normalized();
	float3 right = front.Cross(up).Normalized();
	float3 up_corrected = right.Cross(front).Normalized();

	float4x4 camera_matrix = float4x4::identity;
	camera_matrix.SetCol3(0, right);
	camera_matrix.SetCol3(1, up_corrected);
	camera_matrix.SetCol3(2, -front);


	float4x4 translation_matrix = float4x4::Translate(-eye);
	rotationMatrix = camera_matrix.Transposed();
	viewMatrix = rotationMatrix * translation_matrix;

}

void ModuleCamera::CalcProjMatrix(const float aspectRatio, const float nearPlane, const float farPlane)
{
	frustum.type = FrustumType::PerspectiveFrustum;

	frustum.nearPlaneDistance = nearPlane;
	frustum.farPlaneDistance = farPlane;
	frustum.verticalFov = M_PI / 4.0f; //45 degrees
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * aspectRatio);

	projMatrix = frustum.ProjectionMatrix();
	
}


