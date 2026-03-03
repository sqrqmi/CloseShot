#include "Camera.h"

#include "framework.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

//=========================================================
// カメラの初期化処理
void Camera::Initialize()
{
	CameraPosition = DirectX::XMFLOAT3(-5.0f, 0, -5.0f);
	FocusPoint = DirectX::XMFLOAT3(0, 0, 0);
	Movement = { 0.f, 0.025f, 0.f };

	UpVector = { 0, 1.0f, 0 };
}

//=========================================================
// カメラの更新処理
void Camera::Update()
{
	//return;

	// カメラの位置を移動量分だけ移動させる
	CameraPosition.x += Movement.x;
	CameraPosition.y += Movement.y;
	CameraPosition.z += Movement.z;

	if (CameraPosition.y > 1000.f)
	{
		CameraPosition.y = 0.0f;
		CameraPosition.z = 0.0f;
	}
}

//=========================================================
// ビュー変換行列の取得
DirectX::XMMATRIX Camera::GetViewMatrix() const
{
	using namespace DirectX;

	XMVECTOR eye = XMLoadFloat3(&CameraPosition);
	XMVECTOR focus = XMLoadFloat3(&FocusPoint);
	XMVECTOR up = XMLoadFloat3(&UpVector);

	if( XMVector3Equal(eye, focus) )
	{
		focus = XMVectorAdd(eye, XMVectorSet(0, 0, 1.0f, 0));
	}

	auto viewMatrix = XMMatrixLookAtLH(eye, focus, up);

	char buffer[256];
	sprintf_s(buffer, "CameraPosition: (%.2f, %.2f, %.2f)\n", CameraPosition.x, CameraPosition.y, CameraPosition.z);
	OutputDebugStringA(buffer);

	return viewMatrix;
}