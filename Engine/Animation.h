#pragma once
#include "Resource.h"

class Animation : public Resource
{
public:
	struct KeyFrame
	{
		double time;
		Vector4 position;
		Vector4 rotation;
		Vector4 scale;
	};

	struct AnimationData
	{
		string boneName;
		vector<KeyFrame> keyFrames;
	};

public:
	Animation();
	Animation(string name, float duration, float ticksPerSecond);
	~Animation() = default;

public:
	void SetName(const string& name) { _name = name; }
	string GetName() const { return _name; }

	float GetDuration() const { return _duration; }

	float GetTicksPerSecond() const { return _ticksPerSecond; }

	void AddAnimationData(const AnimationData animation);

	XMMATRIX GetAnimationMatTest(string boneName);

private:
	string _name;
	float _duration;
	float _ticksPerSecond;
	unordered_map<string, AnimationData> _boneAnimations;
};

