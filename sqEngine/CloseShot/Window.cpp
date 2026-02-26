#include "pch.h"
#include "Window.h"
#include <tchar.h>

//=================================================
Window::Window() : sizeWindow_()
{
	sizeWindow_.cx = 640;
	sizeWindow_.cy = 480;
}

//=================================================
Window::~Window()
{

}

//=================================================
// ウィンドウ初期化処理
bool Window::Initialize(HINSTANCE hInst)
{
	ZeroMemory(&wc_, sizeof(wc_));

	//--------------------------------------------------
	// ウィンドウクラスの登録

	wc_.style			= CS_HREDRAW | CS_VREDRAW;
	wc_.lpfnWndProc		= (WNDPROC)Window::MainWndProc;
	wc_.cbClsExtra		= 0;
	wc_.cbWndExtra		= 0;
	wc_.hInstance		= hInst;
	wc_.hIcon			= LoadIcon(nullptr, IDI_APPLICATION);
	wc_.hCursor			= LoadCursor(nullptr, IDC_ARROW);
	wc_.hbrBackground	= GetSysColorBrush(COLOR_WINDOW);
	wc_.lpszMenuName	= nullptr;
	wc_.lpszClassName	= _T("CloseShot");

	if (!RegisterClass(&wc_))
	{
		return false;
	}

	//--------------------------------------------------
	// メインウィンドウ作成

	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = sizeWindow_.cx;
	rect.bottom = sizeWindow_.cy;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	hWindow_ = CreateWindow(
		wc_.lpszClassName,
		_T("ShowWindow"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		hInst,
		nullptr
	);

	if (hWindow_ == nullptr)
	{
		return false;
	}

	//--------------------------------------------------
	// ウィンドウ表示

	ShowWindow(hWindow_, SW_SHOWNORMAL);
	UpdateWindow(hWindow_);

	if (hWindow_ == nullptr)
	{
		MessageBox(nullptr, L"Window生成失敗", L"エラー", MB_OK);
		return false;
	}

	return true;
}

//=================================================
// ウィンドウ終了処理
bool Window::Finalize()
{
	DestroyWindow(GetWindowHandle());

	//--------------------------------------------------
	// ウィンドウクラスの登録解除

	UnregisterClass(wc_.lpszClassName, wc_.hInstance);
	hWindow_ = nullptr;

	return true;
}

//=================================================
bool Window::MessageUpdate()
{
	//--------------------------------------------------
	// Windowsメッセージループ処理

	if (PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg_);
		DispatchMessage(&msg_);
	}

	// アプリケーションが終わるときにmessageが[WM_QUIT]になる
	if (msg_.message == WM_QUIT)
	{
		return false;
	}

	return true;
}

//=================================================
LRESULT CALLBACK Window::MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:

		// アプリ終了
		PostQuitMessage(0);

		return 0;
	}

	// デフォルト処理（必須）
	return DefWindowProc(hWnd, msg, wParam, lParam);
}