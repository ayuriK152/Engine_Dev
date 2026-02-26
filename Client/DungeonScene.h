#pragma once
#include "IExecute.h"

class DungeonScene : public IExecute
{
public:
	~DungeonScene();

	void Init() override;
	void Update() override;

	shared_ptr<GameObject> camera;
	shared_ptr<GameObject> tpvCamera;
	shared_ptr<GameObject> tpvCameraArm;

	shared_ptr<GameObject> player;

	vector<shared_ptr<GameObject>> gameObjects;
};

