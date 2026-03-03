#pragma once

// Direct3Dのライブラリを使用できるようにする
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// Direct3D型・クラス・関数などを呼べるようにする
#include <d3d11.h>
#include <d3dcompiler.h>

// DirectXMathを使用できるようにする
#include <DirectXMath.h>

// ComPtrを使用できるようにする
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

// DirectXTexを使用できるようにする
#include <DirectXTex.h>

// 自作のDirectX関係のヘッダーをインクルード（適宜追記する）
#include "Direct3D.h"