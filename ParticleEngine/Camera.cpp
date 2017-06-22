#include "pch.h"
#include "Camera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Camera::Camera()
{
	SetPerspectiveLens(0.25f*DirectX::XM_PI, 1.0f, 1.0f, 1000.0f);
}


Camera::~Camera()
{
}

DirectX::SimpleMath::Vector3 Camera::GetPosition() const
{
	return m_position;
}

void Camera::SetPosition(float x, float y, float z)
{
	SetPosition(Vector3(x, y, z));
}

void Camera::SetPosition(const DirectX::SimpleMath::Vector3& v)
{
	m_position = v;
	m_viewDirty = true;
}

DirectX::SimpleMath::Vector3 Camera::GetRight() const
{
	return m_right;
}

DirectX::SimpleMath::Vector3 Camera::GetUp() const
{
	return m_up;
}

DirectX::SimpleMath::Vector3 Camera::GetLook() const
{
	return m_look;
}

void Camera::SetPerspectiveLens(float fovY, float aspect, float zn, float zf)
{
	m_proj = XMMatrixPerspectiveFovLH(fovY, aspect, zn, zf);
}

void Camera::SetOrthographicLens(float width, float height, float zNearPlane, float zFarPlane)
{
	m_proj = XMMatrixOrthographicLH(width, height, zNearPlane, zFarPlane);
}

void Camera::LookAt(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& target, const DirectX::SimpleMath::Vector3& worldUp)
{
	Vector3 L = -XMVector3Normalize(XMVectorSubtract(target, pos));
	Vector3 R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	Vector3 U = XMVector3Cross(L, R);

	m_position = pos;
	m_look = L;
	m_right = R;
	m_up = U;

	m_viewDirty = true;
}

void Camera::LookAt(DirectX::SimpleMath::Vector3 worldUp, DirectX::SimpleMath::Vector3 look)
{
	Vector3 L = XMVector3Normalize(look);
	Vector3 R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	Vector3 U = XMVector3Cross(L, R);

	m_look = L;
	m_right = R;
	m_up = U;

	m_viewDirty = true;
}

DirectX::SimpleMath::Matrix Camera::GetView() const
{
	assert(!m_viewDirty);
	return m_view;
}

DirectX::SimpleMath::Matrix Camera::GetProj() const
{
	return m_proj;
}

void Camera::Strafe(float d)
{
	// mPosition += d*mRight
	Vector3 s = Vector3(d);
	Vector3 r = m_right;
	Vector3 p = m_position;
	m_position = XMVectorMultiplyAdd(s, r, p);

	m_viewDirty = true;
}

void Camera::Walk(float d)
{
	// mPosition += d*mLook
	Vector3 s = Vector3(d);
	Vector3 l = m_look;
	Vector3 p = m_position;
	m_position = XMVectorMultiplyAdd(s, l, p);

	m_viewDirty = true;
}

void Camera::Pitch(float angle)
{
	// Rotate up and look vector about the right vector.
	Matrix R = XMMatrixRotationAxis(XMLoadFloat3(&m_right), angle);

	m_up = XMVector3TransformNormal(XMLoadFloat3(&m_up), R);
	m_look = XMVector3TransformNormal(XMLoadFloat3(&m_look), R);

	m_viewDirty = true;
}

void Camera::RotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis.
	Matrix R = XMMatrixRotationY(angle);

	m_right = XMVector3TransformNormal(XMLoadFloat3(&m_right), R);
	m_up = XMVector3TransformNormal(XMLoadFloat3(&m_up), R);
	m_look = XMVector3TransformNormal(XMLoadFloat3(&m_look), R);

	m_viewDirty = true;
}

void Camera::UpdateViewMatrix()
{
	if (m_viewDirty)
	{
		Vector3 R = m_right;
		Vector3 U = m_up;
		Vector3 L = m_look;
		Vector3 P = m_position;

		// Keep camera's axes orthogonal to each other and of unit length.
		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		// U, L already ortho-normal, so no need to normalize cross product.
		R = XMVector3Cross(U, L);

		// Fill in the view matrix entries.
		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		m_right = R;
		m_up = U;
		m_look = L;

		m_view(0, 0) = m_right.x;
		m_view(1, 0) = m_right.y;
		m_view(2, 0) = m_right.z;
		m_view(3, 0) = x;

		m_view(0, 1) = m_up.x;
		m_view(1, 1) = m_up.y;
		m_view(2, 1) = m_up.z;
		m_view(3, 1) = y;

		m_view(0, 2) = m_look.x;
		m_view(1, 2) = m_look.y;
		m_view(2, 2) = m_look.z;
		m_view(3, 2) = z;

		m_view(0, 3) = 0.0f;
		m_view(1, 3) = 0.0f;
		m_view(2, 3) = 0.0f;
		m_view(3, 3) = 1.0f;


		m_viewDirty = false;
	}
}

void Camera::SetOrientation(DirectX::SimpleMath::Quaternion quaternion)
{
	Matrix m = XMMatrixRotationQuaternion(quaternion);
	LookAt(Vector3::UnitY, Vector3(m._31, m._32, m._33));
	m_viewDirty = true;
}
