#pragma once
#include "Math.h"

class Camera final
{
public:
	Camera(const dae::Vector3& origin, float aspectRatio, float fovAngle = 90.f,
			float nearPlane = .1f, float farPlane = 100.f, float cameraSpeed = 20.f, float mouseSens = 0.3f);

	Camera(const Camera& other) = delete;
	Camera(Camera&& other) noexcept = delete;
	Camera& operator=(const Camera& other) = delete;
	Camera& operator=(Camera&& other) noexcept = delete;

	void Update(const dae::Timer* pTimer);

	const dae::Matrix& GetInvViewMatrix();
	const dae::Matrix& GetViewMatrx();
	const dae::Matrix& GetProjectionMatrix();
	void SetPosition(const dae::Vector3& newOrigin);
private:
	void CalculateViewMatrix();
	void CalculateProjectionMatrix(float aspectRatio);

	dae::Vector3 m_Origin{};
	const float m_CameraSpeed{};
	const float m_MouseSens{};
	float m_FovAngle{};
	float m_Fov{};
	float m_Far{};
	float m_Near{};

	dae::Vector3 m_Forward{ dae::Vector3::UnitZ };
	dae::Vector3 m_Up{ dae::Vector3::UnitY };
	dae::Vector3 m_Right{ dae::Vector3::UnitX };

	float m_TotalPitch{};
	float m_TotalYaw{};

	dae::Matrix m_ProjectionMatrix{};
	dae::Matrix m_InvViewMatrix{};
	dae::Matrix m_ViewMatrix{};
};

