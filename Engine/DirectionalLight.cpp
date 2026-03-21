#include "pch.h"
#include "DirectionalLight.h"

REGISTER_COMPONENT(DirectionalLight);

DirectionalLight::DirectionalLight() : Super()
{
	//direction = GetTransform()->GetLook();
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

	RENDER->AddLight(static_pointer_cast<Light>(shared_from_this()));
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

	RENDER->DeleteLight(static_pointer_cast<Light>(shared_from_this()));

	_transform.reset();
}

void DirectionalLight::LoadXML(XMLElement* compElem)
{
	XMLElement* ambientElem = compElem->FirstChildElement("Ambient");
	if (ambientElem) {
		ColorRGBA color;
		color.x = ambientElem->FloatAttribute("r");
		color.y = ambientElem->FloatAttribute("g");
		color.z = ambientElem->FloatAttribute("b");
		color.w = ambientElem->FloatAttribute("a");
		ambient = color;
	}

	XMLElement* diffuseElem = compElem->FirstChildElement("Diffuse");
	if (diffuseElem) {
		ColorRGBA color;
		color.x = diffuseElem->FloatAttribute("r");
		color.y = diffuseElem->FloatAttribute("g");
		color.z = diffuseElem->FloatAttribute("b");
		color.w = diffuseElem->FloatAttribute("a");
		diffuse = color;
	}

	XMLElement* specularElem = compElem->FirstChildElement("Specular");
	if (specularElem) {
		ColorRGBA color;
		color.x = specularElem->FloatAttribute("r");
		color.y = specularElem->FloatAttribute("g");
		color.z = specularElem->FloatAttribute("b");
		color.w = specularElem->FloatAttribute("a");
		specular = color;
	}
}

void DirectionalLight::SaveXML(XMLElement* compElem)
{
	compElem->SetAttribute("ComponentType", "DirectionalLight");

	XMLElement* ambientElem = compElem->InsertNewChildElement("Ambient");
	ambientElem->SetAttribute("r", ambient.x);
	ambientElem->SetAttribute("g", ambient.y);
	ambientElem->SetAttribute("b", ambient.z);
	ambientElem->SetAttribute("a", ambient.w);

	XMLElement* diffuseElem = compElem->InsertNewChildElement("Diffuse");
	diffuseElem->SetAttribute("r", diffuse.x);
	diffuseElem->SetAttribute("g", diffuse.y);
	diffuseElem->SetAttribute("b", diffuse.z);
	diffuseElem->SetAttribute("a", diffuse.w);

	XMLElement* specularElem = compElem->InsertNewChildElement("Specular");
	specularElem->SetAttribute("r", specular.x);
	specularElem->SetAttribute("g", specular.y);
	specularElem->SetAttribute("b", specular.z);
	specularElem->SetAttribute("a", specular.w);
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
