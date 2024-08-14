#pragma once
#include <string>
#include <vector>
#include "pch.h"
#include "Math.h"
#include "DataTypes.h"
#include "Camera.h"
#include "Mesh.h"
#include "Effect.h"
#include "SimpleShapes.h"
#include "PhysicsCalculator.h"
#include "Projectile.h"

//Forward Declarations
class Timer;

//Scene Base Class
class Scene
{
public:
	Scene() = default;
	virtual ~Scene() = default;

	Scene(const Scene&) = delete;
	Scene(Scene&&) noexcept = delete;
	Scene& operator=(const Scene&) = delete;
	Scene& operator=(Scene&&) noexcept = delete;

	virtual void Initialize(ID3D11Device* pDevice, float AspectRatio);
	virtual void Update(const dae::Timer* pTimer, bool transforming = true)
	{
		m_pCamera->Update(pTimer);
	}
	virtual void Render(ID3D11DeviceContext* pDeviceContext) const = 0;
	Camera* GetCamera() { return m_pCamera; }
	virtual void SetFilterMethod(Effect::FilterMethod filterMethod) = 0;
protected:
	std::string	m_SceneName;

	Camera* m_pCamera{};

	float m_TotalRotation{};
	dae::Matrix m_TransformMatrix{};
	dae::Matrix m_WorldMatrix{};
};

class Scene_Test final : public Scene
{
public:
	Scene_Test() = default;
	~Scene_Test() override;

	Scene_Test(const Scene_Test&) = delete;
	Scene_Test(Scene_Test&&) noexcept = delete;
	Scene_Test& operator=(const Scene_Test&) = delete;
	Scene_Test& operator=(Scene_Test&&) noexcept = delete;

	void Initialize(ID3D11Device* pDevice, float AspectRatio) override;
	void Update(const dae::Timer* pTimer, bool transforming) override;
	void Render(ID3D11DeviceContext* pDeviceContext) const override;
	void SetFilterMethod(Effect::FilterMethod filterMethod) override;
private:
	Mesh* m_pVehicleMesh{};
	Mesh* m_pFireMesh{};
	Effect* m_pShadeEffect{};
	Effect* m_pPartialCoverageEffect{};
	dae::Matrix m_WorldViewProjectionMatrix{};
};

class Scene_Test0 final : public Scene
{
public:
	Scene_Test0() = default;
	~Scene_Test0() override;

	Scene_Test0(const Scene_Test0&) = delete;
	Scene_Test0(Scene_Test0&&) noexcept = delete;
	Scene_Test0& operator=(const Scene_Test0&) = delete;
	Scene_Test0& operator=(Scene_Test0&&) noexcept = delete;

	void Initialize(ID3D11Device* pDevice, float AspectRatio) override;
	void Update(const dae::Timer* pTimer, bool transforming) override;
	void Render(ID3D11DeviceContext* pDeviceContext) const override;
	void SetFilterMethod(Effect::FilterMethod filterMethod) override;
private:
	Mesh* m_pMesh{};
	Effect* m_pPartialCoverageEffect{};
	dae::Matrix m_WorldViewProjectionMatrix{};
};

class Scene_PhysicsSimDefault : public Scene
{
public:
	Scene_PhysicsSimDefault() = default;
	~Scene_PhysicsSimDefault() override;

	Scene_PhysicsSimDefault(const Scene_PhysicsSimDefault&) = delete;
	Scene_PhysicsSimDefault(Scene_PhysicsSimDefault&&) noexcept = delete;
	Scene_PhysicsSimDefault& operator=(const Scene_PhysicsSimDefault&) = delete;
	Scene_PhysicsSimDefault& operator=(Scene_PhysicsSimDefault&&) noexcept = delete;

	void Initialize(ID3D11Device* pDevice, float AspectRatio) override;
	void Update(const dae::Timer* pTimer, bool transforming) override;
	virtual void SetPlanes(ID3D11Device* pDevice);

	void Render(ID3D11DeviceContext* pDeviceContext) const override;
	void SetFilterMethod(Effect::FilterMethod filterMethod) override;
	void SetTurretAngle(float angleA, float angleB);
protected:
	void SetTurretStartingOrientation();
	void ShootTurret();

	void UpdateTurret(const dae::Timer* pTimer);
	void UpdateTurretAngle(const Uint8 up, const Uint8 down, const Uint8 left, const Uint8 right);
	void UpdateGravity(const Uint8 comma, const Uint8 period);
	void UpdateLaunchForce(const Uint8 plus, const Uint8 minus);

	void LoadProjectile(Projectile*& nextProjectile);

	Mesh* m_pTurret{};
	std::vector<Projectile*> m_pProjectiles{};
	//Projectile* m_pProjectileTest{};

	std::vector<Sphere*> m_pProjectileTrace{};
	std::vector<Plane*> m_pPlanes{};

	Effect* m_pWallEffect{};
	Effect* m_pTracerEffect{};
	Effect* m_pProjectileEffect{};
	Effect* m_pTurretEffect{};

	dae::Matrix m_WorldViewProjectionMatrix{};
	PhysicsCalculator* m_PhysicsCalculator{};
	ID3D11Device* m_pDevice{};

	float m_LaunchForce{ 100.f };
	float m_LaunchAngleA{float(M_PI/8.f)};
	float m_LaunchAngleB{ float(3.f * M_PI / 4.f) };
	float m_TurretAngleB{ float(3.f * M_PI / 4.f) };

	const float m_TurretScale{ 2.f };
	dae::Vector3 m_TurretOrigin{ 0.f,-50.f + 2.f,20.f };

	float m_HoldTime{};
	float m_ShootHoldTime{};
	const float m_AngleChange{ 0.3f / 180.f * float(M_PI) };
	const float m_MaxHoldTime{ .01f };
	const float m_MaxShootHoldTime{ 0.25f };
	bool m_Pressed{ false };
	bool m_ShootPressed{ false };
};

class Scene_PhysicsSimHex : public Scene_PhysicsSimDefault
{
public:
	Scene_PhysicsSimHex() = default;
	~Scene_PhysicsSimHex() = default;

	Scene_PhysicsSimHex(const Scene_PhysicsSimHex&) = delete;
	Scene_PhysicsSimHex(Scene_PhysicsSimHex&&) noexcept = delete;
	Scene_PhysicsSimHex& operator=(const Scene_PhysicsSimHex&) = delete;
	Scene_PhysicsSimHex& operator=(Scene_PhysicsSimHex&&) noexcept = delete;

	void SetPlanes(ID3D11Device* pDevice) override;
};

class Scene_PhysicsSimOct : public Scene_PhysicsSimDefault
{
public:
	Scene_PhysicsSimOct() = default;
	~Scene_PhysicsSimOct() = default;

	Scene_PhysicsSimOct(const Scene_PhysicsSimOct&) = delete;
	Scene_PhysicsSimOct(Scene_PhysicsSimOct&&) noexcept = delete;
	Scene_PhysicsSimOct& operator=(const Scene_PhysicsSimOct&) = delete;
	Scene_PhysicsSimOct& operator=(Scene_PhysicsSimOct&&) noexcept = delete;

	void SetPlanes(ID3D11Device* pDevice) override;
};