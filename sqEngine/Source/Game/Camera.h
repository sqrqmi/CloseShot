#pragma once
#include "Source/DirectX/DirectX.h"

//=========================================================
// Cameraクラス
//=========================================================

class Camera
{
public:
	
	Camera();
	~Camera();

	void Initialize();

	void Update();

	void SetCameraPosition(const DirectX::XMFLOAT3& position)
	{
		CameraPosition = position;
	}

	DirectX::XMMATRIX GetViewMatrix() const;

	static inline Camera* sInstance;

private:

	DirectX::XMFLOAT3	CameraPosition = { 0, 0, 0 };	// カメラの位置
	DirectX::XMFLOAT3	FocusPoint = { 0, 0, 0 };		// カメラの注視点
	DirectX::XMFLOAT3	Movement = { 0, 1.0f, 0 };		// カメラの移動量

	DirectX::XMFLOAT3	UpVector = { 0, 1.0f, 0 };		// カメラの上方向ベクトル

public:

	// インスタンス作成
	static void CreateInstance()
	{
		DeleteInstance();

		sInstance = new Camera();
	}

	// インスタンス削除
	static void DeleteInstance()
	{
		if (sInstance != nullptr)
		{
			delete sInstance;
			sInstance = nullptr;
		}
	}

	// 唯一のインスタンスを取得
	static Camera& GetInstance()
	{
		return *sInstance;
	}
};

// Camera唯一のインスタンスを簡単に取得するためのマクロ
#define CAM Camera::GetInstance()