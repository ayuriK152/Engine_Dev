#include "pch.h"
#include "Terrain.h"

Terrain::Terrain() : Component(ComponentType::Terrain)
{

}

Terrain::~Terrain()
{

}

void Terrain::Init()
{
	SetTestHeightSample();

	// 설정 객체 생성
	JPH::HeightFieldShapeSettings settings;
	settings.mOffset = Vec3(0, 0, 0); // 지형의 시작점 오프셋
	settings.mScale = Vec3(1.0f, 1.0f, 1.0f); // 각 샘플 사이의 거리 및 높이 배율
	settings.mSampleCount = _sampleCount;
	settings.mHeightSamples.resize(_sampleCount * _sampleCount);
	for (int y = 0; y < _sampleCount; ++y) {
		for (int x = 0; x < _sampleCount; ++x) {
			settings.mHeightSamples[y * _sampleCount + x] = _heightSamples[y * _sampleCount + x];
		}
	}

	_shapeResult = settings.Create();

	JPH::BodyCreationSettings bodySettings(_shapeResult.Get(),
		JPH::RVec3(0, 0, 0),
		JPH::Quat(0, 0, 0, 1),
		EMotionType::Static,
		Layers::NON_MOVING);

	bodySettings.mUserData = reinterpret_cast<JPH::uint64>(_gameObject.lock().get());

	JPH::BodyInterface& bodyInterface = PHYSICS->GetPhysicsSystem()->GetBodyInterface();
	_bodyID = bodyInterface.CreateAndAddBody(bodySettings, JPH::EActivation::Activate);
}

void Terrain::Update()
{

}

void Terrain::OnDestroy()
{

}

void Terrain::SetTestHeightSample()
{
	_sampleCount = 10;
	_heightSamples.clear();
	_heightSamples.resize(_sampleCount * _sampleCount);
	_heightSamples = vector<float>(_sampleCount * _sampleCount);
	_heightSamples = {
		0.12f, 0.15f, 0.22f, 0.35f, 0.42f, 0.38f, 0.25f, 0.18f, 0.12f, 0.10f,
		0.18f, 0.25f, 0.38f, 0.52f, 0.58f, 0.55f, 0.42f, 0.28f, 0.20f, 0.15f,
		0.25f, 0.42f, 0.55f, 0.72f, 0.85f, 0.78f, 0.62f, 0.45f, 0.32f, 0.22f,
		0.32f, 0.58f, 0.75f, 0.88f, 0.95f, 0.92f, 0.78f, 0.55f, 0.38f, 0.28f,
		0.35f, 0.62f, 0.82f, 0.95f, 1.00f, 0.98f, 0.85f, 0.62f, 0.42f, 0.32f,
		0.28f, 0.55f, 0.78f, 0.92f, 0.98f, 0.95f, 0.82f, 0.58f, 0.38f, 0.25f,
		0.22f, 0.45f, 0.65f, 0.78f, 0.85f, 0.75f, 0.62f, 0.42f, 0.28f, 0.18f,
		0.15f, 0.28f, 0.42f, 0.55f, 0.62f, 0.58f, 0.45f, 0.32f, 0.22f, 0.12f,
		0.10f, 0.18f, 0.28f, 0.38f, 0.45f, 0.42f, 0.35f, 0.25f, 0.15f, 0.08f,
		0.05f, 0.12f, 0.18f, 0.25f, 0.32f, 0.28f, 0.22f, 0.15f, 0.10f, 0.05f };
}

void Terrain::LoadXML(Bulb::XMLElement compElem)
{

}

void Terrain::SaveXML(Bulb::XMLElement compElem)
{

}

ComponentSnapshot Terrain::CaptureSnapshot()
{
	ComponentSnapshot snapshot;

	snapshot.id = _id;
	snapshot.componentType = "Terrain";

	return snapshot;
}

void Terrain::RestoreSnapshot(ComponentSnapshot snapshot)
{

}
