#pragma once

class Texture
{

public:
	~Texture();

	Texture(const Texture& other) = delete;
	Texture(Texture&& other) noexcept = delete;
	Texture& operator=(const Texture& other) = delete;
	Texture& operator=(Texture&& other) noexcept = delete;

	static Texture* LoadFromFile(const std::string& path, ID3D11Device* pDevice);
	dae::ColorRGB Sample(const dae::Vector2& uv) const;
	ID3D11ShaderResourceView* GetSRV();

private:
	Texture(SDL_Surface* pSurface, ID3D11Device* pDevice);

	uint32_t* m_pSurfacePixels{ nullptr };
	ID3D11Texture2D* m_pResource{};
	ID3D11ShaderResourceView* m_pSRV{};

};

