#include "pch.h"
#include "PointLight.h"

PointLight::PointLight() : Super()
{

}

PointLight::PointLight(XMFLOAT4 diffuse) : Super(diffuse)
{

}

PointLight::~PointLight()
{
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "Released - PointLight:" << _id << "\n";
#endif

	_transform.reset();
}

void PointLight::Init()
{
	_lightType = LightType::Point;

	_matProj = MathHelper::Identity4x4();

	_transform = GetTransform();

	RENDER->AddLight(static_pointer_cast<Light>(shared_from_this()));
}

void PointLight::Update()
{
	if (_gameObject.lock()->GetFramesDirty() > 0) {
		_position = _transform->GetPosition();
	}
}

void PointLight::OnDestroy()
{
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "OnDestroy - DirectionalLight:" << _id << "\n";
#endif

	RENDER->DeleteLight(static_pointer_cast<Light>(shared_from_this()));

	_transform.reset();
}

void PointLight::LoadXML(Bulb::XMLElement compElem)
{
	intensity = compElem.FloatAttribute("Intensity", 1.0f);

	Bulb::XMLElement diffuseElem = compElem.FirstChildElement("Diffuse");
	if (!diffuseElem.IsNullPtr()) {
		Bulb::Color color;
		color.r = diffuseElem.FloatAttribute("r");
		color.g = diffuseElem.FloatAttribute("g");
		color.b = diffuseElem.FloatAttribute("b");
		color.a = diffuseElem.FloatAttribute("a");
		diffuse = color;
	}
}

void PointLight::SaveXML(Bulb::XMLElement compElem)
{
	compElem.SetAttribute("ComponentType", "PointLight");

	compElem.SetAttribute("Intensity", intensity);

	Bulb::XMLElement diffuseElem = compElem.InsertNewChildElement("Diffuse");
	diffuseElem.SetAttribute("r", diffuse.r);
	diffuseElem.SetAttribute("g", diffuse.g);
	diffuseElem.SetAttribute("b", diffuse.b);
	diffuseElem.SetAttribute("a", diffuse.a);
}

ComponentSnapshot PointLight::CaptureSnapshot()
{
	ComponentSnapshot snapshot;

	snapshot.id = _id;
	snapshot.componentType = "PointLight";

	return snapshot;
}

void PointLight::RestoreSnapshot(ComponentSnapshot snapshot)
{

}

LightConstants PointLight::GetLightConstants()
{
	LightConstants constants;
	constants.LightType = POINT_LIGHT;
	constants.Position = _position;
	constants.FallOffStart = _fallOffStart;
	constants.FallOffEnd = _fallOffEnd;
	constants.Diffuse = diffuse;
	constants.intensity = intensity;

	return constants;
}
