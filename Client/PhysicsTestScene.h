#pragma once
#include "IExecute.h"
class PhysicsTestScene : public IExecute
{
public:
	void Init() override;
	void Update() override;

private:
	vector<shared_ptr<GameObject>> gameObjects;
};
