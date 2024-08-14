#include "pch.h"
#include "Projectile.h"

Projectile::Projectile(ID3D11Device* pDevice, Effect* pEffect, dae::Vector3 origin, float radius)
{
	m_pShape = new Sphere{ pDevice,pEffect,origin,radius };
	m_InitPos = origin;

}

Projectile::~Projectile()
{
	delete m_pShape;
}

void Projectile::Update(const dae::Timer* pTimer, Camera* pCamera)
{	
	if (m_Shot && (m_CurrentBounceCount < m_MaxEquations))
	{
		//int 🥖{ 69 };
		//float 👀{ 234 };
		//std::string 👉👈🥺{ "For me??" };
		//m_Shot = false;
		m_TotalTime += pTimer->GetElapsed();
		m_pShape->SetPosition(m_Movement[m_CurrentBounceCount].calculateMovement(m_pShape->GetStartPos(), m_TotalTime));
		if (m_TotalTime >= m_IntersectionT[m_CurrentBounceCount])
		{
			m_TotalTime = 0;
			m_pShape->SetStartpos(m_StartPositions[m_CurrentBounceCount]);
			++m_CurrentBounceCount;
		}
	}
	else
	{
		m_pShape->SetPosition(m_InitPos);
		m_pShape->SetStartpos(m_InitPos);
		m_CurrentBounceCount = 0;
		m_Shot = false;
		m_Ready = false;
	}
}

void Projectile::RenderUpdate(const dae::Timer* pTimer, Camera* pCamera)
{
	m_pShape->Update(pTimer, pCamera);
}

void Projectile::SetMovement(int movementIdx, MovementEquation movement, const dae::Vector3& nextStartPos, float minT)
{
	if (movementIdx >= m_Movement.size())
	{
		m_Movement.push_back(movement);
		m_IntersectionT.push_back(minT);
		m_StartPositions.push_back(nextStartPos);
	}
	else
	{
		m_Movement[movementIdx] = movement;
		m_IntersectionT[movementIdx] = minT;
		m_StartPositions[movementIdx] = nextStartPos;
	}
}

void Projectile::SetEquationsCount(int equationCount)
{
	m_MaxEquations = equationCount;
	m_Ready = true;
}

void Projectile::Render(ID3D11DeviceContext* pDeviceContext)
{
	m_pShape->Render(pDeviceContext);
}

bool Projectile::BeenShot() const
{
	return m_Shot;
}

void Projectile::Shoot()
{
	m_Shot = true;
}

bool Projectile::IsReady() const
{
	return m_Ready;
}
