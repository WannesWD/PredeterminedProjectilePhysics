#include "pch.h"
#include "SimpleShapes.h"
#include "Math.h"
#include "DataTypes.h"
#include "Utils.h"
#include "Effect.h"
#include "Mesh.h"
#include "Camera.h"

Plane::Plane(ID3D11Device* pDevice, Effect* pEffect, dae::Vector3 origin, dae::Vector3 rotation) :
	m_Origin{ origin },
	m_Rotation{ rotation }
{
	std::vector<dae::Vertex> vertices{};
	std::vector<uint32_t> indices{};
	dae::Utils::ParseOBJ("Resources/Plane.obj", vertices, indices);

	m_pPlaneMesh = new Mesh{ pDevice,pEffect,vertices,indices };
	dae::Matrix worldMatrix{ m_pPlaneMesh->GetWorldMatrix() * dae::Matrix::CreateRotation(m_Rotation) * dae::Matrix::CreateTranslation(m_Origin) };
	m_pPlaneMesh->UpdateWorldMatrix(worldMatrix);
	
	dae::Vector3 a{ worldMatrix.TransformPoint(vertices[0].position) };
	m_OtherVertex = a;
	dae::Vector3 ab{ a - worldMatrix.TransformPoint(vertices[1].position) };
	dae::Vector3 ac{ a - worldMatrix.TransformPoint(vertices[2].position) };
	dae::Vector3 cross{ dae::Vector3::Cross(ac.Normalized(), ab.Normalized()).Normalized()};
	m_Normal = cross;
	m_CartesianCords = dae::Vector4{ cross , -(cross.x * a.x + cross.y * a.y + cross.z * a.z) };
}

Plane::~Plane()
{
	delete m_pPlaneMesh;
}

void Plane::Render(ID3D11DeviceContext* pDevicecontext)
{
	m_pPlaneMesh->Render(pDevicecontext, m_WorldViewProjectionMatrix);
	for (Sphere* sphere : m_DebugSpheres)
	{
		sphere->Render(pDevicecontext);
	}
}

void Plane::Update(const dae::Timer* pTime, Camera* pCamera)
{
	m_WorldViewProjectionMatrix = m_pPlaneMesh->GetWorldMatrix() * pCamera->GetViewMatrx() * pCamera->GetProjectionMatrix();
	for (Sphere* sphere : m_DebugSpheres)
	{
		sphere->Update(pTime, pCamera);
	}
}

dae::Vector3 Plane::GetNormal()
{
	return m_Normal;
}

dae::Vector3 Plane::GetOtherVertex()
{
	return m_OtherVertex;
}

dae::Vector4 Plane::GetCartesianCords()
{
	return m_CartesianCords;
}

dae::Vector3 Plane::GetOrigin()
{
	return m_Origin;
}

Sphere::Sphere(ID3D11Device* pDevice, Effect* pEffect, dae::Vector3 origin, float radius) :
	m_StartPos{ origin },
	m_Origin{ origin },
	m_Radius{ radius }
{
	if (m_Vertices.size() <=0)
	{
		dae::Utils::ParseOBJ("Resources/Sphere.obj", m_Vertices, m_Indices);
	}

	m_pSphereMesh = new Mesh{ pDevice,pEffect,m_Vertices,m_Indices };

	m_pSphereMesh->UpdateWorldMatrix(m_pSphereMesh->GetWorldMatrix() * 
				dae::Matrix::CreateScale(dae::Vector3{ radius,radius,radius }) * dae::Matrix::CreateTranslation(m_Origin));
}

Sphere::~Sphere()
{
	delete m_pSphereMesh;
}

void Sphere::Render(ID3D11DeviceContext* pDevicecontext)
{
	m_pSphereMesh->Render(pDevicecontext, m_WorldViewProjectionMatrix);
}

void Sphere::Update(const dae::Timer* pTime, Camera* pCamera)
{
	m_WorldViewProjectionMatrix = m_pSphereMesh->GetWorldMatrix() * pCamera->GetViewMatrx() * pCamera->GetProjectionMatrix();
}

void Sphere::SetPosition(const dae::Vector3& newPos)
{
	m_Origin = newPos;

	m_pSphereMesh->UpdateWorldMatrix(dae::Matrix::CreateScale(dae::Vector3{ m_Radius,m_Radius,m_Radius }) *
		dae::Matrix::CreateTranslation(m_Origin));
}

dae::Vector3 Sphere::GetStartPos()
{
	return m_StartPos;
}

void Sphere::SetStartpos(const dae::Vector3& newStartPos)
{
	m_StartPos = newStartPos;
}

