//***************************************************************************************
// Camera.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Camera.h"

using namespace DirectX;

FCamera::FCamera()
{
	SetLens(0.25f*MathHelper::Pi, 1.0f, 1.0f, 1000.0f);
}

FCamera::~FCamera()
{
}

XMVECTOR FCamera::GetPosition()const
{
	return XMLoadFloat3(&Position);
}

XMFLOAT3 FCamera::GetPosition3f()const
{
	return Position;
}

void FCamera::SetPosition(float x, float y, float z)
{
	Position = XMFLOAT3(x, y, z);
	bViewDirty = true;
}

void FCamera::SetPosition(const XMFLOAT3& v)
{
	Position = v;
	bViewDirty = true;
}

XMVECTOR FCamera::GetRight()const
{
	return XMLoadFloat3(&Right);
}

XMFLOAT3 FCamera::GetRight3f()const
{
	return Right;
}

XMVECTOR FCamera::GetUp()const
{
	return XMLoadFloat3(&Up);
}

XMFLOAT3 FCamera::GetUp3f()const
{
	return Up;
}

XMVECTOR FCamera::GetLook()const
{
	return XMLoadFloat3(&Look);
}

XMFLOAT3 FCamera::GetLook3f()const
{
	return Look;
}

float FCamera::GetNearZ()const
{
	return NearZ;
}

float FCamera::GetFarZ()const
{
	return FarZ;
}

float FCamera::GetAspect()const
{
	return Aspect;
}

float FCamera::GetFovY()const
{
	return FovY;
}

float FCamera::GetFovX()const
{
	float HalfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f*atan(HalfWidth / NearZ);
}

float FCamera::GetNearWindowWidth()const
{
	return Aspect * NearWindowHeight;
}

float FCamera::GetNearWindowHeight()const
{
	return NearWindowHeight;
}

float FCamera::GetFarWindowWidth()const
{
	return Aspect * FarWindowHeight;
}

float FCamera::GetFarWindowHeight()const
{
	return FarWindowHeight;
}

void FCamera::SetLens(float InFovY, float InAspect, float InZn, float InZf)
{
	// cache properties
	FovY = InFovY;
	Aspect = InAspect;
	NearZ = InZn;
	FarZ = InZf;

	NearWindowHeight = 2.0f * NearZ * tanf( 0.5f*FovY );
	FarWindowHeight  = 2.0f * FarZ * tanf( 0.5f*FovY );

	XMMATRIX P = XMMatrixPerspectiveFovLH(FovY, Aspect, NearZ, FarZ);
	XMStoreFloat4x4(&Proj, P);
}

void FCamera::LookAt(FXMVECTOR InPos, FXMVECTOR InTarget, FXMVECTOR InWorldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(InTarget, InPos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(InWorldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&Position, InPos);
	XMStoreFloat3(&Look, L);
	XMStoreFloat3(&Right, R);
	XMStoreFloat3(&Up, U);

	bViewDirty = true;
}

void FCamera::LookAt(const XMFLOAT3& InPos, const XMFLOAT3& InTarget, const XMFLOAT3& InUp)
{
	XMVECTOR P = XMLoadFloat3(&InPos);
	XMVECTOR T = XMLoadFloat3(&InTarget);
	XMVECTOR U = XMLoadFloat3(&InUp);

	LookAt(P, T, U);

	bViewDirty = true;
}

XMMATRIX FCamera::GetView()const
{
	assert(!bViewDirty);
	return XMLoadFloat4x4(&View);
}

XMMATRIX FCamera::GetProj()const
{
	return XMLoadFloat4x4(&Proj);
}


XMFLOAT4X4 FCamera::GetView4x4f()const
{
	assert(!bViewDirty);
	return View;
}

XMFLOAT4X4 FCamera::GetProj4x4f()const
{
	return Proj;
}

void FCamera::Strafe(float d)
{
	// mPosition += d*mRight
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&Right);
	XMVECTOR p = XMLoadFloat3(&Position);
	XMStoreFloat3(&Position, XMVectorMultiplyAdd(s, r, p));

	bViewDirty = true;
}

void FCamera::Walk(float d)
{
	// Position += d*Look
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&Look);
	XMVECTOR p = XMLoadFloat3(&Position);
	XMStoreFloat3(&Position, XMVectorMultiplyAdd(s, l, p));

	bViewDirty = true;
}

void FCamera::Pitch(float InAngle)
{
	// Rotate up and look vector about the right vector.

	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&Right), InAngle);

	XMStoreFloat3(&Up,   XMVector3TransformNormal(XMLoadFloat3(&Up), R));
	XMStoreFloat3(&Look, XMVector3TransformNormal(XMLoadFloat3(&Look), R));

	bViewDirty = true;
}

void FCamera::RotateY(float InAngle)
{
	// Rotate the basis vectors about the world y-axis.

	XMMATRIX R = XMMatrixRotationY(InAngle);

	XMStoreFloat3(&Right,   XMVector3TransformNormal(XMLoadFloat3(&Right), R));
	XMStoreFloat3(&Up, XMVector3TransformNormal(XMLoadFloat3(&Up), R));
	XMStoreFloat3(&Look, XMVector3TransformNormal(XMLoadFloat3(&Look), R));

	bViewDirty = true;
}

void FCamera::UpdateViewMatrix()
{
	if(bViewDirty)
	{
		XMVECTOR R = XMLoadFloat3(&Right);
		XMVECTOR U = XMLoadFloat3(&Up);
		XMVECTOR L = XMLoadFloat3(&Look);
		XMVECTOR P = XMLoadFloat3(&Position);

		// Keep camera's axes orthogonal to each other and of unit length.
		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		// U, L already ortho-normal, so no need to normalize cross product.
		R = XMVector3Cross(U, L);

		// Fill in the view matrix entries.
		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		XMStoreFloat3(&Right, R);
		XMStoreFloat3(&Up, U);
		XMStoreFloat3(&Look, L);

		View(0, 0) = Right.x;
		View(1, 0) = Right.y;
		View(2, 0) = Right.z;
		View(3, 0) = x;

		View(0, 1) = Up.x;
		View(1, 1) = Up.y;
		View(2, 1) = Up.z;
		View(3, 1) = y;

		View(0, 2) = Look.x;
		View(1, 2) = Look.y;
		View(2, 2) = Look.z;
		View(3, 2) = z;

		View(0, 3) = 0.0f;
		View(1, 3) = 0.0f;
		View(2, 3) = 0.0f;
		View(3, 3) = 1.0f;

		bViewDirty = false;
	}
}


