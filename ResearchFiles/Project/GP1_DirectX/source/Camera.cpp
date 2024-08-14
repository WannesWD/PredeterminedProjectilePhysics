#include "pch.h"
#include "Camera.h"

Camera::Camera(const dae::Vector3& origin, float aspectRatio,float fovAngle, 
	float nearPlane, float farPlane, float cameraSpeed, float mouseSens) :
	m_Origin{ origin },
	m_FovAngle{ fovAngle },
	m_CameraSpeed{ cameraSpeed },
	m_MouseSens{ mouseSens },
	m_Fov{ tanf((fovAngle * dae::TO_RADIANS) / 2.f) },
	m_Near{ nearPlane },
	m_Far{ farPlane }
{
	CalculateProjectionMatrix(aspectRatio);
}

void Camera::Update(const dae::Timer* pTimer)
{
	const float deltaTime = pTimer->GetElapsed();

	//Camera Update Logic

	//Keyboard Input
	const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
	dae::Vector3 zDirection{ 0.f,0.f,0.f };
	dae::Vector3 xDirection{ 0.f,0.f,0.f };
	dae::Vector3 yDirection{ 0.f,0.f,0.f };

	if (pKeyboardState[SDL_SCANCODE_W])
		zDirection = m_Forward.Normalized();
	if (pKeyboardState[SDL_SCANCODE_S])
		zDirection = -m_Forward.Normalized();

	if (pKeyboardState[SDL_SCANCODE_D])
		xDirection = m_Right.Normalized();
	if (pKeyboardState[SDL_SCANCODE_A])
		xDirection = -m_Right.Normalized();

	if (pKeyboardState[SDL_SCANCODE_SPACE])
		yDirection = m_Up.Normalized();
	if (pKeyboardState[SDL_SCANCODE_LSHIFT])
		yDirection = -m_Up.Normalized();



	//Mouse Input

	dae::Matrix pitchMatrix;
	bool mousePosChange{ false };
	int mouseX{}, mouseY{};
	const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);



	if (mouseState == SDL_BUTTON_X1)			//lmb
	{
		m_TotalYaw += (mouseX * m_MouseSens) * dae::TO_RADIANS;

		m_TotalPitch -= (mouseY * m_MouseSens) * dae::TO_RADIANS;

		mousePosChange = true;
	}
	else if (mouseState == SDL_BUTTON_LEFT)
	{
		zDirection = m_Forward.Normalized() * float(-mouseY);

		m_TotalYaw += (mouseX * m_MouseSens) * dae::TO_RADIANS;

		mousePosChange = true;
	}
	else if (mouseState == SDL_BUTTON_X2)		//lmb + rmb
	{
		m_Origin.y += mouseY / 2;

		mousePosChange = true;
	}

	m_Origin += ((zDirection + xDirection + yDirection) * m_CameraSpeed * deltaTime);

	if (mousePosChange)
	{
		dae::Matrix yawMatrix{ dae::Matrix::CreateRotationY(m_TotalYaw) };
		dae::Matrix pitchMatrix{ dae::Matrix::CreateRotationX(m_TotalPitch) };

		dae::Matrix finalRotation{ pitchMatrix * yawMatrix };
		m_Forward = finalRotation.TransformVector(dae::Vector3::UnitZ);
		m_Forward.Normalize();
	}

	//Update Matrices
	CalculateViewMatrix();
}

const dae::Matrix& Camera::GetInvViewMatrix()
{
	return m_InvViewMatrix;
}

const dae::Matrix& Camera::GetViewMatrx()
{
	return m_ViewMatrix;
}

const dae::Matrix& Camera::GetProjectionMatrix()
{
	return m_ProjectionMatrix;
}

void Camera::SetPosition(const dae::Vector3& newOrigin)
{
	m_Origin = newOrigin;
}

void Camera::CalculateViewMatrix()
{
	m_Right = dae::Vector3::Cross(dae::Vector3::UnitY, m_Forward).Normalized();
	m_Up = dae::Vector3::Cross(m_Forward, m_Right).Normalized();

	m_ViewMatrix = dae::Matrix{ {m_Right,0},{m_Up,0},{m_Forward,0},{m_Origin,1} }.Inverse();
}

void Camera::CalculateProjectionMatrix(float aspectRatio)
{
	m_ProjectionMatrix = dae::Matrix::CreatePerspectiveFovLH(m_Fov, aspectRatio, m_Near, m_Far);
	int test{};
}
