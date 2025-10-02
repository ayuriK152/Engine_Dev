#pragma once
#include "pch.h"

#define		CONSTANT_MAX_GENERAL_LIGHT		20

#define		MAX_BONE_COUNT			128
#define		MAX_KEYFRAME_COUNT	    300

#define		DIRECT_LIGHT	 0
#define		POINT_LIGHT		 1
#define		SPOT_LIGHT		 2

struct InstanceConstants
{
	XMFLOAT4X4 World = MathHelper::Identity4x4();
	XMFLOAT4X4 WorldInv = MathHelper::Identity4x4();
	UINT MaterialIndex = 0;
};

struct MaterialConstants
{
	XMFLOAT4X4 matTransform = MathHelper::Identity4x4();
	Color Ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
	Color Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	Color Specular = { 0.01f, 0.01f, 0.01f, 1.0f };
	Color Emissive = { 0.0f, 0.0f, 0.0f, 0.0f };
	Vector2 Tilling = { 1.0f, 1.0f };
	float Shiness = 0.25f;
	UINT DiffuseMapIndex;
};

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

struct LightConstants
{
	XMFLOAT4X4 View = MathHelper::Identity4x4();
	XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	XMFLOAT4 Ambient = { 0.0f, 0.0f, 0.0f, 0.0f };
	XMFLOAT4 Diffuse = { 0.0f, 0.0f, 0.0f, 0.0f };
	XMFLOAT4 Specular = { 0.0f, 0.0f, 0.0f, 0.0f };
	XMFLOAT3 Direction = { 0.0f, 0.0f, 0.0f };
	UINT LightType = DIRECT_LIGHT;
	XMFLOAT2 FalloffInfo = { 0.0f, 0.0f };
	float SpotPower = 0.0f;
	int padding = 0;
};

struct LightGatherConstants
{
	LightConstants GlobalLight;
	LightConstants Lights[CONSTANT_MAX_GENERAL_LIGHT];
};

struct KeyFrameConstants
{
	XMFLOAT4X4 Transform = MathHelper::Identity4x4();
};

struct AnimationDataConstants
{
	KeyFrameConstants KeyFrames[MAX_KEYFRAME_COUNT];
};

struct AnimationConstants
{
	AnimationDataConstants AnimationDatas[MAX_BONE_COUNT];
};

struct AnimationStateConstants
{
	float CurrentTick;
	UINT CurrentAnimIdx;
	float TransitionTick;
	UINT NextAnimIdx;
	bool IsOnTransition;
};
