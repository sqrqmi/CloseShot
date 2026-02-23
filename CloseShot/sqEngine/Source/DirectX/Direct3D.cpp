#include "framework.h"

#include "Direct3D.h"

#include <windows.h>

bool Direct3D::Initialize(HWND hWnd, int width, int height)
{
	//=========================================================
	// ファクトリー作成
	ComPtr<IDXGIFactory> factory;

	// ファクトリーを生成し、失敗したら中断する
	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))))
	{
		return false;
	}

	//=========================================================
	// デバイス生成
	UINT creationFlags = 0;

#ifdef _DEBUG
	// DEBUGビルド時はDirect3Dのデバッグを有効にする（重くなるが、細かいエラーまで見ることができる）
	creationFlags != D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,	// Direct3D 11.1	ShaderModel 5
		D3D_FEATURE_LEVEL_11_0,	// Direct3D 11		ShaderModel 5
		D3D_FEATURE_LEVEL_10_1,	// Direct3D 10.1	ShaderModel 4
		D3D_FEATURE_LEVEL_10_0,	// Direct3D 10.0	ShaderModel 4
		D3D_FEATURE_LEVEL_9_3,	// Direct3D 9.3		ShaderModel 3
		D3D_FEATURE_LEVEL_9_2,	// Direct3D 9.2		ShaderModel 3
		D3D_FEATURE_LEVEL_9_1,	// Direct3D 9.1		ShaderModel 3
	};

	//デバイスとでデバイスコンテキストを作成
	D3D_FEATURE_LEVEL futureLevel;
	if (FAILED(D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		featureLevels,
		_countof(featureLevels),
		D3D11_SDK_VERSION,
		&mDevice,
		&futureLevel,
		&mDeviceContext)))
	{
		return false;
	}

	//=========================================================
	// スワップチェイン作成
	DXGI_SWAP_CHAIN_DESC scDesc = {};		// スワップチェインの設定データ
	scDesc.BufferDesc.Width = width;		// 画面の横幅
	scDesc.BufferDesc.Height = height;		// 画面の高さ
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// バッファの形式
	scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scDesc.BufferDesc.RefreshRate.Numerator = 0;
	scDesc.BufferDesc.RefreshRate.Denominator = 1;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// バッファの使用方法
	scDesc.BufferCount = 2;									// バッファの数
	scDesc.OutputWindow = hWnd;
	scDesc.Windowed = TRUE;									// ウィンドウモード
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// スワップチェインの作成
	if( FAILED( factory->CreateSwapChain(mDevice.Get(), &scDesc, &mSwapChain) ) )
	{
		return false;
	}

	// スワップチェインからバックバッファリソース取得
	ComPtr<ID3D11Texture2D> pBackBuffer;
	if ( FAILED( mSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)) ) )
	{
		return false;
	}

	// バックバッファリソース用のRTVを作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = scDesc.BufferDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	if ( FAILED( mDevice->CreateRenderTargetView(pBackBuffer.Get(), &rtvDesc, &mBackBufferView) ) )
	{
		return false;
	}

	//=========================================================
	// デバイスコンテキストに描画に関する設定を行う

	// バックバッファをRTとしてセット
	mDeviceContext->OMSetRenderTargets(1, mBackBufferView.GetAddressOf(), nullptr);

	// ビューポートの設定
	D3D11_VIEWPORT vp = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
	mDeviceContext->RSSetViewports(1, &vp);

	//=========================================================
	// シェーダーの作成

	// 頂点シェーダーを読み込み＆コンパイル
	ComPtr<ID3DBlob> compiledVS;
	if ( FAILED( D3DCompileFromFile(L"Shader/SpriteShader.hlsl", nullptr, nullptr, "VS", "vs_5_0", 0, 0, &compiledVS, nullptr) ) )
	{
		return false;
	}

	// ピクセルシェーダーを読み込み＆コンパイル
	ComPtr<ID3DBlob> compiledPS;
	if ( FAILED( D3DCompileFromFile(L"Shader/SpriteShader.hlsl", nullptr, nullptr, "PS", "ps_5_0", 0, 0, &compiledPS, nullptr) ) )
	{
		return false;
	}

	// 頂点シェーダー作成
	if ( FAILED( mDevice->CreateVertexShader(compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), nullptr, &mSpriteVS) ) )
	{
		return false;
	}

	// ピクセルシェーダー作成
	if ( FAILED( mDevice->CreatePixelShader(compiledPS->GetBufferPointer(), compiledPS->GetBufferSize(), nullptr, &mSpritePS) ) )
	{
		return false;
	}

	// 1頂点の詳細な情報
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,	0,	0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXUV",		0, DXGI_FORMAT_R32G32_FLOAT,	0,	12,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// 頂点インプットレイアウト作成
	if ( FAILED( mDevice->CreateInputLayout(&layout[0], layout.size(), compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), &mSpriteInputLayout) ) )
	{
		return false;
	}

	return true;
}

//=========================================================
// 2D描画モードにする
void Direct3D::ChangeMode_2D()
{
	// 2D用頂点シェーダーセット
	mDeviceContext->VSSetShader(mSpriteVS.Get(), 0, 0);
	// 2D用ピクセルシェーダーセット
	mDeviceContext->PSSetShader(mSpritePS.Get(), 0, 0);
	// 入力レイアウトセット
	mDeviceContext->IASetInputLayout(mSpriteInputLayout.Get());

	// 四角形用 頂点バッファ作成（初回のみ）
	if ( mVbSquare == nullptr)
	{
		D3D11_BUFFER_DESC vbDesc = {};
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// デバイスにバインドするときの種類（頂点バッファ、インデックスバッファ、定数バッファなど）
		vbDesc.ByteWidth = sizeof(VertexType2D) * 4;	//作成するバッファのバイトサイズ
		vbDesc.MiscFlags = 0;							//その他のフラグ
		vbDesc.StructureByteStride = 0;					//構造化バッファの場合、その構造体のサイズ
		vbDesc.Usage = D3D11_USAGE_DYNAMIC;				//作成するバッファの使用法
		vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		mDevice->CreateBuffer(&vbDesc, nullptr, &mVbSquare);
	}

	// 頂点バッファを描画で使えるようにセットする
	UINT stride = sizeof(VertexType2D);
	UINT offset = 0;
	mDeviceContext->IASetVertexBuffers(0, 1, mVbSquare.GetAddressOf(), &stride, &offset);

	// プリミティブ・トロポジーをセット
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// サンプラーステートを作成しセットする
	{
		// 異方性フィルタリング補間、Wrapモード
		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// 線形フィルタリング
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		// テクスチャアドレッシングモードをWrapに
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		// テクスチャアドレッシングモードをWrapに
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;		// テクスチャアドレッシングモードをWrapに
		desc.MipLODBias = 0;
		desc.MaxAnisotropy = 0;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		// ステートオブジェクト作成
		ComPtr<ID3D11SamplerState> state;
		mDevice->CreateSamplerState(&desc, &state);

		// 各シェーダーの0番目にセット（実際は必要なシェーダーのみセット）
		mDeviceContext->VSSetSamplers(0, 1, state.GetAddressOf());	// 頂点シェーダーの0番目にセット
		mDeviceContext->PSSetSamplers(0, 1, state.GetAddressOf());	// ピクセルシェーダーの0番目にセット
		mDeviceContext->GSSetSamplers(0, 1, state.GetAddressOf());	// ジオメトリシェーダーの0番目にセット
		mDeviceContext->CSSetSamplers(0, 1, state.GetAddressOf());	// コンピュートシェーダーの0番目にセット
	}
}

//=========================================================
// 2D描画
void Direct3D::Draw2D(const Texture& tex, float x, float y, float w, float h)
{
	float hW = w * 0.5f;
	float hH = h * 0.5f;

	// 頂点データ作成
	VertexType2D v[4] = {
		{{x - hW, y - hH, 0}, {0, 1}},	// 左下
		{{x - hW, y + hH, 0}, {0, 0}},	// 左上
		{{x + hW, y - hH, 0}, {1, 1}},	// 右下
		{{x + hW, y + hH, 0}, {1, 0}},	// 右上
	};

	// 頂点バッファにデータを書き込む
	D3D11_MAPPED_SUBRESOURCE pData;
	if ( SUCCEEDED( mDeviceContext->Map(mVbSquare.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pData) ) )
	{
		// データコピー
		memcpy_s(pData.pData, sizeof(v), &v[0], sizeof(v));

		mDeviceContext->Unmap(mVbSquare.Get(), 0);
	}

	// テクスチャをスロット0にセット
	mDeviceContext->PSSetShaderResources(0, 1, tex.mSrv.GetAddressOf());

	// デバイスコンテキストに描画を依頼する
	mDeviceContext->Draw(4, 0);
}