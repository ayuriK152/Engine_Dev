#pragma once
#include "Component.h"

class BULB_API Terrain : public Component
{
public:
	Terrain();
	virtual ~Terrain();

	void Init() override;
	void Update() override;

	void OnDestroy() override;

	void SetTestHeightSample();

	void LoadXML(Bulb::XMLElement compElem) override;

	void SaveXML(Bulb::XMLElement compElem) override;

	ComponentSnapshot CaptureSnapshot() override;

	void RestoreSnapshot(ComponentSnapshot snapshot) override;

	void SetHeightMap(shared_ptr<Texture> texture);

private:
	int _sampleCount;
	vector<float> _heightSamples;
	string _heightMapTexturePath;

	JPH::BodyID _bodyID;
	JPH::ShapeSettings::ShapeResult _shapeResult;
};

