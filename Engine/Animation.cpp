#include "pch.h"
#include "Animation.h"

Animation::Animation() : Resource(ResourceType::Animation)
{

}

Animation::Animation(string name, float duration, float ticksPerSecond) : Resource(ResourceType::Animation)
{
	SetName(name);
	_duration = duration;
	_ticksPerSecond = ticksPerSecond;
}

void Animation::AddAnimationData(const AnimationData animation)
{
	_animationDatas[animation.boneName] = animation;
}

Animation::KeyFrame Animation::Interpolate(const string& boneName, float tick, int& lastIdx)
{
	KeyFrame resultFrame;

	if (!_animationDatas.contains(boneName))
		return resultFrame;
	if (_animationDatas[boneName].keyFrames.empty())
		return resultFrame;

	if (_animationDatas[boneName].keyFrames.size() == 1)
	{
		return _animationDatas[boneName].keyFrames[0];
	}

	// 이전 키프레임 인덱스를 그대로 사용 가능하면 사용.
	AnimationData animationData = _animationDatas[boneName];
	KeyFrame prevFrame, nextFrame;
	bool foundFlag = false;

	if (lastIdx >= 0 && lastIdx <= animationData.keyFrames.size() - 2)
	{
		if (tick >= animationData.keyFrames[lastIdx].tick && tick <= animationData.keyFrames[lastIdx + 1].tick)
		{
			prevFrame = animationData.keyFrames[lastIdx];
			nextFrame = animationData.keyFrames[lastIdx + 1];
			foundFlag = true;
		}
	}

	// 이전 키프레임 썼으면 통과
	if (!foundFlag)
	{
		// 이진 탐색 알고리즘 채택
		int left = 0, right = animationData.keyFrames.size() - 2;
		int mid;
		while (left <= right)
		{
			mid = left + (right - left) / 2;
			if (tick >= animationData.keyFrames[mid].tick && tick <= animationData.keyFrames[mid + 1].tick)
			{
				prevFrame = animationData.keyFrames[mid];
				nextFrame = animationData.keyFrames[mid + 1];
				foundFlag = true;
				break;
			}
			else if (tick < animationData.keyFrames[mid].tick)
			{
				right = mid - 1;
			}
			else if (tick > animationData.keyFrames[mid + 1].tick)
			{
				left = mid + 1;
			}
		}

		lastIdx = foundFlag ? mid : animationData.keyFrames.size() - 1;
	}

	// 탐색 후에도 못찾았으면 마지막 키프레임 설정
	if (!foundFlag)
	{
		prevFrame = animationData.keyFrames[lastIdx];
		nextFrame = animationData.keyFrames[lastIdx];
	}

	float blendValue = foundFlag ? (tick - prevFrame.tick) / (nextFrame.tick - prevFrame.tick) : 0.0f;
	resultFrame.tick = tick;

	resultFrame.position = Vector3(
		prevFrame.position.x + blendValue * (nextFrame.position.x - prevFrame.position.x),
		prevFrame.position.y + blendValue * (nextFrame.position.y - prevFrame.position.y),
		prevFrame.position.z + blendValue * (nextFrame.position.z - prevFrame.position.z)
	);
	XMStoreFloat4(&resultFrame.rotation, XMQuaternionSlerp(XMLoadFloat4(&prevFrame.rotation), XMLoadFloat4(&nextFrame.rotation), blendValue));
	resultFrame.scale = Vector3(
		prevFrame.scale.x + blendValue * (nextFrame.scale.x - prevFrame.scale.x),
		prevFrame.scale.y + blendValue * (nextFrame.scale.y - prevFrame.scale.y),
		prevFrame.scale.z + blendValue * (nextFrame.scale.z - prevFrame.scale.z)
	);

	return resultFrame;
}
