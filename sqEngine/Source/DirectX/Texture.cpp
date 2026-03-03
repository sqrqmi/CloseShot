#include "framework.h"
#include "Direct3D.h"

#include <locale.h>
#include <memory>

#include "Texture.h"

//=========================================================
// 画像ファイルを読み込む
bool Texture::Load(const std::string& filename)
{
	// マルチバイト文字列からワイド文字列へ変換
	setlocale(LC_CTYPE, "jpn");
	wchar_t wFilename[256];
	size_t ret;
	mbstowcs_s(&ret, wFilename, filename.c_str(), 256);

	// WIC画像を読み込む（WIC : Windows Imaging Component）
	auto image = std::make_unique<DirectX::ScratchImage>();
	if ( FAILED( DirectX::LoadFromWICFile(wFilename, DirectX::WIC_FLAGS_NONE, &mInfo, *image) ) )
	{
		// 失敗
		mInfo = {};
		return false;
	}

	// ミップマップの作成
	if ( mInfo.mipLevels == 1 )
	{
		auto mipChain = std::make_unique<DirectX::ScratchImage>();
		if ( SUCCEEDED( DirectX::GenerateMipMaps(image->GetImages(), image->GetImageCount(), image->GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, *mipChain) ) )
		{
			image = std::move(mipChain);
		}
	}

	// リソースとシェーダーリソースビューを作成
	if ( FAILED( DirectX::CreateShaderResourceView(D3D.mDevice.Get(), image->GetImages(), image->GetImageCount(), mInfo, &mSrv) ) )
	{
		// 失敗
		mInfo = {};
		return false;
	}

	// 成功
	return true;
}