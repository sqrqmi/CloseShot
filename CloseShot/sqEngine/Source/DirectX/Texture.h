#pragma once

#include "DirectX.h"

//=========================================================
// テクスチャクラス

class Texture
{
public:

	// シェーダーリソースビュー（画像データ読み取りハンドル）
	ComPtr<ID3D11ShaderResourceView> mSrv = nullptr;
	// 画像情報
	DirectX::TexMetadata mInfo = {};

	// 画像ファイルを読み込む
	bool Load(const std::string& filename);
};