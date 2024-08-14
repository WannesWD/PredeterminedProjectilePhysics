#include <iostream>
#include "pch.h"
#include "Scene.h"
#include "Utils.h"
#include "Texture.h"
#include "ShadeEffect.h"
#include "PartialCoverageEffect.h"
#include "SimpleEffect.h"

#pragma region Base Scene

	void Scene::Initialize(ID3D11Device* pDevice, float AspectRatio)
	{
		m_pCamera = new Camera{ {0.f,0.f,-50.f},AspectRatio,45.f,.1f,1000.f };
	}

	Scene_Test::~Scene_Test()
	{
		delete m_pVehicleMesh;
		delete m_pFireMesh;
		delete m_pShadeEffect;
		delete m_pPartialCoverageEffect;
	}

	void Scene_Test::Initialize(ID3D11Device* pDevice,float AspectRatio)
	{
		Scene::Initialize(pDevice, AspectRatio);
		m_SceneName = "TestScene";

		std::vector<dae::Vertex> vehicleVertices{};
		std::vector<uint32_t> vehicleIndices{};
		dae::Utils::ParseOBJ("Resources/vehicle.obj", vehicleVertices, vehicleIndices);
		Texture* vehicleDiffuseMap{ Texture::LoadFromFile("Resources/vehicle_diffuse.png",pDevice) };
		Texture* vehicleNormalMap{ Texture::LoadFromFile("Resources/vehicle_normal.png",pDevice) };
		Texture* vehicleSpecularMap{ Texture::LoadFromFile("Resources/vehicle_specular.png",pDevice) };
		Texture* vehicleGlossMap{ Texture::LoadFromFile("Resources/vehicle_gloss.png",pDevice) };

		std::vector<dae::Vertex> fireVertices{};
		std::vector<uint32_t> fireIndices{};
		dae::Utils::ParseOBJ("Resources/fireFX.obj", fireVertices, fireIndices);
		Texture* fireDiffuseMap{ Texture::LoadFromFile("Resources/fireFX_diffuse.png",pDevice) };

		m_pShadeEffect = new ShadeEffect{ pDevice,L"Resources/PosCol3D.fx",
									vehicleDiffuseMap, vehicleNormalMap, vehicleSpecularMap, vehicleGlossMap };
		m_pPartialCoverageEffect = new PartialCoverageEffect{ pDevice, L"Resources/PosCol3D.fx", fireDiffuseMap };

		m_pVehicleMesh = new Mesh{ pDevice, m_pShadeEffect, vehicleVertices, vehicleIndices };
		m_pFireMesh = new Mesh{ pDevice, m_pPartialCoverageEffect, fireVertices, fireIndices };

		delete vehicleDiffuseMap;
		delete vehicleNormalMap;
		delete vehicleSpecularMap;
		delete vehicleGlossMap;
		delete fireDiffuseMap;
	}

	void Scene_Test::Update(const dae::Timer* pTimer, bool transforming)
	{
		Scene::Update(pTimer);
		if (transforming)
		{
			m_TotalRotation = pTimer->GetElapsed();
			m_TransformMatrix = dae::Matrix::CreateRotationY(m_TotalRotation);

			m_WorldMatrix = m_TransformMatrix * m_pVehicleMesh->GetWorldMatrix();
		}

		m_WorldViewProjectionMatrix = m_WorldMatrix * m_pCamera->GetViewMatrx() * m_pCamera->GetProjectionMatrix();

		m_pVehicleMesh->UpdateWorldMatrix(m_WorldMatrix);
		m_pFireMesh->UpdateWorldMatrix(m_WorldMatrix);
	}
	void Scene_Test::Render(ID3D11DeviceContext* pDeviceContext) const
	{
		ID3D11RasterizerState* test{ m_pShadeEffect->GetRasterizerState() };
		pDeviceContext->RSSetState(m_pShadeEffect->GetRasterizerState());
		m_pVehicleMesh->Render(pDeviceContext, m_WorldViewProjectionMatrix);

		pDeviceContext->RSSetState(m_pPartialCoverageEffect->GetRasterizerState());
		m_pFireMesh->Render(pDeviceContext, m_WorldViewProjectionMatrix);
	}

	void Scene_Test::SetFilterMethod(Effect::FilterMethod filterMethod)
	{
		m_pShadeEffect->SetFilterMethod(filterMethod);
		m_pPartialCoverageEffect->SetFilterMethod(filterMethod);
	}

	Scene_Test0::~Scene_Test0()
	{
		delete m_pPartialCoverageEffect;
		delete m_pMesh;
	}

	void Scene_Test0::Initialize(ID3D11Device* pDevice, float AspectRatio)
	{
		Scene::Initialize(pDevice, AspectRatio);
		m_SceneName = "TestScene0";
		m_pCamera->SetPosition({ 0.f,0.f,-10.f });
		std::vector<dae::Vertex> vertices{
		{{-3.f,3.f,2.f},{1.f,0.f,1.f},{0,0}},
		{{3.f,-3.f,2.f},{0.f,0.f,1.f}, {1,1}},
		{{-3.f,-3.f,2.f},{0.f,1.f,0.f}, {0,1}},
		{{3.f,3.f,2.f},{1.f,0.f,0.f},{1,0}},
		};
		std::vector<uint32_t> indices{ 0,1,2,3,1,0};

		Texture* pDiffuseMap{ Texture::LoadFromFile("Resources/uv_grid_2.png",pDevice) };

		m_pPartialCoverageEffect = new PartialCoverageEffect{ pDevice, L"Resources/PosCol3D.fx", pDiffuseMap };
		m_pMesh = new Mesh{ pDevice,m_pPartialCoverageEffect,vertices,indices };

		delete pDiffuseMap;
	}

	void Scene_Test0::Update(const dae::Timer* pTimer, bool transforming)
	{
		Scene::Update(pTimer);
		m_WorldViewProjectionMatrix = m_pMesh->GetWorldMatrix() * m_pCamera->GetViewMatrx() * m_pCamera->GetProjectionMatrix();
	}

	void Scene_Test0::Render(ID3D11DeviceContext* pDeviceContext) const
	{
		ID3D11RasterizerState* test{ m_pPartialCoverageEffect->GetRasterizerState() };
		pDeviceContext->RSSetState(m_pPartialCoverageEffect->GetRasterizerState());
		m_pMesh->Render(pDeviceContext, m_WorldViewProjectionMatrix);
	}

	void Scene_Test0::SetFilterMethod(Effect::FilterMethod filterMethod)
	{
		m_pPartialCoverageEffect->SetFilterMethod(filterMethod);
	}

	Scene_PhysicsSimDefault::~Scene_PhysicsSimDefault()
	{
		delete m_pWallEffect;
		delete m_pTracerEffect;
		delete m_pProjectileEffect;
		delete m_pTurretEffect;

		delete m_pTurret;

		for (Projectile* projectile : m_pProjectiles)
		{
			delete projectile;
		}
		for (Sphere* sphere : m_pProjectileTrace)
		{
			delete sphere;
		}
		for (Plane* plane : m_pPlanes)
		{
			delete plane;
		}
	}

	void Scene_PhysicsSimDefault::Initialize(ID3D11Device* pDevice, float AspectRatio)
	{
		Scene::Initialize(pDevice, AspectRatio);
		m_SceneName = "TestScene0";
		m_pCamera->SetPosition({ 0.f,-45.f,0.f });
		m_PhysicsCalculator = PhysicsCalculator::GetPhysiscsCalculator();

		Texture* pWallDiffuseMap{ Texture::LoadFromFile("Resources/Wall_Diffuse.png",pDevice) };
		Texture* pTracerDiffuseMap{ Texture::LoadFromFile("Resources/Tracer_Diffuse.png",pDevice) };
		Texture* pProjectileDiffuseMap{ Texture::LoadFromFile("Resources/Projectile_Diffuse.png",pDevice) };
		Texture* pTurretDiffuseMap{ Texture::LoadFromFile("Resources/Turret_Diffuse.png",pDevice) };

		m_pWallEffect = new SimpleEffect{ pDevice, L"Resources/PosCol3D.fx", pWallDiffuseMap };
		m_pTracerEffect = new SimpleEffect{ pDevice, L"Resources/PosCol3D.fx", pTracerDiffuseMap};
		m_pProjectileEffect = new SimpleEffect{ pDevice, L"Resources/PosCol3D.fx", pProjectileDiffuseMap};
		m_pTurretEffect = new SimpleEffect{ pDevice, L"Resources/PosCol3D.fx", pTurretDiffuseMap };

		std::vector<dae::Vertex> vertices{};
		std::vector<uint32_t> indices{};
		dae::Utils::ParseOBJ("Resources/Turret.obj", vertices, indices);

		m_pTurret = new Mesh{ pDevice,m_pTurretEffect,vertices,indices };
		SetTurretStartingOrientation();

		float sphereRad{ 1.f };
		m_pProjectiles.push_back(new Projectile{ pDevice,m_pProjectileEffect,m_TurretOrigin,sphereRad });
		SetPlanes(pDevice);


		m_PhysicsCalculator->CalculatePhysics(pDevice,m_pTracerEffect, m_pProjectiles[0], m_TurretOrigin, m_pProjectileTrace
											,m_pPlanes, m_LaunchForce, m_LaunchAngleA, m_LaunchAngleB);
		m_pDevice = pDevice;

		delete pWallDiffuseMap;
		delete pTracerDiffuseMap;
		delete pProjectileDiffuseMap;
		delete pTurretDiffuseMap;
	}

	void Scene_PhysicsSimDefault::Update(const dae::Timer* pTimer, bool transforming)
	{
		Scene::Update(pTimer);
		
		for (Projectile* projectile : m_pProjectiles)
		{
			projectile->RenderUpdate(pTimer, m_pCamera);
			if (projectile->BeenShot() && transforming)
			{
				projectile->Update(pTimer, m_pCamera);
			}
		}

		for (Sphere* sphere : m_pProjectileTrace)
		{
			sphere->Update(pTimer, m_pCamera);
		}
		for (Plane* plane : m_pPlanes)
		{
			plane->Update(pTimer, m_pCamera);
		}

		UpdateTurret(pTimer);

		m_WorldViewProjectionMatrix = m_pTurret->GetWorldMatrix() * m_pCamera->GetViewMatrx() * m_pCamera->GetProjectionMatrix();

	}

	void Scene_PhysicsSimDefault::SetPlanes(ID3D11Device* pDevice)
	{
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{50.f,0.f,50.f},dae::Vector3{0.f,0.f,float(M_PI / 2.f)} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{-50.f,0.f,50.f},dae::Vector3{0.f,0.f,-float(M_PI / 2.f)} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{0.f,-50.f,50.f},dae::Vector3{0.f,0.f,0.f} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{0.f,50.f,50.f},dae::Vector3{0.f,0.f,float(M_PI)} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{0.f,0.f,100.f},dae::Vector3{float(M_PI / 2.f),0.f,0.f} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{0.f,0.f,0.f},dae::Vector3{-float(M_PI / 2.f),0.f,0.f} });
	}

	void Scene_PhysicsSimDefault::Render(ID3D11DeviceContext* pDeviceContext) const
	{
		
		pDeviceContext->RSSetState(m_pProjectileEffect->GetRasterizerState());
		for (Projectile* projectile : m_pProjectiles)
		{
			if (projectile->BeenShot())
			{
				projectile->Render(pDeviceContext);
			}
		}

		pDeviceContext->RSSetState(m_pTracerEffect->GetRasterizerState());
		for (Sphere* sphere : m_pProjectileTrace)
		{
			sphere->Render(pDeviceContext);
		}
		pDeviceContext->RSSetState(m_pWallEffect->GetRasterizerState());
		for (Plane* plane : m_pPlanes)
		{
			plane->Render(pDeviceContext);
		}
		pDeviceContext->RSSetState(m_pTurretEffect->GetRasterizerState());
		m_pTurret->Render(pDeviceContext, m_WorldViewProjectionMatrix);
	}

	void Scene_PhysicsSimDefault::SetFilterMethod(Effect::FilterMethod filterMethod)
	{
		m_pWallEffect->SetFilterMethod(filterMethod);
		m_pTracerEffect->SetFilterMethod(filterMethod);
		m_pProjectileEffect->SetFilterMethod(filterMethod);
		m_pTurretEffect->SetFilterMethod(filterMethod);

	}

	void Scene_PhysicsSimDefault::SetTurretAngle(float angleA, float angleB)
	{
		m_pTurret->UpdateWorldMatrix(m_pTurret->GetWorldMatrix() * dae::Matrix::CreateRotation(angleA, angleB, 0.f));
	}

	void Scene_PhysicsSimDefault::SetTurretStartingOrientation()
	{
		m_pTurret->UpdateWorldMatrix(m_pTurret->GetWorldMatrix() *
			dae::Matrix::CreateScale(dae::Vector3{ m_TurretScale,m_TurretScale,m_TurretScale }) *
			dae::Matrix::CreateRotation(m_LaunchAngleA, m_LaunchAngleB - float(M_PI),0)*
			dae::Matrix::CreateTranslation(m_TurretOrigin));
	}

	void Scene_PhysicsSimDefault::UpdateTurret(const dae::Timer* pTimer)
	{
		const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

		const Uint8 up{ pKeyboardState[SDL_SCANCODE_UP] };
		const Uint8 down{ pKeyboardState[SDL_SCANCODE_DOWN] };
		const Uint8 left{ pKeyboardState[SDL_SCANCODE_LEFT] };
		const Uint8 right{ pKeyboardState[SDL_SCANCODE_RIGHT] };
		const Uint8 enter{ pKeyboardState[SDL_SCANCODE_RETURN] };
		const Uint8 plus{ pKeyboardState[SDL_SCANCODE_EQUALS] };
		const Uint8 minus{ pKeyboardState[SDL_SCANCODE_MINUS] };
		const Uint8 comma{ pKeyboardState[SDL_SCANCODE_COMMA] };
		const Uint8 period{ pKeyboardState[SDL_SCANCODE_PERIOD] };

		if (!m_Pressed)
		{
			if (up || down || left || right ||
				plus || minus || comma || period)
			{
				UpdateTurretAngle(up, down, left, right);

				UpdateGravity(comma, period);

				UpdateLaunchForce(plus, minus);

				Projectile* nextProjectile{};
				LoadProjectile(nextProjectile);

				m_Pressed = true;
			}
		}
		else if (!up && !down && !left && !right)
		{
			m_Pressed = false;
		}
		else
		{
			m_HoldTime += pTimer->GetElapsed();
			if (m_HoldTime >= m_MaxHoldTime)
			{
				m_HoldTime = 0;
				m_Pressed = false;
			}
		}

		if (!m_ShootPressed)
		{
			if (enter)
			{
				ShootTurret();

				m_ShootPressed = true;
			}
		}
		else if (!enter)
		{
			m_ShootPressed = false;
		}
		else
		{
			m_ShootHoldTime += pTimer->GetElapsed();
			if (m_ShootHoldTime >= m_MaxShootHoldTime)
			{
				m_ShootHoldTime = 0;
				m_ShootPressed = false;
			}
		}
	}

	void Scene_PhysicsSimDefault::UpdateTurretAngle(const Uint8 up, const Uint8 down, const Uint8 left, const Uint8 right)
	{
		m_pTurret->UpdateWorldMatrix(m_pTurret->GetWorldMatrix() *
			dae::Matrix::CreateTranslation(-m_TurretOrigin) *
			dae::Matrix::CreateRotation(0, -(m_TurretAngleB - float(M_PI)), 0) *
			dae::Matrix::CreateRotation(-m_LaunchAngleA, 0, 0));

		if (up)
			m_LaunchAngleA += m_AngleChange;

		if (down)
			m_LaunchAngleA -= m_AngleChange;

		if (left)
		{
			m_LaunchAngleB += m_AngleChange;
			m_TurretAngleB -= m_AngleChange;
		}

		if (right)
		{
			m_LaunchAngleB -= m_AngleChange;
			m_TurretAngleB += m_AngleChange;
		}

		m_pTurret->UpdateWorldMatrix(m_pTurret->GetWorldMatrix() *
			dae::Matrix::CreateRotation(m_LaunchAngleA, m_TurretAngleB - float(M_PI), 0) *
			dae::Matrix::CreateTranslation(m_TurretOrigin));
	}

	void Scene_PhysicsSimDefault::UpdateGravity(const Uint8 comma, const Uint8 period)
	{
		float currentGravity{ m_PhysicsCalculator->GetGravity() };

		if (comma)
			m_PhysicsCalculator->SetGravity(currentGravity - .1f);

		if (period)
			m_PhysicsCalculator->SetGravity(currentGravity + .1f);
	}

	void Scene_PhysicsSimDefault::UpdateLaunchForce(const Uint8 plus, const Uint8 minus)
	{
		if (plus)
			m_LaunchForce += 1.f;

		if (minus)
			m_LaunchForce -= 1.f;
	}

	void Scene_PhysicsSimDefault::ShootTurret()
	{
		bool shot{};
		for (Projectile* projectile : m_pProjectiles)
		{
			if (!projectile->BeenShot() && projectile->IsReady())
			{
				projectile->Shoot();
				shot = true;
				break;
			}
		}
		if (!shot)
		{
			Projectile* nextProjectile{};
			LoadProjectile(nextProjectile);
			nextProjectile->Shoot();
		}
	}


	void Scene_PhysicsSimDefault::LoadProjectile(Projectile*& nextProjectile)
	{
		for (Projectile* projectile : m_pProjectiles)
		{
			if (!projectile->BeenShot())
			{
				nextProjectile = projectile;
				break;
			}
		}
		if (nextProjectile == nullptr)
		{
			nextProjectile = new Projectile(m_pDevice, m_pProjectileEffect, m_TurretOrigin, 1.f);
			m_pProjectiles.push_back(nextProjectile);
		}

		m_PhysicsCalculator->CalculatePhysics(m_pDevice, m_pTracerEffect, nextProjectile, m_TurretOrigin, m_pProjectileTrace
			, m_pPlanes, m_LaunchForce, m_LaunchAngleA, m_LaunchAngleB);
	}


	void Scene_PhysicsSimHex::SetPlanes(ID3D11Device* pDevice)
	{
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{85.f,-15.f,50.f},dae::Vector3{0.f,0.f,float(M_PI / 4.f)} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{-85.f,-15.f,50.f},dae::Vector3{0.f,0.f,-float(M_PI / 4.f)} });

		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{85.f,50.f,50.f},dae::Vector3{0.f,0.f,float(3.f * M_PI / 4.f)} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{-85.f,50.f,50.f},dae::Vector3{0.f,0.f,-float(3.f * M_PI / 4.f)} });

		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{0.f,-50.f,50.f},dae::Vector3{0.f,0.f,0.f} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{0.f,85.f,50.f},dae::Vector3{0.f,0.f,float(M_PI)} });

		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{0.f,0.f,100.f},dae::Vector3{float(M_PI / 2.f),0.f,0.f} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{100.f,0.f,100.f},dae::Vector3{float(M_PI / 2.f),0.f,0.f} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{-100.f,0.f,100.f},dae::Vector3{float(M_PI / 2.f),0.f,0.f} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{0.f,100.f,100.f},dae::Vector3{float(M_PI / 2.f),0.f,0.f} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{100.f,100.f,100.f},dae::Vector3{float(M_PI / 2.f),0.f,0.f} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{-100.f,100.f,100.f},dae::Vector3{float(M_PI / 2.f),0.f,0.f} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{0.f,0.f,0.f},dae::Vector3{-float(M_PI / 2.f),0.f,0.f} });
	}

	void Scene_PhysicsSimOct::SetPlanes(ID3D11Device* pDevice)
	{
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{120.f,0.f,120.f},dae::Vector3{0.f,0.f,float(M_PI / 2.f)} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{-120.f,0.f,120.f},dae::Vector3{0.f,0.f,-float(M_PI / 2.f)} });

		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{85.f,0.f,35.f},dae::Vector3{-float(M_PI / 4.f),0.f,float(M_PI / 2.f)} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{-85.f,0.f,35.f},dae::Vector3{-float(M_PI / 4.f),0.f,-float(M_PI / 2.f)} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{85.f,0.f,205.f},dae::Vector3{float(M_PI / 4.f),0.f,float(M_PI / 2.f)} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{-85.f,0.f,205.f},dae::Vector3{float(M_PI / 4.f),0.f,-float(M_PI / 2.f)} });

		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{0.f,-50.f,100.f},dae::Vector3{0.f,0.f,0.f} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{100.f,-50.f,100.f},dae::Vector3{0.f,0.f,0.f} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{-100.f,-50.f,100.f},dae::Vector3{0.f,0.f,0.f} });

		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{0.f,-50.f,0.f},dae::Vector3{0.f,0.f,0.f} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{100.f,-50.f,0.f},dae::Vector3{0.f,0.f,0.f} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{-100.f,-50.f,0.f},dae::Vector3{0.f,0.f,0.f} });

		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{0.f,-50.f,200.f},dae::Vector3{0.f,0.f,0.f} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{100.f,-50.f,200.f},dae::Vector3{0.f,0.f,0.f} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{-100.f,-50.f,200.f},dae::Vector3{0.f,0.f,0.f} });

		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{0.f,50.f,100.f},dae::Vector3{0.f,0.f,float(M_PI)} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{100.f,50.f,100.f},dae::Vector3{0.f,0.f,float(M_PI)} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{-100.f,50.f,100.f},dae::Vector3{0.f,0.f,float(M_PI)} });

		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{0.f,50.f,0.f},dae::Vector3{0.f,0.f,float(M_PI)} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{100.f,50.f,0.f},dae::Vector3{0.f,0.f,float(M_PI)} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{-100.f,50.f,0.f},dae::Vector3{0.f,0.f,float(M_PI)} });

		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{0.f,50.f,200.f},dae::Vector3{0.f,0.f,float(M_PI)} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{100.f,50.f,200.f},dae::Vector3{0.f,0.f,float(M_PI)} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{-100.f,50.f,200.f},dae::Vector3{0.f,0.f,float(M_PI)} });

		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{0.f,0.f,240.f},dae::Vector3{float(M_PI / 2.f),0.f,0.f} });
		m_pPlanes.push_back(new Plane{ pDevice,m_pWallEffect,dae::Vector3{0.f,0.f,0.f},dae::Vector3{-float(M_PI / 2.f),0.f,0.f} });
	}
