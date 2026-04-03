#include "pch.h"
#include "DirectionalLight.h"

// REGISTER_COMPONENT(DirectionalLight);

DirectionalLight::DirectionalLight() : Super()
{

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
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "Released - DirectionalLight:" << _id << "\n";
#endif
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
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "OnDestroy - DirectionalLight:" << _id << "\n";
#endif

	RENDER->DeleteLight(static_pointer_cast<Light>(shared_from_this()));

	_transform.reset();
}

void DirectionalLight::LoadXML(Bulb::XMLElement compElem)
{
	Bulb::XMLElement ambientElem = compElem.FirstChildElement("Ambient");
	if (!ambientElem.IsNullPtr()) {
		Bulb::Color color;
		color.r = ambientElem.FloatAttribute("r");
		color.g = ambientElem.FloatAttribute("g");
		color.b = ambientElem.FloatAttribute("b");
		color.a = ambientElem.FloatAttribute("a");
		ambient = color;
	}

	Bulb::XMLElement diffuseElem = compElem.FirstChildElement("Diffuse");
	if (!diffuseElem.IsNullPtr()) {
		Bulb::Color color;
		color.r = diffuseElem.FloatAttribute("r");
		color.g = diffuseElem.FloatAttribute("g");
		color.b = diffuseElem.FloatAttribute("b");
		color.a = diffuseElem.FloatAttribute("a");
		diffuse = color;
	}

	Bulb::XMLElement specularElem = compElem.FirstChildElement("Specular");
	if (!specularElem.IsNullPtr()) {
		Bulb::Color color;
		color.r = specularElem.FloatAttribute("r");
		color.g = specularElem.FloatAttribute("g");
		color.b = specularElem.FloatAttribute("b");
		color.a = specularElem.FloatAttribute("a");
		specular = color;
	}
}

void DirectionalLight::SaveXML(Bulb::XMLElement compElem)
{
	compElem.SetAttribute("ComponentType", "DirectionalLight");

	Bulb::XMLElement ambientElem = compElem.InsertNewChildElement("Ambient");
	ambientElem.SetAttribute("r", ambient.r);
	ambientElem.SetAttribute("g", ambient.g);
	ambientElem.SetAttribute("b", ambient.b);
	ambientElem.SetAttribute("a", ambient.a);

	Bulb::XMLElement diffuseElem = compElem.InsertNewChildElement("Diffuse");
	diffuseElem.SetAttribute("r", diffuse.r);
	diffuseElem.SetAttribute("g", diffuse.g);
	diffuseElem.SetAttribute("b", diffuse.b);
	diffuseElem.SetAttribute("a", diffuse.a);

	Bulb::XMLElement specularElem = compElem.InsertNewChildElement("Specular");
	specularElem.SetAttribute("r", specular.r);
	specularElem.SetAttribute("g", specular.g);
	specularElem.SetAttribute("b", specular.b);
	specularElem.SetAttribute("a", specular.a);
}

ComponentSnapshot DirectionalLight::CaptureSnapshot()
{
	ComponentSnapshot snapshot;

	snapshot.id = _id;
	snapshot.componentType = "DirectionalLight";

	return snapshot;
}

void DirectionalLight::RestoreSnapshot(ComponentSnapshot snapshot)
{

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
