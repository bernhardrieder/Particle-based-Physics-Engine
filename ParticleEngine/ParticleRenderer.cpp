#include "pch.h"
#include "ParticleRenderer.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

ParticleRenderer::ParticleRenderer()
{
}

ParticleRenderer::ParticleRenderer(const DirectX::XMVECTORF32& particleColor) : m_particleColor(particleColor)
{

}

ParticleRenderer::~ParticleRenderer()
{
	m_states.reset();
	m_effect.reset();
	m_batch.reset();
	m_inputLayout.Reset();
}

void ParticleRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ParticleWorld* particleWorld)
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
	m_particleWorld = particleWorld;
	createParticlesVertices();
}

void ParticleRenderer::Render(ID3D11DeviceContext* deviceContext, const Camera& camera)
{
	deviceContext->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	deviceContext->OMSetDepthStencilState(m_states->DepthNone(), 0);
	deviceContext->RSSetState(m_states->CullNone());
	deviceContext->IASetInputLayout(m_inputLayout.Get());

	for(Particle* particle : m_particleWorld->GetActiveParticles())
	{
		assert(particle != nullptr && "particle is nullptr!");
		Matrix scale = Matrix::CreateScale(particle->GetWorldSpaceRadius()*2);
		Matrix world = Matrix::CreateTranslation(particle->GetPosition());

		m_batch->Begin();

		m_effect->SetMatrices(scale*world, camera.GetView(), camera.GetProj());
		m_effect->Apply(deviceContext);

		m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &m_vertices[0], m_vertices.size());

		m_batch->End();
	}
}

void ParticleRenderer::SetParticleColor(const DirectX::XMVECTORF32& color)
{
	m_particleColor = color;
}

void ParticleRenderer::createParticlesVertices()
{
	std::vector<Vector2> vertices = createCircleVerticesTriangleList(Vector2::Zero, 0.5f);
	for (size_t i = 0; i < vertices.size(); ++i)
		m_vertices.push_back(VertexPositionColor(Vector3(vertices[i].x, vertices[i].y, 0.0f), m_particleColor));
}

std::vector<Vector2> ParticleRenderer::createCircleVerticesTriangleList(const DirectX::SimpleMath::Vector2& center, const float& radius) const
{
	std::vector<Vector2> vertices;
	int resolution = 50;

	Vector2 vertex0 = center;
	Vector2 vertex1;
	for (int i = 0; i <= resolution; ++i)
	{
		float t = XM_PI * 2.f * static_cast<float>(i) / static_cast<float>(resolution);
		Vector2 vertex2 = Vector2(radius * cos(t), radius * -sin(t));
		vertices.push_back(vertex0);
		vertices.push_back(vertex1);
		vertices.push_back(vertex2);

		vertex1 = vertex2;
	}
	return vertices;
}