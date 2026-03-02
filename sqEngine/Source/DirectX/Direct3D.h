#pragma once

#include "DirectX.h"

class Texture;

// 2D用頂点構造体
struct VertexType2D
{
	DirectX::XMFLOAT3 Pos;	// 座標
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
	// 四角形の2D描画
	// squares_	：四角形の頂点座標配列
	void SquareDraw2D(std::vector<VertexType2D>& squares_);

	//=========================================================
	// 2D描画
	// vertices_	：描画する頂点座標配列
	// vertexCount_	：描画する頂点の数
	bool Draw2D(std::vector<VertexType2D>& vertices_, int vertexCount_);

	//=========================================================
	// 2D座標移動変換
	// vertices_	：変換する頂点座標配列
	// offset_		：頂点座標の移動量
	void Transform2D(std::vector<VertexType2D>& vertices_, const DirectX::XMFLOAT2& offset_);

	//=========================================================
	// 2D座標回転変換
	// vertices_	：変換する頂点座標配列
	// angle_		：頂点座標の回転量
	// center_		：回転の中心座標
	void Rotation2D(std::vector<VertexType2D>& vertices_, float angle_, const DirectX::XMFLOAT2& center_);

	//=========================================================
	// 2D座標拡縮変換
	// vertices_	：変換する頂点座標配列
	// scale_		：頂点座標の拡縮量
	// center_		：拡縮の中心座標
	void Scale2D(std::vector<VertexType2D>& vertices_, const DirectX::XMFLOAT2& scale_, const DirectX::XMFLOAT2& center_);



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