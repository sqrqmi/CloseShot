#pragma once

#include "Vertex.h"

#include "Source/DirectX/Direct3D.h"

struct Triangle
{
	static constexpr size_t VERTEX_NUM = 3;
	Vertex Vertices[VERTEX_NUM];
	ID3D11Buffer* VertexBuffer = nullptr;

	Triangle();
	~Triangle();

	bool CreateVertexBuffer()
};