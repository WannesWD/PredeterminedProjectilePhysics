#include "pch.h"
#include "Mesh.h"
#include "Effect.h"
#include <cassert>

Mesh::Mesh(ID3D11Device* pDevice, Effect* pEffect, const std::vector<dae::Vertex>& vertices, const std::vector<uint32_t>& indices)
{
	m_pEffect = pEffect;
	m_pEffect->SetWorldMatrix(m_WorldMatrix);

	static constexpr uint32_t numElements{ 5 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "COLOR";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "TEXCOORD";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "NORMAL";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[4].SemanticName = "TANGENT";
	vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//create input layout
	ID3DX11EffectTechnique* pTechnique{ m_pEffect->GetTechnique() };
	D3DX11_PASS_DESC passDesc{};
	pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
	
	HRESULT result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);
	if (FAILED(result))
		assert(false);

	//create vertex buffer
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(dae::Vertex) * static_cast<uint32_t>(vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData {};
	initData.pSysMem = vertices.data();
	
	result = pDevice->CreateBuffer(&bd,&initData,&m_pVertexBuffer);
	if (FAILED(result))
		assert(false);
	
	//create index buffer
	m_NumIndices = static_cast<uint32_t>(indices.size());
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = indices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);

	if (FAILED(result))
		assert(false);
}

Mesh::~Mesh()
{
	m_pIndexBuffer->Release();
	m_pVertexBuffer->Release();
	m_pInputLayout->Release();
}

void Mesh::Render(ID3D11DeviceContext* pDevicecontext, const dae::Matrix& worldViewProj) const
{
	m_pEffect->SetWorldViewProjMatrix(worldViewProj);

	pDevicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pDevicecontext->IASetInputLayout(m_pInputLayout);

	constexpr UINT stride = sizeof(dae::Vertex);
	constexpr UINT offset = 0;

	pDevicecontext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	pDevicecontext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);


	D3DX11_TECHNIQUE_DESC techDesc{};
	m_pEffect->GetTechnique()->GetDesc(&techDesc);
	for (UINT p =0; p< techDesc.Passes; ++p)
	{
		m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDevicecontext);
		pDevicecontext->DrawIndexed(m_NumIndices, 0, 0);
	}
}

const dae::Matrix& Mesh::GetWorldMatrix()
{
	return m_WorldMatrix;
}

void Mesh::UpdateWorldMatrix(const dae::Matrix& newWorldMatrix)
{
	m_WorldMatrix = newWorldMatrix;
	m_pEffect->SetWorldMatrix(m_WorldMatrix);
}
