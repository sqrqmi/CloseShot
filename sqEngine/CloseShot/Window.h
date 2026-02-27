#pragma once
#include <Windows.h>

class Window
{
public:

	Window();
	virtual ~Window();

	// ウィンドウ初期化処理
	bool Initialize(HINSTANCE hInst);

	// ウィンドウ終了処理
	bool Finalize();

	bool MessageUpdate();

	static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void OnResize(UINT width, UINT height);

	HWND GetWindowHandle() { return hWindow_; }

private:

	WNDCLASS	wc_;
	HWND		hWindow_ = nullptr;
	SIZE		sizeWindow_;
	MSG			msg_ = {};

};