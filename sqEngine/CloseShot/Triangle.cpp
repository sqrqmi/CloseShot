#include "pch.h"
#include "Triangle.h"
#include "Renderer.h"

//=================================================
Triangle::Triangle()
{
	// 頂点のワインディングを反転（CCW順）
	Vertices[0] = { -0.5f, -0.5f, 0.f };
	Vertices[1] = {  0.5f, -0.5f, 0.f };
	Vertices[2] = {  0.0f,  0.5f, 0.f };
}

//=================================================
Triangle::~Triangle()
{
	DX_SAFE_RELEASE(VertexBuffer);
}

//=================================================
bool Triangle::CreateVertexBuffer(Renderer& renderer_)
{
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 3;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexSubData = {};
	vertexSubData.pSysMem = Vertices;

	auto hr = renderer_.GetDevice()->CreateBuffer(
		&vertexBufferDesc,
		&vertexSubData,
		&VertexBuffer
	);

	if (FAILED(hr)) { return false; }

	return true;
}

//=================================================
void Triangle::Draw(Renderer& renderer_)
{
	auto pDeviceContext = renderer_.GetDeviceContext();

	// ガード: コンテキスト／バッファが有効か
	if (!pDeviceContext) { return; }
	if (!VertexBuffer) { return; }

	UINT stride = static_cast<UINT>(sizeof(Vertex));
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->Draw(static_cast<UINT>(VERTEX_NUM), 0);
}