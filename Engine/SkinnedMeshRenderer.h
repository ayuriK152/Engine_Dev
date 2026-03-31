#pragma once

class Skeleton;

class SkinnedMeshRenderer : public MeshRenderer
{ 
	using Super = MeshRenderer;

public:
	SkinnedMeshRenderer();
	virtual ~SkinnedMeshRenderer();

	void Init() override;
	void Render(ID3D12GraphicsCommandList* cmdList, UINT renderState) override;

	void OnDestroy() override;

	void LoadXML(XMLElement* compElem) override;
	void SaveXML(XMLElement* compElem) override;

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

