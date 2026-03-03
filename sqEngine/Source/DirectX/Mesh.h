#pragma once

#include "DirectX.h"
#include "Direct3D.h"

#include <assimp/mesh.h>

class Mesh
{
public:

	Mesh();
	~Mesh();

	bool Initialize(aiMesh* pMeshData_);

	void Finalize();

	void Draw();

private:

	bool CreateVertexBuffer();
	bool CreateIndexBuffer();
	void DestroyVertexBuffer();
	void DestroyIndexBuffer();

private:
	
	VertexType2D*	Vertices = nullptr;
	UINT*			Indices = nullptr;
	UINT			VertexNum = 0;
	UINT			IndexNum = 0;

	// バッファリソース
	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;
};

#define DX_SAFE_RELEASE(p) { if((p) != nullptr) { (p)->Release(); (p) = nullptr; } }