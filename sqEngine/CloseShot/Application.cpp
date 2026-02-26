#include "pch.h"
#include "Application.h"

//=================================================
// アプリ初期化処理
void Application::Initialize(HINSTANCE hInst)
{
	if (window_.Initialize(hInst))
	{ MessageBox(nullptr, L"window初期化処理完了", L"完了", MB_OK); }
	else 
	{ MessageBox(nullptr, L"window初期化処理失敗", L"エラー", MB_OK); }
	
	if (renderer_.Initialize(window_.GetWindowHandle()))
	{
		MessageBox(nullptr, L"renderer初期化処理完了", L"完了", MB_OK);
		isRendererInitializeFailed_ = false;
	}
	else
	{
		MessageBox(nullptr, L"renderer初期化処理失敗", L"エラー", MB_OK);
		isRendererInitializeFailed_ = true;
	}
}

//=================================================
// アプリ更新処理
void Application::Update()
{
	// rendererの初期化に失敗していたら再生成を試みる
	while( isRendererInitializeFailed_ )
	{
		int respons = MessageBox(nullptr, L"rendererの初期化に失敗しました。再生成を試みますか。", L"確認", MB_YESNO | MB_ICONQUESTION);
		if (respons == IDNO) { return; }

		if( renderer_.Initialize(window_.GetWindowHandle()) )
		{
			MessageBox(nullptr, L"renderer初期化処理完了", L"完了", MB_OK);
			isRendererInitializeFailed_ = false;
			break;
		}
		else
		{
			MessageBox(nullptr, L"renderer初期化処理失敗", L"エラー", MB_OK);
			isRendererInitializeFailed_ = true;
		}
	}

	while (true)
	{
		bool result = window_.MessageUpdate();
		if (result == false) { break; }

		result = GameUpdate();
		if (result == false) { break; }
	}
}

//=================================================
// アプリ終了処理
void Application::Finalize()
{
	window_.Finalize();

	if (isRendererInitializeFailed_ == false)
	{
		renderer_.Finalize();
	}
}

//=================================================
// ゲームメインループ
bool Application::GameUpdate()
{
	renderer_.Draw();

	renderer_.Swap();

	return true;
}