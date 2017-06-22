#pragma once
class Platform
{
public:
	Platform();
	explicit Platform(const DirectX::XMVECTORF32& color, const float& platformThickness);
	~Platform();

	void SetColorAndThickness(const DirectX::XMVECTORF32& color, const float& platformThickness);
	void Initialize(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Render(ID3D11DeviceContext* deviceContext, const Camera& camera);

private:
	void createVertices(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end);

	//rendering
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
	std::unique_ptr<DirectX::BasicEffect> m_effect;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	std::unique_ptr<DirectX::CommonStates> m_states;
	std::vector<DirectX::VertexPositionColor> m_vertices;
	
	DirectX::XMVECTORF32 m_color = DirectX::Colors::White;
	float m_thickness = 1;
};

