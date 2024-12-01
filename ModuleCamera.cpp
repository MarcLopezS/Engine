#include "ModuleCamera.h"
#include "Math/TransformOps.h"
#include "Geometry/Frustum.h"


ModuleCamera::ModuleCamera()
{
}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	LOG("Creating Camera context");
	bool ret = true;
	LookAt(float3(0.0f, 3.0f, 6.0f), float3(0.0f, 0.0f, 0.0f), float3::unitY);
	CalcProjMatrix(16.0f / 9.0f, 0.1f, 100.0f);

	return ret;
}

update_status ModuleCamera::Update()
{
	return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp()
{
	return true;
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


