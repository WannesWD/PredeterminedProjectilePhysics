#pragma once
#include "SimpleShapes.h"
#include "PhysicsCalculator.h"

class Effect;

class Projectile
{
public:
	Projectile(ID3D11Device* pDevice, Effect* pEffect, dae::Vector3 origin, float radius);
	~Projectile();

	Projectile(const Projectile&) = delete;
	Projectile(Projectile&&) noexcept = delete;
	Projectile& operator=(const Projectile&) = delete;
	Projectile& operator=(Projectile&&) noexcept = delete;

	void Update(const dae::Timer* pTimer, Camera* pCamera);
	void RenderUpdate(const dae::Timer* pTimer, Camera* pCamera);
	void SetMovement(int movementIdx, MovementEquation movement,const dae::Vector3& nextStartPos, float minT);
	void SetEquationsCount(int equationCount);
	void Render(ID3D11DeviceContext* pDeviceContext);
	bool BeenShot() const;
	void Shoot();
	bool IsReady() const;
private:
	Sphere* m_pShape{};
	dae::Vector3 m_InitPos{};

	std::vector<MovementEquation> m_Movement{};
	std::vector<dae::Vector3> m_StartPositions{};
	std::vector<float> m_IntersectionT{};

	float m_TotalTime{};

	int m_CurrentBounceCount{};
	int m_MaxEquations{};

	bool m_Shot{};
	bool m_Ready{};
};

