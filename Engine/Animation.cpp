#include "pch.h"
#include "Animation.h"

Animation::Animation() : Resource(ResourceType::Animation)
{

}

Animation::Animation(string name, float duration, float ticksPerSecond) : Resource(ResourceType::Animation)
{
	_name = name;
	_duration = duration;
	_ticksPerSecond = ticksPerSecond;
}

void Animation::AddAnimationData(const AnimationData animation)
{
	_animationDatas[animation.boneName] = animation;
}

Animation::KeyFrame* Animation::Interpolate(const string& boneName, float tick)
{
	if (!_animationDatas.contains(boneName))
		return nullptr;
	if (_animationDatas[boneName].keyFrames.empty())
		return nullptr;
	if (_animationDatas[boneName].keyFrames.size() == 1)
	{
		KeyFrame* resultFrame = new KeyFrame();
		resultFrame->tick = _animationDatas[boneName].keyFrames[0].tick;
		resultFrame->position = _animationDatas[boneName].keyFrames[0].position;
		resultFrame->rotation = _animationDatas[boneName].keyFrames[0].rotation;
		resultFrame->scale = _animationDatas[boneName].keyFrames[0].scale;

		return resultFrame;
	}

	AnimationData animationData = _animationDatas[boneName];
	KeyFrame prevFrame, nextFrame;
	KeyFrame* resultFrame = new KeyFrame();
	for (int i = 0; i < animationData.keyFrames.size() - 1; i++)
	{
		if (tick >= animationData.keyFrames[i].tick && tick <= animationData.keyFrames[i + 1].tick)
		{
			prevFrame = animationData.keyFrames[i];
			nextFrame = animationData.keyFrames[i + 1];
			break;
		}
	}

	float blendValue = (tick - prevFrame.tick) / (nextFrame.tick - prevFrame.tick);
	resultFrame->tick = tick;

	resultFrame->position = Vector3(
		prevFrame.position.x + blendValue * (nextFrame.position.x - prevFrame.position.x),
		prevFrame.position.y + blendValue * (nextFrame.position.y - prevFrame.position.y),
		prevFrame.position.z + blendValue * (nextFrame.position.z - prevFrame.position.z)
	);
	XMStoreFloat4(&resultFrame->rotation, XMQuaternionSlerp(XMLoadFloat4(&prevFrame.rotation), XMLoadFloat4(&nextFrame.rotation), blendValue));
	resultFrame->scale = Vector3(
		prevFrame.scale.x + blendValue * (nextFrame.scale.x - prevFrame.scale.x),
		prevFrame.scale.y + blendValue * (nextFrame.scale.y - prevFrame.scale.y),
		prevFrame.scale.z + blendValue * (nextFrame.scale.z - prevFrame.scale.z)
	);

	return resultFrame;
}
