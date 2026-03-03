#pragma once

#include "Source/DirectX/Texture.h"
#include "Source/DirectX/Direct3D.h"

//=========================================================
// GameSystemクラス
// 役割：ゲームループ
//=========================================================

class GameSystem
{
public:

	// ゲームの初期化処理
	void Initialize();

	// ゲームの更新処理
	void Update();

//=========================================================
// シングルトンパターン

private:

	// 唯一のインスタンス用のポインタ
	static inline GameSystem* sInstance;
	// コンストラクタはprivateにする
	GameSystem() {};

	// 頂点座標配列
	std::vector<VertexType2D> Triangle;

	// 移動量
	float MoveOffset = 0.01f;

	// 拡大縮小量
	DirectX::XMMATRIX Scale = DirectX::XMMatrixScaling(2.0f, 2.0f, 2.0f);

	// 回転量
	DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(45.0f));

	// 移動量
	DirectX::XMMATRIX Translation = DirectX::XMMatrixTranslation(0.5f, 0.f, 0.f);

	// 合成行列
	DirectX::XMMATRIX WorldMatrix;

public:

	// インスタンス作成
	static void CreateInstance()
	{
		DeleteInstance();

		sInstance = new GameSystem();
	}

	// インスタンス削除
	static void DeleteInstance()
	{
		if ( sInstance != nullptr )
		{
			delete sInstance;
			sInstance = nullptr;
		}
	}

	// 唯一のインスタンスを取得
	static GameSystem& GetInstance()
	{
		return *sInstance;
	}
};

// GameSystemの唯一のインスタンスを簡単に取得するためのマクロ
#define GAMESYS GameSystem::GetInstance()
