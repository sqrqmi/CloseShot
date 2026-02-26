#pragma once

#define DX_SAFE_RELEASE(x)	{ if(x) { (x)->Release(); (x)=nullptr; } }

#include "Shader.h"
#include "Triangle.h"

class Renderer
{
public:

	Renderer();
	~Renderer();

	// 描画初期化処理
	bool Initialize(HWND hWindow);

	// 描画終了処理
	void Finalize();

	// バックバッファに描画
	void Draw();

	// 画面に表示
	void Swap();

	ID3D11Device* GetDevice() { return pD3DDevice_; }

	ID3D11DeviceContext* GetDeviceContext() { return pImmediateContext_; }

private:

	bool Initialize_DeviceAndSwapChain(HWND hWindow);
	bool Initialize_BackBuffer();
	bool CompileShader(const WCHAR* vsPath, const WCHAR* psPath, Shader& outShader);

private:

	// 機能レベルの配列
	static const UINT FEATURE_LEVELS_NUM = 3;
	D3D_FEATURE_LEVEL pFeatureLevels_[FEATURE_LEVELS_NUM] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	// デバイス作成時に返される機能レベル
	D3D_FEATURE_LEVEL featureLevelsSupported_;

	// デバイス
	ID3D11Device* pD3DDevice_ = nullptr;

	// デバイスコンテキスト
	ID3D11DeviceContext* pImmediateContext_ = nullptr;

	// スワップチェイン
	IDXGISwapChain* pSwapChain_ = nullptr;

	// 描画ターゲット・ビュー
	ID3D11RenderTargetView* pRenderTargetView_ = nullptr;

	// ビューポート
	D3D11_VIEWPORT			viewPort_[1];

	UINT backBufferNum_ = 1;
	UINT screenWidth_ = 0;
	UINT screenHeight_ = 0;

	Shader DefaultShader;
	Triangle SampleTriangle;
};
