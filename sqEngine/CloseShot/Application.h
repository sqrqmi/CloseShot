#pragma once

#include "Window.h"
#include "Renderer.h"

class Application
{
public:

	// アプリケーション初期化処理
	void Initialize(HINSTANCE hInst);

	// アプリケーション更新処理
	void Update();

	// アプリケーション終了処理
	void Finalize();

private:

	// ゲームのメインループ
	bool GameUpdate();

private:

	Window window_;
	Renderer renderer_;

	// レンダラーの初期化失敗フラグ
	bool isRendererInitializeFailed_ = false;
};