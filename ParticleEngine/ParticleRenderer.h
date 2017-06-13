#pragma once
class ParticleRenderer : public ParticleManagement
{
public:
	ParticleRenderer();
	explicit ParticleRenderer(const DirectX::XMVECTORF32& particleColor);
	~ParticleRenderer();

	void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Render(ID3D11DeviceContext* deviceContext, const Camera& camera);
	void SetParticleColor(const DirectX::XMVECTORF32& color);

private:
	void createParticlesVertices();
	std::vector<DirectX::SimpleMath::Vector2> createCircleVerticesTriangleList(const DirectX::SimpleMath::Vector2& center, const float& radius) const;

	//rendering
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
	std::unique_ptr<DirectX::BasicEffect> m_effect;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	std::unique_ptr<DirectX::CommonStates> m_states;
	std::vector<DirectX::VertexPositionColor> m_vertices;

	DirectX::XMVECTORF32 m_particleColor = DirectX::Colors::White;
};
