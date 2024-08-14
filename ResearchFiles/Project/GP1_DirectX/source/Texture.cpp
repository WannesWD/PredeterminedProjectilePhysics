#include "pch.h"
#include "Texture.h"
#include <cassert>


Texture::Texture(SDL_Surface* pSurface, ID3D11Device* pDevice) :
	m_pSurfacePixels{ (uint32_t*)pSurface->pixels }
{
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = pSurface->w;
	desc.Height = pSurface->h;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = pSurface->pixels;
	initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
	initData.SysMemSlicePitch = static_cast<UINT>(pSurface->h * pSurface->pitch);

	HRESULT hr = pDevice->CreateTexture2D(&desc, &initData, &m_pResource);
	if (FAILED(hr))
		assert(false);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView(m_pResource, &SRVDesc, &m_pSRV);
	if (FAILED(hr))
		assert(false);

	SDL_FreeSurface(pSurface);
}

Texture::~Texture()
{
	m_pSRV->Release();
	m_pResource->Release();
}

Texture* Texture::LoadFromFile(const std::string& path, ID3D11Device* pDevice)
{
	SDL_Surface* surface{ IMG_Load(path.c_str()) };
	if (surface == nullptr)
		assert("couldn't loadImage");
	
	return new Texture{ surface,pDevice };
}

dae::ColorRGB Texture::Sample(const dae::Vector2& uv) const
{
	//int x{ int(uv.x * m_pSurface->w) };
	//int y{ int(uv.y * m_pSurface->h) };
	//Uint32 pixel{ Uint32(x + (y * m_pSurface->w)) };
	Uint8 r{};
	Uint8 g{};
	Uint8 b{};
	//SDL_GetRGB(m_pSurfacePixels[pixel], m_pSurface->format, &r, &g, &b);
	//
	return { r / 255.f,g / 255.f,b / 255.f };
}

ID3D11ShaderResourceView* Texture::GetSRV()
{
	return m_pSRV;
}
