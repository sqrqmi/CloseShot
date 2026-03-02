#pragma once

#include "Source/DirectX/Texture.h"

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
