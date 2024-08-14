#pragma once
#include "DataTypes.h"

//struct Vertex_PosCol
//{
//	dae::Vector3 position{};
//	dae::Vector3 color{};
//};

class Effect;
class Texture;
class Mesh final
{
public:
	//Mesh(ID3D11Device* pDevice, Effect* pEffect, const std::vector<dae::Vertex>& vertices, const std::vector<uint32_t>& indices,
	//	Texture* diffuseMap);
	Mesh(ID3D11Device* pDevice, Effect* pEffect, const std::vector<dae::Vertex>& vertices, const std::vector<uint32_t>& indices);
	~Mesh();
	Mesh(const Mesh& other) = delete;
	Mesh(Mesh&& other) noexcept = delete;
	Mesh& operator=(const Mesh& other) = delete;
	Mesh& operator=(Mesh&& other) noexcept = delete;

	void Render(ID3D11DeviceContext* pDevicecontext, const dae::Matrix& worldViewProj) const;
	const dae::Matrix& GetWorldMatrix();
	void UpdateWorldMatrix(const dae::Matrix& newWorldMatrix);
private:
	dae::Matrix m_WorldMatrix{};
	Effect* m_pEffect{};
	ID3D11InputLayout* m_pInputLayout{};
	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};
	
	uint32_t m_NumIndices{};
};

