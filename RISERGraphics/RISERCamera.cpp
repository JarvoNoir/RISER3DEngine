#include "RISERCamera.h"

RISERCamera::RISERCamera()
{
	this->pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->posVector = XMLoadFloat3(&this->pos);
	this->rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void RISERCamera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

const XMMATRIX& RISERCamera::GetViewMatrix() const { return this->viewMatrix; }
const XMMATRIX& RISERCamera::GetProjectionMatrix() const { return this->projectionMatrix; }
const XMVECTOR& RISERCamera::GetPositionVector() const { return this->posVector; }
const XMFLOAT3& RISERCamera::GetPositionFloat3() const{ return this->pos; }
const XMVECTOR& RISERCamera::GetRotationVector() const{ return this->rotVector; }
const XMFLOAT3& RISERCamera::GetRotationFloat3() const{ return this->rot; }

void RISERCamera::SetPosition(const XMVECTOR& pos)
{
	XMStoreFloat3(&this->pos, pos);
	this->posVector = pos;
	this->UpdateViewMatrix();
}

void RISERCamera::SetPosition(float x, float y, float z)
{
	this->pos = XMFLOAT3(x, y, z);
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateViewMatrix();
}

void RISERCamera::AdjustPosition(const XMVECTOR& pos)
{
	this->posVector += pos;
	XMStoreFloat3(&this->pos, this->posVector);
	this->UpdateViewMatrix();
}

void RISERCamera::AdjustPosition(float x, float y, float z)
{
	this->pos.x += x;
	this->pos.y += y;
	this->pos.z += z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateViewMatrix();
}

void RISERCamera::SetRotation(const XMVECTOR& rot)
{
	this->rotVector = rot;
	XMStoreFloat3(&this->rot, rot);
	this->UpdateViewMatrix();
}

void RISERCamera::SetRotation(float x, float y, float z)
{
	this->rot = XMFLOAT3(x, y, z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void RISERCamera::AdjustRotation(const XMVECTOR& rot)
{
	this->rotVector += rot;
	XMStoreFloat3(&this->rot, this->rotVector);
	this->UpdateViewMatrix();
}

void RISERCamera::AdjustRotation(float x, float y, float z)
{
	this->rot.x += x;
	this->rot.y += y;
	this->rot.z += z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void RISERCamera::SetLookAtPosition(XMFLOAT3 lookAtPosition)
{
	//if camera position is the same as lookAtPosition, return (should not be looking at same pos)
	if (lookAtPosition.x == this->pos.x && lookAtPosition.y == this->pos.y && lookAtPosition.z == this->pos.z)
		return;
	lookAtPosition.x = this->pos.x - lookAtPosition.x;
	lookAtPosition.y = this->pos.y - lookAtPosition.y;
	lookAtPosition.z = this->pos.z - lookAtPosition.z;

	float pitch = 0.0f;
	if (lookAtPosition.y != 0.0f)
	{
		//distance formula
		const float distance = sqrt(lookAtPosition.x * lookAtPosition.x + lookAtPosition.z * lookAtPosition.z);
		pitch = atan(lookAtPosition.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPosition.x != 0.0f)
		yaw = atan(lookAtPosition.x / lookAtPosition.z);
	if (lookAtPosition.z > 0)
		yaw += XM_PI;

	this->SetRotation(pitch, yaw, 0.0f);
}

const XMVECTOR& RISERCamera::GetForwardVector(){ return this->forwardVector; }
const XMVECTOR& RISERCamera::GetRightVector(){ return this->rightVector; }
const XMVECTOR& RISERCamera::GetLeftVector(){ return this->leftVector; }
const XMVECTOR& RISERCamera::GetBackwardVector(){ return this->backwardVector; }

void RISERCamera::UpdateViewMatrix() //Updates view matrix and also updates the movement vectors
{
	//Calculate camera rotation matrix
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
	//Calculate unit vector of cam target based off camera forward value transformed by cam rotation matrix
	XMVECTOR camTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	//Adjust cam target to be offset by the camera's current position
	camTarget += this->posVector;
	//Calculate up direction based on current rotation
	XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, camRotationMatrix);
	//Rebuild view matrix
	this->viewMatrix = XMMatrixLookAtLH(this->posVector, camTarget, upDir);

	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);
	this->forwardVector = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, rotationMatrix);
	this->rightVector = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, rotationMatrix);
	this->leftVector = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, rotationMatrix);
	this->backwardVector = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, rotationMatrix);
}