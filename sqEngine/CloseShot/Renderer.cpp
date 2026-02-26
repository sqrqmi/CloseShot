#include "pch.h"
#include "Renderer.h"

//=================================================
Renderer::Renderer()
{

}

//=================================================
Renderer::~Renderer()
{

}

//=================================================
// 描画初期化処理
bool Renderer::Initialize(HWND hWindow)
{
	// Windowに合わせてスクリーンサイズを初期化
	RECT rc;
	GetClientRect(hWindow, &rc);
	screenWidth_ = rc.right - rc.left;
	screenHeight_ = rc.bottom - rc.top;

	// デバイスとスワップチェインの初期化に失敗したら初期化失敗
	if (!Initialize_DeviceAndSwapChain(hWindow))
	{
		MessageBox(nullptr, L"DeviceAndSwapChain生成失敗", L"エラー", MB_OK);
		return false;
	}

	// バックバッファの初期化に失敗したら初期化失敗
	if (!Initialize_BackBuffer())
	{
		MessageBox(nullptr, L"BackBuffer生成失敗", L"エラー", MB_OK);
		return false;
	}

	// シェーダをコンパイルして設定（ファイル名は実行時のカレントディレクトリ基準）
	// 必要に応じて絶対パスに変更してください
	if (!CompileShader(L"VertexShader.hlsl", L"PixelShader.hlsl", DefaultShader))
	{
		MessageBox(nullptr, L"Shader生成失敗", L"エラー", MB_OK);
		return false;
	}

	// 頂点バッファの作成に失敗したら初期化失敗
	if (!SampleTriangle.CreateVertexBuffer(*this))
	{
		MessageBox(nullptr, L"VertexBuffer生成失敗", L"エラー", MB_OK);
		return false;
	}

	return true;
}

//=================================================
// 描画終了処理
void Renderer::Finalize()
{
	// デバイス・ステートのクリア
	if (pImmediateContext_) { pImmediateContext_->ClearState(); }

	// スワップチェインをウィンドウモードにする
	if (pSwapChain_) { pSwapChain_->SetFullscreenState(FALSE, nullptr); }

	// 取得したインターフェイスの解放
	DX_SAFE_RELEASE(pRenderTargetView_);
	DX_SAFE_RELEASE(pSwapChain_);
	DX_SAFE_RELEASE(pImmediateContext_);
	DX_SAFE_RELEASE(pD3DDevice_);
}

//=================================================
// バックバッファに描画
void Renderer::Draw()
{
	// 出力先をセット
	pImmediateContext_->OMSetRenderTargets(1, &pRenderTargetView_, nullptr);

	// 背景クリア
	float color[] = { 0.f, 0.f, 0.f, 0.f };
	pImmediateContext_->ClearRenderTargetView(pRenderTargetView_, color);

	// シェーダと入力レイアウトを必ずバインドする
	if (DefaultShader.pInputLayout)
	{
		pImmediateContext_->IASetInputLayout(DefaultShader.pInputLayout);
	}
	if (DefaultShader.pVertexShader)
	{
		pImmediateContext_->VSSetShader(DefaultShader.pVertexShader, nullptr, 0);
	}
	if (DefaultShader.pPixelShader)
	{
		pImmediateContext_->PSSetShader(DefaultShader.pPixelShader, nullptr, 0);
	}

	pImmediateContext_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 三角形描画（Triangle::Draw 内でも追加ガードあり）
	SampleTriangle.Draw(*this);
}

//=================================================
// 画面に表示
void Renderer::Swap()
{
	// バックバッファの表示（画面をすぐに更新）
	if (!pSwapChain_)
	{
		// SwapChain が無い場合は無視
		return;
	}

	HRESULT hr = pSwapChain_->Present(0, 0);

	if (FAILED(hr)) { return; }
}

//=================================================
bool Renderer::Initialize_DeviceAndSwapChain(HWND hWindow)
{
	// デバイスとスワップチェイン作成
	DXGI_SWAP_CHAIN_DESC sd = {};
	ZeroMemory(&sd, sizeof(sd));

	sd.BufferCount							= 1;										// バックバッファの数（レガシー用）
	sd.BufferDesc.Width						= screenWidth_;								// バックバッファの横幅
	sd.BufferDesc.Height					= screenHeight_;							// バックバッファの高さ
	sd.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;				// フォーマット
	sd.BufferDesc.RefreshRate.Numerator		= 60;										// リフレッシュレート（分母）
	sd.BufferDesc.RefreshRate.Denominator	= 1;										// リフレッシュレート（分子）
	sd.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;			// バックバッファの使用法
	sd.OutputWindow							= hWindow;									// 関連付けるウィンドウ
	sd.SampleDesc.Count						= 1;										// マルチサンプル（アンチエイリアス）の数
	sd.SampleDesc.Quality					= 0;										// マルチサンプル（アンチエイリアス）のクオリティ
	sd.Windowed								= TRUE;										// ウィンドウモード（TRUEがウィンドウモード）
	sd.Flags								= 0;										// モード自動切り換え

#if defined(DEBUG) || defined(_DEBUG)
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#else
	UINT createDeviceFlags = 0;
#endif

	const D3D_DRIVER_TYPE DriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		// D3D_DRIVER_TYPE_REFERENCE,
	};

	HRESULT hr = E_FAIL;
	D3D_DRIVER_TYPE lastTried = D3D_DRIVER_TYPE_UNKNOWN;

	for (auto type : DriverTypes)
	{
		lastTried = type;

		// まずデバイスのみ作成（SwapChain は別途作成して切り分け）
		ID3D11Device* tmpDevice = nullptr;
		ID3D11DeviceContext* tmpContext = nullptr;
		D3D_FEATURE_LEVEL tmpFeatureLevel = D3D_FEATURE_LEVEL_11_0;

		hr = D3D11CreateDevice(
			nullptr,
			type,
			nullptr,
			createDeviceFlags,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&tmpDevice,
			&tmpFeatureLevel,
			&tmpContext
		);

		if (FAILED(hr))
		{
			// デバイス作成失敗なら次のドライバを試す
			DX_SAFE_RELEASE(tmpDevice);
			DX_SAFE_RELEASE(tmpContext);
			continue;
		}

		// デバイス成功 → IDXGIDevice / Adapter / Factory を経由して SwapChain を作成
		IDXGIDevice* pDXGIDevice = nullptr;
		hr = tmpDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
		if (FAILED(hr))
		{
			DX_SAFE_RELEASE(tmpDevice);
			DX_SAFE_RELEASE(tmpContext);
			continue;
		}

		IDXGIAdapter* pAdapter = nullptr;
		hr = pDXGIDevice->GetAdapter(&pAdapter);
		if (FAILED(hr))
		{
			DX_SAFE_RELEASE(pDXGIDevice);
			DX_SAFE_RELEASE(tmpDevice);
			DX_SAFE_RELEASE(tmpContext);
			continue;
		}

		// まず IDXGIFactory2 (フリップモデル) を試す
		IDXGIFactory2* pFactory2 = nullptr;
		hr = pAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&pFactory2);
		if (SUCCEEDED(hr) && pFactory2)
		{
			// フリップモデル用の記述
			DXGI_SWAP_CHAIN_DESC1 sd1 = {};
			sd1.Width = screenWidth_;
			sd1.Height = screenHeight_;
			sd1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			sd1.Stereo = FALSE;
			sd1.SampleDesc.Count = 1;
			sd1.SampleDesc.Quality = 0;
			sd1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd1.BufferCount = 2; // フリップモデルでは 2 以上が推奨
			sd1.Scaling = DXGI_SCALING_STRETCH;
			sd1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			sd1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			sd1.Flags = 0;

			// CreateSwapChainForHwnd を呼ぶ（IDXGISwapChain1 を受け取る）
			IDXGISwapChain1* pSwapChain1 = nullptr;
			hr = pFactory2->CreateSwapChainForHwnd(
				tmpDevice,
				hWindow,
				&sd1,
				nullptr,
				nullptr,
				&pSwapChain1
			);

			if (SUCCEEDED(hr) && pSwapChain1)
			{
				// IDXGISwapChain1 を IDXGISwapChain にキャストして保存
				hr = pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), (void**)&pSwapChain_);
				pSwapChain1->Release();
			}

			// 後片付け
			pFactory2->Release();
		}
		else
		{
			// IDXGIFactory2 が得られない場合は従来の CreateSwapChain を試す
			IDXGIFactory* pFactory = nullptr;
			hr = pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);
			if (SUCCEEDED(hr) && pFactory)
			{
				hr = pFactory->CreateSwapChain(tmpDevice, &sd, &pSwapChain_);
				pFactory->Release();
			}
		}

		// 後片付け（成功したら tmpDevice/tmpContext をメンバへ移す）
		DX_SAFE_RELEASE(pAdapter);
		DX_SAFE_RELEASE(pDXGIDevice);

		if (SUCCEEDED(hr) && pSwapChain_)
		{
			pD3DDevice_ = tmpDevice;
			pImmediateContext_ = tmpContext;
			featureLevelsSupported_ = tmpFeatureLevel;

			if (pD3DDevice_ && pImmediateContext_)
			{
				D3D11_RASTERIZER_DESC rd{};
				rd.FillMode = D3D11_FILL_SOLID;
				rd.CullMode = D3D11_CULL_NONE;
				rd.FrontCounterClockwise = FALSE;
				rd.DepthClipEnable = TRUE;

				ID3D11RasterizerState* pRS = nullptr;
				if (SUCCEEDED(pD3DDevice_->CreateRasterizerState(&rd, &pRS)) && pRS)
				{
					pImmediateContext_->RSSetState(pRS);
					pRS->Release();
				}
			}

			break;
		}

		// 失敗なら確実に解放して次へ
		DX_SAFE_RELEASE(pSwapChain_);
		DX_SAFE_RELEASE(tmpDevice);
		DX_SAFE_RELEASE(tmpContext);
	}

	if (FAILED(hr))
	{
		wchar_t msgBuf[512] = {};
		wchar_t sysMsg[512] = {};
		DWORD len = FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			static_cast<DWORD>(hr & 0xFFFFFFFF),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			sysMsg,
			_countof(sysMsg),
			nullptr
		);

		const wchar_t* driverName = L"Unknown";
		switch (lastTried)
		{
		case D3D_DRIVER_TYPE_HARDWARE: driverName = L"HARDWARE"; break;
		case D3D_DRIVER_TYPE_WARP: driverName = L"WARP"; break;
		case D3D_DRIVER_TYPE_REFERENCE: driverName = L"REFERENCE"; break;
		}

		if (len == 0)
		{
			swprintf_s(msgBuf, L"SwapChain生成失敗\nDriverType: %s\nHRESULT: 0x%08X\n(システムメッセージなし)", driverName, static_cast<unsigned int>(hr));
		}
		else
		{
			swprintf_s(msgBuf, L"SwapChain生成失敗\nDriverType: %s\nHRESULT: 0x%08X\nMessage: %s", driverName, static_cast<unsigned int>(hr), sysMsg);
		}

		OutputDebugStringW(msgBuf);
		MessageBox(nullptr, msgBuf, L"Error", MB_OK);

		return false;
	}

	return true;
}

//=================================================
bool Renderer::Initialize_BackBuffer()
{
	HRESULT hr;

	// スワップチェインから最初のバックバッファを取得する
	ID3D11Texture2D* pBackBuffer;	// バッファのアクセスに使うインターフェース

	hr = pSwapChain_->GetBuffer(
		0,							// バックバッファの番号
		__uuidof(ID3D11Texture2D),	// バッファにアクセスするインターフェース
		(LPVOID*)&pBackBuffer		// バッファを受け取る変数
	);

	if (FAILED(hr)) { return false; }

	// バックバッファの情報
	D3D11_TEXTURE2D_DESC descBackBuffer;
	pBackBuffer->GetDesc(&descBackBuffer);

	// バックバッファの描画ターゲットビューを作る
	hr = pD3DDevice_->CreateRenderTargetView(
		pBackBuffer,		// ビューでアクセスするリソース
		nullptr,			// 描画ターゲットビューの定義
		&pRenderTargetView_	// 描画ターゲットビューを受け取る変数
	);

	DX_SAFE_RELEASE(pBackBuffer);	// 以降、バックバッファは直接使わないので解放

	if (FAILED(hr)) { return false; }

	//ビューポートの設定
	viewPort_[0].TopLeftX	= 0.0f;									// ビューポート領域の左上X座標
	viewPort_[0].TopLeftY	= 0.0f;									// ビューポート領域の左上Y座標
	viewPort_[0].Width		= static_cast<float>(screenWidth_);		// ビューポート領域の横幅
	viewPort_[0].Height		= static_cast<float>(screenHeight_);	// ビューポート領域の高さ
	viewPort_[0].MinDepth	= 0.0f;									// ビューポート領域の深度値の最小値
	viewPort_[0].MaxDepth	= 1.0f;									// ビューポート領域の深度値の最大値
	pImmediateContext_->RSSetViewports(1, &viewPort_[0]);

	return true;
}

//=================================================
bool Renderer::CompileShader(const WCHAR* vsPath, const WCHAR* psPath, Shader& outShader)
{
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errBlob = nullptr;

	outShader.pVertexShader = nullptr;
	outShader.pPixelShader = nullptr;
	outShader.pInputLayout = nullptr;

	auto pDevice = GetDevice();
	if (!pDevice)
	{
		OutputDebugStringW(L"CompileShader: GetDevice() returned nullptr\n");
		return false;
	}

	// 頂点シェーダーコンパイル
	HRESULT hr = D3DCompileFromFile(
		vsPath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_4_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob,
		&errBlob
	);

	if (FAILED(hr))
	{
		if (errBlob)
		{
			OutputDebugStringA(static_cast<const char*>(errBlob->GetBufferPointer()));
			MessageBoxA(nullptr, static_cast<const char*>(errBlob->GetBufferPointer()), "Vertex shader compile error", MB_OK);
			errBlob->Release();
			errBlob = nullptr;
		}
		DX_SAFE_RELEASE(vsBlob);
		return false;
	}

	// 頂点シェーダー作成
	ID3D11VertexShader* pVertexShader = nullptr;
	hr = pDevice->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		nullptr,
		&pVertexShader
	);

	if (FAILED(hr))
	{
		OutputDebugStringW(L"CompileShader: CreateVertexShader failed\n");
		DX_SAFE_RELEASE(vsBlob);
		DX_SAFE_RELEASE(errBlob);
		return false;
	}

	// 入力レイアウト作成
	ID3D11InputLayout* pInputLayout = nullptr;
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = pDevice->CreateInputLayout(
		layout,
		_countof(layout),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&pInputLayout
	);

	if (FAILED(hr))
	{
		OutputDebugStringW(L"CompileShader: CreateInputLayout failed\n");
		DX_SAFE_RELEASE(pVertexShader);
		DX_SAFE_RELEASE(vsBlob);
		DX_SAFE_RELEASE(errBlob);
		return false;
	}

	// 頂点用の blob はもう不要（入力レイアウト作成後でも安全に解放可）
	DX_SAFE_RELEASE(vsBlob);
	DX_SAFE_RELEASE(errBlob);

	// ピクセルシェーダーコンパイル
	hr = D3DCompileFromFile(
		psPath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_4_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob,
		&errBlob
	);

	if (FAILED(hr))
	{
		if (errBlob)
		{
			OutputDebugStringA(static_cast<const char*>(errBlob->GetBufferPointer()));
			MessageBoxA(nullptr, static_cast<const char*>(errBlob->GetBufferPointer()), "Pixel shader compile error", MB_OK);
			errBlob->Release();
			errBlob = nullptr;
		}
		DX_SAFE_RELEASE(pInputLayout);
		DX_SAFE_RELEASE(pVertexShader);
		DX_SAFE_RELEASE(psBlob);
		return false;
	}

	// ピクセルシェーダー作成
	ID3D11PixelShader* pPixelShader = nullptr;
	hr = pDevice->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		nullptr,
		&pPixelShader
	);

	if (FAILED(hr))
	{
		OutputDebugStringW(L"CompileShader: CreatePixelShader failed\n");
		DX_SAFE_RELEASE(pInputLayout);
		DX_SAFE_RELEASE(pVertexShader);
		DX_SAFE_RELEASE(psBlob);
		DX_SAFE_RELEASE(errBlob);
		return false;
	}

	// 成功したらアウトに渡す
	outShader.pVertexShader = pVertexShader;
	outShader.pPixelShader = pPixelShader;
	outShader.pInputLayout = pInputLayout;

	// クリーンアップ
	DX_SAFE_RELEASE(psBlob);
	DX_SAFE_RELEASE(errBlob);

	return true;
}