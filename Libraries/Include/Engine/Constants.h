#pragma once
#include "pch.h"

#define DIRECT_LIGHT	 0
#define POINT_LIGHT		 1
#define SPOT_LIGHT		 2

struct CameraConstants
{
	XMFLOAT4X4 View = MathHelper::Identity4x4();
	XMFLOAT4X4 InvView = MathHelper::Identity4x4();
	XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
	XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
	XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
	XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
};

struct ObjectConstants
{
	XMFLOAT4X4 World = MathHelper::Identity4x4();
	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
};

struct LightConstants
{
	XMFLOAT4 Ambient = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMFLOAT4 Diffuse = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMFLOAT4 Specular = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMFLOAT4 Emissive = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMFLOAT3 Direction = { 0.0f, 0.0f, 0.0f };
	UINT LightType = DIRECT_LIGHT;
	XMFLOAT2 FalloffInfo = { 0.0f, 0.0f };
	float SpotPower = 0.0f;
	int padding = 0;
};

struct PassConstants
{
	XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };
	LightConstants Lights[1];
};
