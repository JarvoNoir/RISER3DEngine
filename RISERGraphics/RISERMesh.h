#pragma once
#include "RISERVertex.h"
#include "RISERVertexBuffer.h"
#include "RISERIndexBuffer.h"
#include "RISERConstantBuffer.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class RISERMesh
{
public:
	RISERMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext,std::vector<RISERVertex>& vertices, std::vector<DWORD>& indices);
	RISERMesh(const RISERMesh& mesh); //copy constructor
	void Draw();
private:
	RISERVertexBuffer<RISERVertex> vertexBuffer;
	RISERIndexBuffer indexBuffer;
	ID3D11DeviceContext* deviceContext;
};