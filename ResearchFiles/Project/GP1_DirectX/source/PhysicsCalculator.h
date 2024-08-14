#pragma once
#include <vector>
class Plane;
class Sphere;
class Effect;
class Projectile;

struct MovementEquation
{
	float x;
	float y0;
	float y1;
	float z;

	float launchAngleB{};
	dae::Vector3 calculateMovement(const dae::Vector3& startPos, float t);
};

class PhysicsCalculator
{
public:
	static PhysicsCalculator* GetPhysiscsCalculator();
	~PhysicsCalculator() = default;

	PhysicsCalculator(const PhysicsCalculator&) = delete;
	PhysicsCalculator(PhysicsCalculator&&) noexcept = delete;
	PhysicsCalculator& operator=(const PhysicsCalculator&) = delete;
	PhysicsCalculator& operator=(PhysicsCalculator&&) noexcept = delete;


	void CalculatePhysics(ID3D11Device* pDevice, Effect* pEffect, Projectile* pProjectile,
							const dae::Vector3& startPos, std::vector<Sphere*>& projectileTrace,
							const std::vector<Plane*>& worldPlanes,
							float launchForce,float launchAngleA,float launchAngleB);
	void UpdateProjectilePosition(const dae::Timer* pTimer, Sphere* projectile, bool& finished);
	float GetGravity();
	void SetGravity(float gravity);
private:
	PhysicsCalculator() = default;

	void SetPhysics(ID3D11Device* pDevice, Effect* pEffect, Projectile* pProjectile,
		const dae::Vector3& startPos, std::vector<Sphere*>& projectileTrace,
		const std::vector<Plane*>& worldPlanes,
		float launchForce, float launchAngleA, float launchAngleB);

	void SetTrace(ID3D11Device* pDevice, Effect* pEffect,const dae::Vector3 startPos, 
					std::vector<Sphere*>& projectileTrace, float minT);

	float m_Gravity{ -9.5f };
	
	std::vector<MovementEquation> m_ProjectileMovementEquation{};
	std::vector<float> m_IntersectionT{};
	std::vector<dae::Vector3> m_StartPositions{};
	dae::Vector3 m_InitPos{};
	int instanceCount{};

	float m_TotalTime{};

	const int m_MaxBounceCount{ 25 };
	int m_MaxNumTracers{ 50 };
	int m_CurrentBounceCount{ 0 };
	int m_CurrentTracerCount{ 0 };
	int m_CurrentEquationCount{ 0 };
	int m_MaxEquations{ 0 };
};

