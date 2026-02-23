#pragma once

#include "DirectX.h"

class Texture;

// 2D用頂点構造体
struct VertexType2D
{
	DirectX::XMFLOAT3 Pos;	// 座標
	DirectX::XMFLOAT2 UV;	// UV座標
};

//================================================
// Direct3Dクラス
//================================================
class Direct3D
{
public:
	// Direct3Dデバイス（GPUに描画命令を送るための窓口）
	ComPtr<ID3D11Device>			mDevice;
	// Direct3Dデバイスコンテキスト（上記デバイスに対して描画命令を実際に送る場所）
	ComPtr<ID3D11DeviceContext>		mDeviceContext;
	// スワップチェイン（バックバッファと表示画面の入れ替えの管理者）
	ComPtr<IDXGISwapChain>			mSwapChain;
	// バックバッファーのRTビュー（バックバッファを描画先として許可をGPUに教えるもの）
	ComPtr<ID3D11RenderTargetView>	mBackBufferView;

	//=========================================================
	// Direct3Dを初期化し、使用できるようにする関数
	// hWnd		：ウィンドウハンドル（ウィンドウそのものを指すIDのようなもの[参照]）
	// width	：画面の横幅
	// height	：画面の高さ
	bool Initialize(HWND hWnd, int width, int height);

	// 2D描画用のシェーダー
	ComPtr<ID3D11VertexShader>	mSpriteVS = nullptr;	// 頂点シェーダー
	ComPtr<ID3D11PixelShader>	mSpritePS = nullptr;	// ピクセルシェーダー
	ComPtr<ID3D11InputLayout>	mSpriteInputLayout = nullptr;	// 入力レイアウト

	ComPtr<ID3D11Buffer>		mVbSquare;				// 四角形用頂点バッファ

	//=========================================================
	// 2D描画モードにする
	void ChangeMode_2D();

	//=========================================================
	// 2D描画
	// tex	：テクスチャ
	// x	：x座標
	// y	：y座標
	// w	：横幅
	// h	：高さ
	void Draw2D(const Texture& tex, float x, float y, float w, float h);


// シングルトンパターン
private:
	// 唯一のインスタンス用のポインタ
	static inline Direct3D* sInstance;
	// コンストラクタはprivateにする
	Direct3D() {}

public:
	// インスタンス作成
	static void CreateInstance()
	{
		DeleteInstance();

		sInstance = new Direct3D();
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
	static Direct3D& GetInstance()
	{
		return *sInstance;
	}
};

// Direct3Dの唯一のインスタンスを簡単に取得するためのマクロ
#define D3D Direct3D::GetInstance()