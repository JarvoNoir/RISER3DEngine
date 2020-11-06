#include "RISERModel.h"

bool RISERModel::Init(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, RISERConstantBuffer<RISERCB_VS_VertexShader>& vertexShader)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->texture = texture;
	this->vertexShader = &vertexShader;

	try
	{
		////Textured Square
		//RISERVertex v[] =
		//{
		//	RISERVertex(-0.5f,  -0.5f, -0.5f, 0.0f, 1.0f), //FRONT Bottom Left   - [0]
		//	RISERVertex(-0.5f,   0.5f, -0.5f, 0.0f, 0.0f), //FRONT Top Left      - [1]
		//	RISERVertex(0.5f,   0.5f, -0.5f, 1.0f, 0.0f), //FRONT Top Right     - [2]
		//	RISERVertex(0.5f,  -0.5f, -0.5f, 1.0f, 1.0f), //FRONT Bottom Right   - [3]
		//	RISERVertex(-0.5f,  -0.5f, 0.5f, 0.0f, 1.0f), //BACK Bottom Left   - [4]
		//	RISERVertex(-0.5f,   0.5f, 0.5f, 0.0f, 0.0f), //BACK Top Left      - [5]
		//	RISERVertex(0.5f,   0.5f, 0.5f, 1.0f, 0.0f), //BACK Top Right     - [6]
		//	RISERVertex(0.5f,  -0.5f, 0.5f, 1.0f, 1.0f), //BACK Bottom Right   - [7]
		//};
		////Load Vertex Data
		//HRESULT hr = this->vertexBuffer.Init(this->device, v, ARRAYSIZE(v));
		//RISERCOM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer.");
		//DWORD indices[] =
		//{
		//	0, 1, 2, //FRONT
		//	0, 2, 3, //FRONT
		//	4, 7, 6, //BACK 
		//	4, 6, 5, //BACK
		//	3, 2, 6, //RIGHT SIDE
		//	3, 6, 7, //RIGHT SIDE
		//	4, 5, 1, //LEFT SIDE
		//	4, 1, 0, //LEFT SIDE
		//	1, 5, 6, //TOP
		//	1, 6, 2, //TOP
		//	0, 3, 7, //BOTTOM
		//	0, 7, 4, //BOTTOM
		//};
		////Load Index Data
		//hr = this->indexBuffer.Init(this->device, indices, ARRAYSIZE(indices));
		//RISERCOM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer.");
		if (!this->LoadModel(filePath))
			return false;
	}
	catch (RISERCOMException& exception)
	{
		RISERErrorLogger::Log(exception);
		return false;
	}
	this->SetPosition(0.0f,0.0f,0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateWorldMatrix();
	return true;
}

void RISERModel::SetTexture(ID3D11ShaderResourceView* texture)
{
	this->texture = texture;
}

void RISERModel::Draw(const XMMATRIX& viewProjectionMatrix)
{
	//Update Constant buffer with WVP Matrix
	this->vertexShader->data.matrix = this->worldMatrix * viewProjectionMatrix; //Calculate World-View-Projection Matrix
	this->vertexShader->data.matrix = XMMatrixTranspose(this->vertexShader->data.matrix);
	this->vertexShader->ApplyChanges();
	this->deviceContext->VSSetConstantBuffers(0, 1, this->vertexShader->GetAddressOf());
	this->deviceContext->PSSetShaderResources(0, 1, &this->texture); //Set Texture
	//Draw
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw();
	}
}

void RISERModel::UpdateWorldMatrix()
{
	//this->worldMatrix = XMMatrixIdentity();
	this->worldMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);
	this->forwardVector = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	this->backwardVector = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	this->leftVector = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	this->rightVector = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
}

bool RISERModel::LoadModel(const std::string& filePath)
{
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if(pScene == nullptr)
		return false;

	this->ProcessNode(pScene->mRootNode, pScene);
	return true;
}

void RISERModel::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(this->ProcessMesh(mesh, scene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], scene);
	}
}

RISERMesh RISERModel::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	// Data to fill
	std::vector<RISERVertex> vertices;
	std::vector<DWORD> indices;

	//Get vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		RISERVertex vertex;

		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}

	//Get indices
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	return RISERMesh(this->device, this->deviceContext, vertices, indices);
}

const XMVECTOR& RISERModel::GetPositionVector() const
{
	return this->posVector;
}

const XMFLOAT3& RISERModel::GetPositionFloat3() const
{
	return this->pos;
}

const XMVECTOR& RISERModel::GetRotationVector() const
{
	return this->rotVector;
}

const XMFLOAT3& RISERModel::GetRotationFloat3() const
{
	return this->rot;
}

void RISERModel::SetPosition(const XMVECTOR& pos)
{
	XMStoreFloat3(&this->pos, pos);
	this->posVector = pos;
	this->UpdateWorldMatrix();
}

void RISERModel::SetPosition(const XMFLOAT3& pos)
{
	this->pos = pos;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateWorldMatrix();
}

void RISERModel::SetPosition(float x, float y, float z)
{
	this->pos = XMFLOAT3(x, y, z);
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateWorldMatrix();
}

void RISERModel::AdjustPosition(const XMVECTOR& pos)
{
	this->posVector += pos;
	XMStoreFloat3(&this->pos, this->posVector);
	this->UpdateWorldMatrix();
}

void RISERModel::AdjustPosition(const XMFLOAT3& pos)
{
	this->pos.x += pos.y;
	this->pos.y += pos.y;
	this->pos.z += pos.z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateWorldMatrix();
}

void RISERModel::AdjustPosition(float x, float y, float z)
{
	this->pos.x += x;
	this->pos.y += y;
	this->pos.z += z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateWorldMatrix();
}

void RISERModel::SetRotation(const XMVECTOR& rot)
{
	this->rotVector = rot;
	XMStoreFloat3(&this->rot, rot);
	this->UpdateWorldMatrix();
}

void RISERModel::SetRotation(const XMFLOAT3& rot)
{
	this->rot = rot;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateWorldMatrix();
}

void RISERModel::SetRotation(float x, float y, float z)
{
	this->rot = XMFLOAT3(x, y, z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateWorldMatrix();
}

void RISERModel::AdjustRotation(const XMVECTOR& rot)
{
	this->rotVector += rot;
	XMStoreFloat3(&this->rot, this->rotVector);
	this->UpdateWorldMatrix();
}

void RISERModel::AdjustRotation(const XMFLOAT3& rot)
{
	this->rot.x += rot.x;
	this->rot.y += rot.y;
	this->rot.z += rot.z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateWorldMatrix();
}

void RISERModel::AdjustRotation(float x, float y, float z)
{
	this->rot.x += x;
	this->rot.y += y;
	this->rot.z += z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateWorldMatrix();
}

void RISERModel::SetLookAtPos(XMFLOAT3 lookAtPos)
{
	//Verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
	if (lookAtPos.x == this->pos.x && lookAtPos.y == this->pos.y && lookAtPos.z == this->pos.z)
		return;

	lookAtPos.x = this->pos.x - lookAtPos.x;
	lookAtPos.y = this->pos.y - lookAtPos.y;
	lookAtPos.z = this->pos.z - lookAtPos.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		yaw += XM_PI;

	this->SetRotation(pitch, yaw, 0.0f);
}

const XMVECTOR& RISERModel::GetForwardVector()
{
	return this->forwardVector;
}

const XMVECTOR& RISERModel::GetRightVector()
{
	return this->rightVector;
}

const XMVECTOR& RISERModel::GetBackwardVector()
{
	return this->backwardVector;
}

const XMVECTOR& RISERModel::GetLeftVector()
{
	return this->leftVector;
}