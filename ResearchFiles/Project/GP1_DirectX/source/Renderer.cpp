#include "pch.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
#include "Effect.h"	
#include "ShadeEffect.h"
#include "PartialCoverageEffect.h"
#include "Utils.h"
#include "Scene.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}
		m_pScenes.push_back(new Scene_PhysicsSimDefault{});
		m_pScenes.push_back(new Scene_PhysicsSimOct{});
		m_pScenes.push_back(new Scene_PhysicsSimHex{});
		m_pScenes.push_back(new Scene_Test{});
		for (Scene* scene : m_pScenes)
		{
			scene->Initialize(m_pDevice, float(m_Width) / float(m_Height));
		}
	}

	Renderer::~Renderer()
	{
		for (Scene* scene : m_pScenes)
		{
			delete scene;
		}
		ReleaseResources();
	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_pScenes[m_CurrentScene]->Update(pTimer,m_Transforming);

		UpdateInput();
	}


	void Renderer::Render() const
	{
		if (!m_IsInitialized)
			return;

		constexpr float color[4] = { 0.f,0.f,0.3f,1.f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		m_pScenes[m_CurrentScene]->Render(m_pDeviceContext);

		m_pSwapChain->Present(0, 0);
	}

	void Renderer::UpdateInput()
	{
		const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

		const Uint8 f2{ pKeyboardState[SDL_SCANCODE_F2] };
		const Uint8 f3{ pKeyboardState[SDL_SCANCODE_F3] };
		const Uint8 f4{ pKeyboardState[SDL_SCANCODE_F4] };

		if (!m_Pressed)
		{
			if (f2)
			{
				NextFilterMethod();
				m_Pressed = true;
			}
			if (f3)
			{
				NextScene();
				m_Pressed = true;
			}
			if (f4)
			{
				m_Transforming = !m_Transforming;
				m_Pressed = true;
			}
		}
		else if (!f2 && !f3 && !f4)
		{
			m_Pressed = false;
		}
	}
		
	HRESULT Renderer::InitializeDirectX()
	{
		HRESULT result{ CreateDevice() };
		if (FAILED(result))
			return result;

		result = CreateSwapChain();
		if (FAILED(result))
			return result;

		result = CreateDepthBuffer();
		if (FAILED(result))
			return result;

		result = CreateResourceView();
		if (FAILED(result))
			return result;

		SetViewport();

		return S_OK;
		//return S_FALSE;
	}

	HRESULT Renderer::CreateDevice()
	{
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlags = 0;

#ifdef DEBUG || _DEBUG
		createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif // DEBUG || _DEBUG
		return D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0,
			createDeviceFlags, &featureLevel, 1, D3D11_SDK_VERSION,
			&m_pDevice, nullptr, &m_pDeviceContext);
	}

	HRESULT Renderer::CreateSwapChain()
	{
		HRESULT result = CreateDXGIFactory1(__uuidof(IDXGIFactory1),
			reinterpret_cast<void**>(&m_pDxgiFactory));
		if (FAILED(result))
			return result;

		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		SDL_SysWMinfo sysWMInfo{};
		SDL_GetVersion(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		result = m_pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
		return result;
	}

	HRESULT Renderer::CreateDepthBuffer()
	{
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		HRESULT result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result))
			return result;
		
		return m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
	}

	HRESULT Renderer::CreateResourceView()
	{
		HRESULT result{ m_pSwapChain->GetBuffer(0,__uuidof(ID3D11Texture2D),
						reinterpret_cast<void**>(&m_pRenderTargetBuffer)) };
		if (FAILED(result))
			return result;

		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result))
			return result;

		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
		return result;
	}
	void Renderer::SetViewport()
	{
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewport);
	}
	void Renderer::ReleaseResources()
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetBuffer->Release();
		m_pDepthStencilView->Release();
		m_pDepthStencilBuffer->Release();
		m_pSwapChain->Release();
		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}
		m_pDevice->Release();
		m_pDxgiFactory->Release();
	}
	void Renderer::NextFilterMethod()
	{
		m_CurrentFilterMethod = Effect::FilterMethod((int(m_CurrentFilterMethod) + 1) % int(Effect::maxMethod));
		std::wcout << L"Filter Method: ";
		switch (m_CurrentFilterMethod)
		{
		case Effect::Anisotropic:
			std::wcout << "Anisotropic\n";
			break;
		case Effect::Point:
			std::wcout << "Point\n";
			break;
		case Effect::Linear:
			std::wcout << "Linear\n";
			break;
		}

		for (Scene* scene : m_pScenes)
		{
			scene->SetFilterMethod(m_CurrentFilterMethod);
		}
	}
	void Renderer::NextScene()
	{
		m_CurrentScene = ++m_CurrentScene % m_pScenes.size();
	}
}
