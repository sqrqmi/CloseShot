#include "framework.h"

// GameSystemクラスを使えるようにする
#include "GameSystem.h"

// Direct3Dクラスを使えるようにする
#include "Source/DirectX/DirectX.h"

//=========================================================
// ゲームの初期化処理
void GameSystem::Initialize()
{
	Squares =
	{
		{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f) },	// 左下
		{ DirectX::XMFLOAT3(-0.5f,  0.5f, 0.0f) },	// 左上
		{ DirectX::XMFLOAT3( 0.5f, -0.5f, 0.0f) },	// 右下
		{ DirectX::XMFLOAT3( 0.5f,  0.5f, 0.0f) },	// 右上
	};
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
		// 2D座標変換
		D3D.Transform2D(Squares, DirectX::XMFLOAT2(MoveOffset, MoveOffset));

		// 2D描画
		D3D.Draw2D(Squares, Squares.size());
	}

	// バックバッファの内容を画面に表示
	D3D.mSwapChain->Present(1, 0);
}