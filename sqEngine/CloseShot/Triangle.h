#pragma once

#include "pch.h"
#include "Vertex.h"

struct Triangle
{
	static constexpr size_t VERTEX_NUM = 3;
	Vertex Vertices[VERTEX_NUM];
	ID3D11Buffer* VertexBuffer = nullptr;

	Triangle();
	~Triangle();

	bool CreateVertexBuffer(Renderer& renderer_);

	void Draw(Renderer& renderer_);
};