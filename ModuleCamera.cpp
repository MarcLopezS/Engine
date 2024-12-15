#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "Application.h"
#include "ModuleRenderExercise.h"
#include "Model.h"
#include "Math/TransformOps.h"
#include "Globals.h"
#include "imgui.h"
#include <algorithm>

ModuleCamera::ModuleCamera()
{
	_frustum.pos = float3(0.0f, 3.0f, 6.0f);
	_frustum.front = -float3::unitZ;
	_frustum.up = float3::unitY;
	
	_yaw = -90.0f;
	_pitch = 0.0f;
	_aspectRatio = 16.0f / 9.0f;
	_nearPlane = 0.1f;
	_farPlane = 100.0f;
	_ignoreInput = false;

}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	LOG("Creating Camera context");
	bool ret = true;
	LookAt(_frustum.pos, float3(0.0f, 0.0f, 0.0f), _frustum.up);
	CalcProjMatrix();

	return ret;
}

//Catch input through moduleInput and do some actions
update_status ModuleCamera::Update()
{
	_deltaTime = App->GetDeltaTime();
	_cameraSpeed = 4.0f;
	_rotationSpeed = 60.0f;		
	_sensitivity = 1.0f;


	//detect if shift key is pressed, then multiply speed movement
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT ||
		App->GetModuleInput()->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT) {

		_cameraSpeed *= 3.0f;
		_sensitivity *= 2.0f;
	}

	//Movement Drag and Zoom
	MovKeyboardController();
	MovMouseController();
	
	RecalculateCameraAxes();
	
	if (!_ignoreInput)
		LookAt(_frustum.pos, _frustum.pos + _frustum.front, _frustum.up);
	
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
		_frustum.pos += _frustum.front * _cameraSpeed * _deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		_frustum.pos -= _frustum.front * _cameraSpeed * _deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
		_frustum.pos += _frustum.up * _cameraSpeed * _deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
		_frustum.pos -= _frustum.up * _cameraSpeed * _deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		_frustum.pos -= _frustum.WorldRight() * _cameraSpeed * _deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		_frustum.pos += _frustum.WorldRight() * _cameraSpeed * _deltaTime;
	
	//Rotation
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		_pitch += _rotationSpeed * _deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		_pitch -= _rotationSpeed * _deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		_yaw += _rotationSpeed * _deltaTime;
	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		_yaw -= _rotationSpeed * _deltaTime;

	if (App->GetModuleInput()->GetKey(SDL_SCANCODE_F) == KEY_REPEAT)
	{
		float3 modelCenter = App->GetModuleRenderExcercise()->GetModel()->GetModelCenter();
		float modelDiagonal = App->GetModuleRenderExcercise()->GetModel()->GetDiagonalModel();
		float distanceFactor = 2.0f;

		float3 objectPosition = modelCenter + (App->GetModuleRenderExcercise()->GetObjectPosition());

		_frustum.pos = objectPosition - _frustum.front * modelDiagonal * distanceFactor;

		RecalculateCameraAxes();

		LookAt(_frustum.pos, objectPosition, _frustum.up);

		_ignoreInput = true;
	}
	else {
		_ignoreInput = false;
	}

}

void ModuleCamera::MovMouseController()
{
	//Deactivate control movement when ImGUI captures mouse
	if (ImGui::GetIO().WantCaptureMouse)
		return;

	Point mouseDelta = App->GetModuleInput()->GetMouseMotion();
	int mouse_wheel = App->GetModuleInput()->GetMouseWheel();

	//TODO:Check unstable movement.
	if (App->GetModuleInput()->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT ||
		App->GetModuleInput()->GetMouseButtonDown(SDL_BUTTON_MIDDLE) == KEY_REPEAT) 
	{
		_frustum.pos += _frustum.WorldRight() * (-mouseDelta.x) * _sensitivity * _deltaTime;
		_frustum.pos += _frustum.up * mouseDelta.y * _sensitivity * _deltaTime;
	}

	if (mouse_wheel > 0)
		_frustum.pos += _frustum.front * _sensitivity;

	else if(mouse_wheel < 0)
		_frustum.pos -= _frustum.front * _sensitivity;

	if (App->GetModuleInput()->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		//TODO:Check unstable movement.
		if (App->GetModuleInput()->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) //Zoom in/out
		{
			float zoomFactor = 0.0f;
			_sensitivity = 0.2f;
			
			if (mouseDelta.y > 0 || mouseDelta.x < 0) 
				zoomFactor = -_sensitivity;

			else if (mouseDelta.y < 0 || mouseDelta.x > 0)
				zoomFactor = _sensitivity;

			_frustum.pos += _frustum.front * zoomFactor;
		}
		else //Rotation
		{
			float rotationSensitivity = 0.5f;
			float threshold = 0.1f;

			_yaw += (float)mouseDelta.x * rotationSensitivity;
			_pitch -= (float)mouseDelta.y * rotationSensitivity;

			_pitch = max(-89.0f, min(89.0f, _pitch));

		}
	}
}

void ModuleCamera::RecalculateCameraAxes()
{
	float3 newForward;
	newForward.x = cosf(DEG_TO_RAD(_yaw)) * cosf(DEG_TO_RAD(_pitch));
	newForward.y = sinf(DEG_TO_RAD(_pitch));
	newForward.z = sinf(DEG_TO_RAD(_yaw)) * cosf(DEG_TO_RAD(_pitch));

	_frustum.front = newForward.Normalized();

	float3 worldUp = float3(0.0f, 1.0f, 0.0f);
	float3 right = worldUp.Cross(_frustum.front).Normalized();
	float3 up = _frustum.front.Cross(right).Normalized();

	_frustum.up = up;
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
	_rotationMatrix = camera_matrix.Transposed();
	_viewMatrix = _rotationMatrix * translation_matrix;

}

void ModuleCamera::CalcProjMatrix()
{
	_frustum.type = FrustumType::PerspectiveFrustum;

	_frustum.nearPlaneDistance = _nearPlane;
	_frustum.farPlaneDistance = _farPlane;
	_frustum.verticalFov = M_PI / 4.0f; //45 degrees
	_frustum.horizontalFov = 2.f * atanf(tanf(_frustum.verticalFov * 0.5f) * _aspectRatio);

	_projMatrix = _frustum.ProjectionMatrix();
	
}


