#pragma once
#include "IExecute.h"
class TestScene : public IExecute
{
public:
	void Init() override;

	void Update() override;

private:
	shared_ptr<GameObject> skybox;

	shared_ptr<GameObject> camera;
	shared_ptr<GameObject> globalLight;
	shared_ptr<GameObject> sphere;
	shared_ptr<GameObject> sphere2;
	shared_ptr<GameObject> ground;
	shared_ptr<GameObject> box;
	shared_ptr<GameObject> box2;
	shared_ptr<GameObject> model;

	vector<shared_ptr<GameObject>> gameObjects;
};

