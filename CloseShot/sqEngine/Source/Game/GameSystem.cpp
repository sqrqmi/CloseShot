#include "framework.h"

// GameSystemクラスを使えるようにする
#include "GameSystem.h"

// Direct3Dクラスを使えるようにする
#include "Source/DirectX/DirectX.h"

//=========================================================
// ゲームの初期化処理
void GameSystem::Initialize()
{
	// 画像の読み込み
	mTex.Load("Data/Logo.png");
}

//=========================================================
// ゲームの更新処理
void GameSystem::Update()
{
	// 画面を青色で塗りつぶす
	float color[4] = { 0.2f, 0.2f, 1.0f, 1.0f };
	D3D.mDeviceContext->ClearRenderTargetView(D3D.mBackBufferView.Get(), color);

	// 三角形の描画
	{
		// 2Dモードに切り替える
		D3D.ChangeMode_2D();

		// 2D描画
		D3D.Draw2D(mTex, 0, 0, 1, 1);
	}

	// バックバッファの内容を画面に表示
	D3D.mSwapChain->Present(1, 0);
}