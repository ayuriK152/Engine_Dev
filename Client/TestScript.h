#pragma once
#include "IExecute.h"
class TestScript : public IExecute
{
public:
	void Init() override;

	void Update() override;

private:
	shared_ptr<GameObject> camera;
};

