#include "pch.h"
#include "DirectionalLight.h"

REGISTER_COMPONENT(DirectionalLight);

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
	cout << "Released - DirectionalLight:" << _id << "\n";
}

void DirectionalLight::Init()
{
	_matProj = MathHelper::Identity4x4();

	_transform = GetTransform();
	direction = _transform->GetLook();

	RENDER->AddLight(this);
}

void DirectionalLight::Update()
{
	if (GetGameObject()->GetFramesDirty() > 0)
	{
		direction = _transform->GetLook();

		_transform->LookAtWithNoRoll(_transform->GetPosition() + direction);

		XMVECTOR eyePos = XMLoadFloat3(&_transform->GetPosition());
		XMVECTOR targetPos = eyePos + XMLoadFloat3(&direction);
		XMVECTOR upVec = XMLoadFloat3(&_transform->GetUp());

		XMMATRIX matView = XMMatrixLookAtLH(eyePos, targetPos, upVec);
		XMStoreFloat4x4(&_matView, XMMatrixTranspose(matView));

		// projMat 갱신 부분 추가해야함
		auto shadowMapViewport = RENDER->GetShadowMap()->GetViewport();
		XMMATRIX matProj = XMMatrixOrthographicLH(
			50.0f,
			50.0f,
			-50.0f,
			50.0f
		);
		XMStoreFloat4x4(&_matProj, XMMatrixTranspose(matProj));

		SetFramesDirty();
	}
}

void DirectionalLight::OnDestroy()
{
	cout << "OnDestroy - DirectionalLight:" << _id << "\n";

	_transform.reset();
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
