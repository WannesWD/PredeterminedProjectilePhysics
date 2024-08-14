#pragma once
#include "DataTypes.h"

class Effect;
class Mesh;
class Camera;
class Sphere;
class Plane
{
public:
	Plane(ID3D11Device* pDevice,Effect* pEffect, dae::Vector3 origin, dae::Vector3 rotation);
	~Plane();

	Plane(const Plane&) = delete;
	Plane(Plane&&) noexcept = delete;
	Plane& operator=(const Plane&) = delete;
	Plane& operator=(Plane&&) noexcept = delete;

	void Render(ID3D11DeviceContext* pDevicecontext);
	void Update(const dae::Timer* pTime,Camera* pCamera);
	dae::Vector3 GetNormal();
	dae::Vector3 GetOtherVertex();
	dae::Vector4 GetCartesianCords();
	dae::Vector3 GetOrigin();
private:
	dae::Vector3 m_Origin{};
	dae::Vector3 m_Rotation{};
	dae::Vector3 m_Normal{};
	dae::Matrix m_WorldViewProjectionMatrix{};
	Mesh* m_pPlaneMesh{};
	dae::Vector4 m_CartesianCords{};
	std::vector<Sphere*> m_DebugSpheres{};
	dae::Vector3 m_OtherVertex{};
};

class Sphere
{
public:
	Sphere(ID3D11Device* pDevice, Effect* pEffect, dae::Vector3 origin, float radius);
	~Sphere();

	Sphere(const Sphere&) = delete;
	Sphere(Sphere&&) noexcept = delete;
	Sphere& operator=(const Sphere&) = delete;
	Sphere& operator=(Sphere&&) noexcept = delete;

	void Render(ID3D11DeviceContext* pDevicecontext);
	void Update(const dae::Timer* pTime, Camera* pCamera);
	void SetPosition(const dae::Vector3& newPos);

	dae::Vector3 GetStartPos();
	void SetStartpos(const dae::Vector3& newStartPos);
private:
	std::vector<dae::Vertex> m_Vertices{};
	std::vector<uint32_t> m_Indices{};

	dae::Vector3 m_StartPos{};
	dae::Vector3 m_Origin{};
	float m_Radius{};
	Mesh* m_pSphereMesh{};
	dae::Matrix m_WorldViewProjectionMatrix{};
};

