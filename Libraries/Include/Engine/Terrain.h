#pragma once
#include "Component.h"

#define DEFAULT_TERRAIN_COUNT	10

class BULB_API Terrain : public Component
{
public:
	Terrain();
	virtual ~Terrain();

	void Init() override;
	void PreUpdate() override;
	void Update() override;
	void Render(ID3D12GraphicsCommandList* cmdList, UINT renderState) override;

	void OnDestroy() override;

	void BuildBuffer();

	static void ReleaseBuffer();

	void SetTestHeightSample();

	void LoadXML(Bulb::XMLElement compElem) override;

	void SaveXML(Bulb::XMLElement compElem) override;

	ComponentSnapshot CaptureSnapshot() override;

	void RestoreSnapshot(ComponentSnapshot snapshot) override;

	void SetHeightMap(shared_ptr<Texture> texture);

	void UpdateConstant();

private:
	static unique_ptr<UploadBuffer<InstanceConstants>> _terrainInstanceSB;
	static int _terrainCount;
	int _terrainId;
	int _instanceSrvIdx;
	string _heightMapTexturePath;
	int _heightMapTextureIndex;

	int _sampleCount;
	vector<float> _heightSamples;
	float _heightFactor = 1.0f;

	shared_ptr<Mesh> _terrainMesh;

	JPH::BodyID _bodyID;
	JPH::ShapeSettings::ShapeResult _shapeResult;
};

