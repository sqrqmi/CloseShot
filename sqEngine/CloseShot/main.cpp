#include <Windows.h>
#include "Application.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
	Application* application = new Application();

	// アプリ初期化
	application->Initialize(hInst);

	// アプリ更新処理
	application->Update();

	// アプリ終了処理
	application->Finalize();

	delete(application);

	return 0;
}