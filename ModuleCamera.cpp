#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "Application.h"
#include "Math/TransformOps.h"
#include "Globals.h"
#include "imgui.h"
#include <algorithm>

ModuleCamera::ModuleCamera()
{
	frustum.pos = float3(0.0f, 3.0f, 6.0f);
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;
	
	yaw = -90.0f;
	pitch = 0.0f;
	aspectRatio = 16.0f / 9.0f;
	nearPlane = 0.1f;
	farPlane = 100.0f;

}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	LOG("Creating Camera context");
	bool ret = true;
	LookAt(frustum.pos, float3(0.0f, 0.0f, 0.0f), frustum.up);
	CalcProjMatrix();

	return ret;
}

//Catch input through moduleInput and do some actions
update_status ModuleCamera::Update()
{
	deltaTime = App->GetDeltaTime();
	cameraSpeed = 4.0f;
	rotationSpeed = 60.0f;		
	sensitivity = 20.0f;


	//detect if shift key is pressed, then multiply speed movement
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT ||
		App->GetModuleInput()->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT) {

		cameraSpeed *= 3.0f;
		rotationSpeed *= 3.0f;
		sensitivity *= 2.0f;
	}

	//Movement Drag and Zoom
	MovKeyboardController();
	MovMouseController();
	
	pitch = max(-89.0f, min(89.0f, pitch));

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

void ModuleCamera::MovKeyboardController()
{
	//Deactivate control movement when ImGUI captures keyboard

	if (ImGui::GetIO().WantCaptureKeyboard)
		return;
	
	//Movement
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		frustum.pos += frustum.front * cameraSpeed * deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		frustum.pos -= frustum.front * cameraSpeed * deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
		frustum.pos += frustum.up * cameraSpeed * deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
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
}

void ModuleCamera::MovMouseController()
{
	//Deactivate control movement when ImGUI captures mouse
	if (ImGui::GetIO().WantCaptureMouse)
		return;

	Point mouseDelta = App->GetModuleInput()->GetMouseMotion();

	if (App->GetModuleInput()->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT ||
		App->GetModuleInput()->GetMouseButtonDown(SDL_BUTTON_MIDDLE) == KEY_REPEAT) 
	{
		frustum.pos += frustum.WorldRight() * -mouseDelta.x * sensitivity * deltaTime;
		frustum.pos += frustum.up * mouseDelta.y * sensitivity * deltaTime;
	}


	if (App->GetModuleInput()->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		if (App->GetModuleInput()->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) //zoom
		{
			sensitivity = 80.0f;
			float zoomFactor = 0.0f;
			if (mouseDelta.y > 0 || mouseDelta.x < 0) zoomFactor = -sensitivity * deltaTime;
			else if (mouseDelta.y < 0 || mouseDelta.x > 0) zoomFactor = sensitivity * deltaTime;

			frustum.pos += frustum.front * zoomFactor;
		}
		else //rotation
		{
			const float threshold = 0.1f; 
			float deltaX = (fabs(mouseDelta.x) > threshold) ? mouseDelta.x : 0.0f;
			float deltaY = (fabs(mouseDelta.y) > threshold) ? mouseDelta.y : 0.0f;

			float sensitivityFactor = 10.0f; 

			yaw += deltaX * sensitivityFactor * rotationSpeed * deltaTime;;
			pitch -= deltaY * sensitivityFactor * rotationSpeed * deltaTime;

		}
	}


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

void ModuleCamera::CalcProjMatrix()
{
	frustum.type = FrustumType::PerspectiveFrustum;

	frustum.nearPlaneDistance = nearPlane;
	frustum.farPlaneDistance = farPlane;
	frustum.verticalFov = M_PI / 4.0f; //45 degrees
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * aspectRatio);

	projMatrix = frustum.ProjectionMatrix();
	
}


