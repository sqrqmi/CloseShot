#include "Mesh.h"

using namespace DirectX;

Mesh::Mesh()
{

}

Mesh::~Mesh()
{
	Finalize();
}

bool Mesh::Initialize(aiMesh* pMeshData_)
{
	// 頂点データ取得
	VertexNum = pMeshData_->mNumVertices;
	Vertices = new VertexType2D[VertexNum];
	for (u_int vertexIdx = 0; vertexIdx < VertexNum; ++vertexIdx) {
		auto& pos = pMeshData_->mVertices[vertexIdx];
		Vertices[vertexIdx].Pos = XMFLOAT3(pos.x, pos.y, pos.z);
		constexpr float COLOR = 0.5f;
		Vertices[vertexIdx].Color = XMFLOAT4(COLOR, COLOR, COLOR, 1.f);
	}

	if (CreateVertexBuffer() == false) {
		return false;
	}

	// 頂点インデックスデータ取得（TriangleList前提）
	IndexNum = pMeshData_->mNumFaces * 3;
	Indices = new u_int[IndexNum];
	for (u_int faceIdx = 0; faceIdx < pMeshData_->mNumFaces; ++faceIdx) {
		auto& face = pMeshData_->mFaces[faceIdx];
		assert(face.mNumIndices == 3);
		for (u_int idx = 0; idx < 3; ++idx) {
			Indices[faceIdx * 3 + idx] = face.mIndices[idx];
		}
	}

	if (CreateIndexBuffer() == false) {
		return false;
	}

	return true;
}

void Mesh::Finalize()
{
	DestroyVertexBuffer();
	DestroyIndexBuffer();
	VertexNum = 0;
	IndexNum = 0;
}

bool Mesh::CreateVertexBuffer()
{
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType2D) * VertexNum;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexSubData;
	vertexSubData.pSysMem = Vertices;

	auto hr = D3D.mDevice->CreateBuffer(
		&vertexBufferDesc,
		&vertexSubData,
		&VertexBuffer
	);

	if (FAILED(hr)) return false;

	return true;
}

bool Mesh::CreateIndexBuffer()
{
	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = IndexNum * 4;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexSubData;
	indexSubData.pSysMem = Indices;

	auto hr = D3D.mDevice->CreateBuffer(
		&indexBufferDesc,
		&indexSubData,
		&IndexBuffer
	);

	if (FAILED(hr)) return false;

	return true;
}

void Mesh::DestroyVertexBuffer()
{
	DX_SAFE_RELEASE(VertexBuffer);
}

void Mesh::DestroyIndexBuffer()
{
	DX_SAFE_RELEASE(IndexBuffer);
}

void Mesh::Draw()
{
	auto pDeviceContext = D3D.mDeviceContext;
	UINT strides[1] = { sizeof(VertexType2D) };
	UINT offsets[1] = { 0 };
	pDeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, strides, offsets);
	pDeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	pDeviceContext->DrawIndexed(IndexNum, 0, 0);
}
