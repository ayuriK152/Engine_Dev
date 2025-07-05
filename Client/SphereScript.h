#pragma once

class SphereScript : public Script
{
public:
	void Init() override;
	void Update() override;

private:
	shared_ptr<GameObject> gameObject;
	shared_ptr<Transform> transform;
};

