#include "pch.h"
#include "Platform.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Platform::Platform()
{
}

Platform::Platform(const DirectX::XMVECTORF32& color, const float& platformThickness) : m_color(color), m_thickness(platformThickness)
{
}

Platform::~Platform()
{
	m_states.reset();
	m_effect.reset();
	m_batch.reset();
	m_inputLayout.Reset();
}

void Platform::SetColorAndThickness(const DirectX::XMVECTORF32& color, const float& platformThickness)
{
	m_color = color;
	m_thickness = platformThickness;
}

void Platform::Initialize(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(deviceContext);
	m_states = std::make_unique<CommonStates>(device);

	/******************* simple shader *******************/
	m_effect = std::make_unique<BasicEffect>(device);
	m_effect->SetVertexColorEnabled(true);

	void const* shaderByteCode;
	size_t byteCodeLength;

	m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	DX::ThrowIfFailed(
		device->CreateInputLayout(VertexPositionColor::InputElements,
			VertexPositionColor::InputElementCount,
			shaderByteCode, byteCodeLength,
			m_inputLayout.ReleaseAndGetAddressOf()));

	//other
	createVertices(start, end);
}

void Platform::Render(ID3D11DeviceContext* deviceContext, const Camera& camera)
{
	deviceContext->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	deviceContext->OMSetDepthStencilState(m_states->DepthNone(), 0);
	deviceContext->RSSetState(m_states->CullNone());
	deviceContext->IASetInputLayout(m_inputLayout.Get());

	m_batch->Begin();

	m_effect->SetMatrices(Matrix::Identity, camera.GetView(), camera.GetProj());
	m_effect->Apply(deviceContext);

	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &m_vertices[0], m_vertices.size());

	m_batch->End();
}

void Platform::createVertices(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end)
{
	Matrix rotation = Matrix::CreateRotationZ(XMConvertToRadians(-90));
	Vector3 vertices[6];
	Vector3 dir = end - start;
	Vector3 rotatedDir = Vector3::Transform(dir, rotation);
	rotatedDir.Normalize();
	
	vertices[0] = start;
	vertices[1] = end;
	vertices[2] = start + rotatedDir*m_thickness;
	vertices[3] = vertices[2];
	vertices[4] = end;
	vertices[5] = vertices[2] + dir;
	for(const Vector3& vertex : vertices)
	{
		m_vertices.push_back(VertexPositionColor(Vector3(vertex.x, vertex.y, 0.0f), m_color));
	}
}
