#include "pch.h"
#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Super()
{
	direction = GetTransform()->GetLook();
}

DirectionalLight::DirectionalLight(
	XMFLOAT4 ambient, 
	XMFLOAT4 diffuse, 
	XMFLOAT4 specular) : 
	Super(ambient, diffuse, specular)
{

}

DirectionalLight::~DirectionalLight()
{

}

void DirectionalLight::Init()
{
	_matProj = MathHelper::Identity4x4();

	direction = GetTransform()->GetLook();

	RENDER->AddLight(this);
}

void DirectionalLight::Update()
{
	if (GetGameObject()->GetFramesDirty() > 0)
	{
		direction = GetTransform()->GetLook();

		XMVECTOR eyePos = XMLoadFloat3(&GetTransform()->GetPosition());
		XMVECTOR targetPos = eyePos + XMLoadFloat3(&direction);
		XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

		XMMATRIX matView = XMMatrixLookAtLH(eyePos, targetPos, upVec);
		XMStoreFloat4x4(&_matView, XMMatrixTranspose(matView));

		// projMat 갱신 부분 추가해야함
		auto shadowMapViewport = RENDER->GetShadowMap()->GetViewport();
		XMMATRIX matProj = XMMatrixOrthographicLH(
			80.0f,
			80.0f,
			-50.0f,
			50.0f
		);
		XMStoreFloat4x4(&_matProj, XMMatrixTranspose(matProj));

		SetFramesDirty();
	}
}

LightConstants DirectionalLight::GetLightConstants()
{
	LightConstants constants;
	constants.View = _matView;
	constants.Proj = _matProj;
	constants.Ambient = ambient;
	constants.Diffuse = diffuse;
	constants.Specular = specular;
	constants.Direction = direction;

	return constants;
}
