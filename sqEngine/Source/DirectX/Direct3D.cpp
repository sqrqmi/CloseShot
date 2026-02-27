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
	if (FAILED(D3DCompileFromFile(L"Shader/SpriteShader.hlsl", nullptr, nullptr, "VS", "vs_5_0", 0, 0, &compiledVS, nullptr)))
	{
		return false;
	}
	// ピクセルシェーダーを読み込み＆コンパイル
	ComPtr<ID3DBlob> compiledPS;
	if (FAILED(D3DCompileFromFile(L"Shader/SpriteShader.hlsl", nullptr, nullptr, "PS", "ps_5_0", 0, 0, &compiledPS, nullptr)))
	{
		return false;
	}

	// 頂点シェーダー作成
	if (FAILED(mDevice->CreateVertexShader(compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), nullptr, &mSpriteVS)))
	{
		return false;
	}
	// ピクセルシェーダー作成
	if (FAILED(mDevice->CreatePixelShader(compiledPS->GetBufferPointer(), compiledPS->GetBufferSize(), nullptr, &mSpritePS)))
	{
		return false;
	}

	// １頂点の詳細な情報
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// 頂点インプットレイアウト作成
	if (FAILED(mDevice->CreateInputLayout(&layout[0], layout.size(), compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), &mSpriteInputLayout)))
	{
		return false;
	}

	return true;
}

//=========================================================
// 2D描画
void Direct3D::SquareDraw2D(std::vector<VertexType2D>& squares_)
{
	VertexType2D squares[] = {
		squares_[0],
		squares_[1],
		squares_[2],
		squares_[3],
	};

	// 四角形用 頂点バッファ作成準備
	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// デバイスにバインドするときの種類（頂点バッファ、インデックスバッファ、定数バッファなど）
	vbDesc.ByteWidth = sizeof(squares);				// 作成するバッファのバイトサイズ
	vbDesc.MiscFlags = 0;							// その他のフラグ
	vbDesc.StructureByteStride = 0;					// 構造化バッファの場合、その構造体のサイズ
	vbDesc.Usage = D3D11_USAGE_DEFAULT;				// 作成するバッファの使用法
	vbDesc.CPUAccessFlags = 0;

	// デバイスに頂点バッファの作成を依頼する
	ComPtr<ID3D11Buffer> vb;
	D3D11_SUBRESOURCE_DATA initData = { &squares[0], sizeof(squares), 0 };	// 書き込むデータ
	
	// 頂点バッファの作成
	mDevice->CreateBuffer(&vbDesc, &initData, &mVbSquare);

	// 頂点バッファを描画で使えるようにセットする
	UINT stride = sizeof(VertexType2D);
	UINT offset = 0;
	mDeviceContext->IASetVertexBuffers(0, 1, mVbSquare.GetAddressOf(), &stride, &offset);

	// プリミティブ・トロポジーをセット
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 2D用頂点シェーダーセット
	mDeviceContext->VSSetShader(mSpriteVS.Get(), 0, 0);
	// 2D用ピクセルシェーダーセット
	mDeviceContext->PSSetShader(mSpritePS.Get(), 0, 0);
	// 入力レイアウトセット
	mDeviceContext->IASetInputLayout(mSpriteInputLayout.Get());

	// デバイスコンテキストに描画を依頼する
	mDeviceContext->Draw(4, 0);
}