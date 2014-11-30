#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <cassert>
#include "stdio.h"
#include "Core.h"
#include "Camera.h"
#include "Input.h"

static const float scCameraTranslationSpeed = 1.2f;
static const float scCameraRotationSpeed = 0.02f;

Camera::Camera()
{
    eye = XMVectorSet(0,0,0,0);
    theta = 0;
    phi = 0;
    BuildMatrices();
}

void Camera::Update()
{
    XMMATRIX rotation = XMMatrixRotationRollPitchYaw(phi, theta, 0);  // rotation around x axis, y axis, z axis
    XMVECTOR cross = rotation.r[0];
    XMVECTOR up    = rotation.r[1];
    XMVECTOR look  = rotation.r[2];
    
    if (Input::key_down['W'])
        eye += look * scCameraTranslationSpeed;
    if (Input::key_down['S'])
        eye -= look * scCameraTranslationSpeed;
    if (Input::key_down['A'])
        eye -= cross * scCameraTranslationSpeed;
    if (Input::key_down['D'])
        eye += cross * scCameraTranslationSpeed;
    if (Input::key_down['Q'])
        eye += XMVectorSet(0,1,0,0) * scCameraTranslationSpeed;
    if (Input::key_down['E'])
        eye -= XMVectorSet(0,1,0,0) * scCameraTranslationSpeed;

	float boost = 1 + Input::gamepad_right_trigger*10;

	eye += cross * scCameraTranslationSpeed * Input::gamepad_left_x * boost;
	eye += look * scCameraTranslationSpeed * Input::gamepad_left_y * boost;
	eye -= XMVectorSet(0,1,0,0) * scCameraTranslationSpeed * Input::gamepad_left_shoulder * boost;
	eye += XMVectorSet(0,1,0,0) * scCameraTranslationSpeed * Input::gamepad_right_shoulder * boost;
	theta += scCameraRotationSpeed * Input::gamepad_right_x * boost;
	phi -= scCameraRotationSpeed * Input::gamepad_right_y * boost;

    if (Input::mousebutton_right_down)
    {
        theta += scCameraRotationSpeed * Input::mouse_dx;
        phi += scCameraRotationSpeed * Input::mouse_dy;
    }

    BuildMatrices();
}

void Camera::MoveTo(XMVECTOR new_eye, float new_theta, float new_phi)
{
    eye = new_eye;
    theta = new_theta;
    phi = new_phi;

    BuildMatrices();
}

void Camera::BuildMatrices()
{
    XMMATRIX rotation = XMMatrixRotationRollPitchYaw(phi, theta, 0);  // rotation around x axis, y axis, z axis
    XMVECTOR look = rotation.r[2];
    
    XMVECTOR up = {0,1,0};
    view = XMMatrixLookToLH(eye, look, up);

    float fov = 45 / 360.0f * 2 * XM_PI;
    float aspect = 4/3.0f;
    float nearz = 0.1f;
    float farz = 2000;
    proj = XMMatrixPerspectiveFovLH(fov, aspect, nearz, farz);
}
