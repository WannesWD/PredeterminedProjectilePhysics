#include "pch.h"
#include "PhysicsCalculator.h"
#include "SimpleShapes.h"
#include "Math.h"
#include "Effect.h"
#include "Projectile.h"

PhysicsCalculator* PhysicsCalculator::GetPhysiscsCalculator()
{
	static PhysicsCalculator* INSTANCE;
	if (!INSTANCE)
	{
		INSTANCE = new PhysicsCalculator{};
	}

	return INSTANCE;
}


void PhysicsCalculator::CalculatePhysics(ID3D11Device* pDevice, Effect* pEffect, Projectile* pProjectile,
						const dae::Vector3& startPos, std::vector<Sphere*>& projectileTrace,
						const std::vector<Plane*>& worldPlanes, float launchForce, float launchAngleA, float launchAngleB)
{
	SetPhysics(pDevice, pEffect, pProjectile, startPos, projectileTrace, worldPlanes, launchForce, launchAngleA, launchAngleB);

	pProjectile->SetEquationsCount(m_CurrentEquationCount);
	m_MaxEquations = m_CurrentEquationCount;
	m_CurrentBounceCount = 0;
	m_CurrentTracerCount = 0;
	m_CurrentEquationCount = 0;
}

void PhysicsCalculator::UpdateProjectilePosition(const dae::Timer* pTimer, Sphere* projectile, bool& finished)
{
	if (m_CurrentBounceCount < m_MaxEquations)
	{
		finished = false;
		m_TotalTime += pTimer->GetElapsed();
		projectile->SetPosition(m_ProjectileMovementEquation[m_CurrentBounceCount].calculateMovement(projectile->GetStartPos(), m_TotalTime));
		if (m_TotalTime >= m_IntersectionT[m_CurrentBounceCount])
		{
			m_TotalTime = 0;
			projectile->SetStartpos(m_StartPositions[m_CurrentBounceCount]);
			++m_CurrentBounceCount;
		}
	}
	else
	{
		projectile->SetPosition(m_InitPos);
		projectile->SetStartpos(m_InitPos);
		m_CurrentBounceCount = 0;
		finished = true;
	}

}

float PhysicsCalculator::GetGravity()
{
	return m_Gravity;
}

void PhysicsCalculator::SetGravity(float gravity)
{
	m_Gravity = gravity;
}

void PhysicsCalculator::SetPhysics(ID3D11Device* pDevice, Effect* pEffect, Projectile* pProjectile, const dae::Vector3& startPos,
									std::vector<Sphere*>& projectileTrace, const std::vector<Plane*>& worldPlanes, 
										float launchForce, float launchAngleA, float launchAngleB)
{
	if (abs(launchForce) < 0.1f)
	{
		return;
	}

	float cosA{ cosf(launchAngleA) };
	float sinA{ sinf(launchAngleA) };
	float cosB{ cosf(launchAngleB) };
	float sinB{ sinf(launchAngleB) };

	MovementEquation ME{};
	ME.x = launchForce * cosA * cosB;
	ME.y0 = launchForce * sinA;
	ME.y1 = m_Gravity / 2;
	ME.z = launchForce * cosA * sinB;

	dae::Vector4 cartesianCords{};
	float a{}, b{}, c{}, d{};

	dae::Vector3 s0{ startPos };
	float A{}, B{}, C{}, D{};

	float t0{};
	float t1{};

	float t{};
	float minT{ FLT_MAX };
	dae::Vector3 planeNormal{};
	dae::Vector3 otherVertex{};

	for (Plane* plane : worldPlanes)
	{
		cartesianCords = plane->GetCartesianCords();
		float epsilon{ 0.001f };
		a = (abs(cartesianCords.x) < epsilon) ? 0.f : cartesianCords.x;
		b = (abs(cartesianCords.y) < epsilon) ? 0.f : cartesianCords.y;
		c = (abs(cartesianCords.z) < epsilon) ? 0.f : cartesianCords.z;
		d = (abs(cartesianCords.w) < epsilon) ? 0.f : cartesianCords.w;

		A = (b * m_Gravity) / 2.f;
		B = launchForce * (a * cosA * cosB + b * sinA + c * cosA * sinB);
		C = d + (a * s0.x) + (b * s0.y) + (c * s0.z);

		if (A == 0)
		{
			t = -C / B;
		}
		else if (B == 0)
		{
			t = sqrt(-C / A);
		}
		else
		{
			D = (B * B) - (4 * A * C);
			if (D <= 0)
				continue;
			float sqrtD{ sqrt(D) };
			float A2{ 2 * A };

			t0 = (-B - sqrtD) / A2;
			t1 = (-B + sqrtD) / A2;
			t0 = (abs(t0) < epsilon) ? 0.f : t0;
			t1 = (abs(t1) < epsilon) ? 0.f : t1;

			t = (t0 > 0 && t1 > 0) ? std::min(t0, t1) : (t0 <= 0 && t1 > 0) ? t1 : (t0 > 0 && t1 <= 0) ? t0 : -1;
			if (t == -1)
				continue;
		}

		t = (abs(t) < epsilon) ? 0.f : t;

		if ((minT-t) > epsilon && t > 0.f)
		{
			minT = t;
			planeNormal = plane->GetNormal();
			otherVertex = plane->GetOtherVertex();
		}
	}


	if (m_CurrentEquationCount >= m_ProjectileMovementEquation.size())
	{
		m_ProjectileMovementEquation.push_back(ME);
		m_IntersectionT.push_back(minT);
		m_StartPositions.push_back(m_ProjectileMovementEquation[m_CurrentBounceCount].calculateMovement(startPos, minT));
	}
	else
	{
		m_ProjectileMovementEquation[m_CurrentEquationCount] = ME;
		m_IntersectionT[m_CurrentEquationCount] = minT;
		m_StartPositions[m_CurrentEquationCount] = m_ProjectileMovementEquation[m_CurrentBounceCount].calculateMovement(startPos, minT);
	}
	dae::Vector3 nextStartPos{ m_ProjectileMovementEquation[m_CurrentBounceCount].calculateMovement(startPos, minT) };
	pProjectile->SetMovement(m_CurrentEquationCount, ME, nextStartPos, minT);

	SetTrace(pDevice, pEffect, startPos, projectileTrace, minT);


	++m_CurrentEquationCount;
	++m_CurrentBounceCount;

	if ( m_CurrentBounceCount < m_MaxBounceCount)
	{
		dae::Vector3 v1{ (launchForce * cosA * cosB) / 1.5f,(launchForce * sinA) + m_Gravity / 2 * minT, (launchForce * cosA * sinB) / 1.5f };
		dae::Vector3 hitPos{ m_StartPositions[m_CurrentBounceCount - 1] };
		dae::Vector3 nextPos{ m_ProjectileMovementEquation[m_CurrentBounceCount - 1].calculateMovement(startPos, minT + .1f) };
		dae::Vector3 hitVelocity{ hitPos ,nextPos };

		float dot{ dae::Vector3::Dot(dae::Vector3{nextPos,otherVertex},planeNormal) };
		dae::Vector3 reflectedNextPos{ nextPos + dot * planeNormal * 2 };
		dae::Vector3 bouncedVelocity{ dae::Vector3{hitPos,reflectedNextPos}.Normalized() };

		dae::Vector3 flattenedBouncedVel{ dae::Vector3{(abs(bouncedVelocity.x) < 0.001f) ? 0.f : bouncedVelocity.x,
														0,
														(abs(bouncedVelocity.z) < 0.001f) ? 0.f : bouncedVelocity.z}.Normalized() };

		float nextA{ (bouncedVelocity.y / abs(bouncedVelocity.y)) * acosf(dae::Vector3::Dot(bouncedVelocity,flattenedBouncedVel)) };
		float nextB{ atan2f(flattenedBouncedVel.z,flattenedBouncedVel.x) };

		SetPhysics(pDevice, pEffect, pProjectile, hitPos, projectileTrace, worldPlanes, v1.Magnitude(), nextA, nextB);
	}
}

void PhysicsCalculator::SetTrace(ID3D11Device* pDevice, Effect* pEffect, const dae::Vector3 startPos,
									std::vector<Sphere*>& projectileTrace, float minT)
{

	float tStep{ minT / m_MaxNumTracers };

	for (int tracerIdx = 0; tracerIdx < m_MaxNumTracers; tracerIdx++)
	{
		dae::Vector3 tracerPos{ m_ProjectileMovementEquation[m_CurrentBounceCount].calculateMovement(startPos, tStep * tracerIdx) };

		if (m_CurrentTracerCount >= projectileTrace.size())
		{
			projectileTrace.push_back(new Sphere{ pDevice,pEffect,tracerPos,.5f });
		}
		else
		{
			projectileTrace[m_CurrentTracerCount]->SetPosition(tracerPos);
		}
		++m_CurrentTracerCount;
	}
}

dae::Vector3 MovementEquation::calculateMovement(const dae::Vector3& startPos, float t)
{
	dae::Vector3 s0{ startPos };
	return dae::Vector3(s0.x + x * t, s0.y + y0 * t + y1 * t * t, s0.z + z * t);
}
