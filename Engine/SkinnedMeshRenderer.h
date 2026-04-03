#pragma once

class Skeleton;

class BULB_API SkinnedMeshRenderer : public MeshRenderer
{ 
	using Super = MeshRenderer;

public:
	SkinnedMeshRenderer();
	virtual ~SkinnedMeshRenderer();

	void Init() override;
	void Render(ID3D12GraphicsCommandList* cmdList, UINT renderState) override;

	void OnDestroy() override;

	void LoadXML(Bulb::XMLElement compElem) override;
	void SaveXML(Bulb::XMLElement compElem) override;

	ComponentSnapshot CaptureSnapshot() override;
	void RestoreSnapshot(ComponentSnapshot snapshot) override;

public:
	void SetRootBone(const shared_ptr<Transform> rootBone);
	shared_ptr<Transform> GetRootBone() const { return _rootBone; }

private:
	string _rootBoneName;
	shared_ptr<Transform> _rootBone;
	shared_ptr<Skeleton> _skeleton;
};

