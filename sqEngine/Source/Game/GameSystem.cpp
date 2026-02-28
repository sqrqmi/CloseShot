#include "framework.h"

// GameSystemクラスを使えるようにする
#include "GameSystem.h"

// Direct3Dクラスを使えるようにする
#include "Source/DirectX/DirectX.h"

//=========================================================
// ゲームの初期化処理
void GameSystem::Initialize()
{
}

//=========================================================
// ゲームの更新処理
void GameSystem::Update()
{
	// 画面を青色で塗りつぶす
	float color[4] = { 0.2f, 0.2f, 1.0f, 1.0f };
	D3D.mDeviceContext->ClearRenderTargetView(D3D.mBackBufferView.Get(), color);

	// 四角形の描画
	{
		std::vector<VertexType2D> squares =
		{ 
			{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f) },	// 左下
			{ DirectX::XMFLOAT3(-0.5f,  0.5f, 0.0f) },	// 左上
			{ DirectX::XMFLOAT3( 0.5f, -0.5f, 0.0f) },	// 右下
			{ DirectX::XMFLOAT3( 0.5f,  0.5f, 0.0f) },	// 右上
		};

		// 2D描画
		D3D.Draw2D(squares, squares.size());
	}

	// バックバッファの内容を画面に表示
	D3D.mSwapChain->Present(1, 0);
}