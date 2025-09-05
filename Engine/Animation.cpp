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

	// ���� Ű������ �ε����� �״�� ��� �����ϸ� ���.
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

	// ���� Ű������ ������ ���
	if (!foundFlag)
	{
		// ���� Ž�� �˰��� ä��
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

	// Ž�� �Ŀ��� ��ã������ ������ Ű������ ����
	if (!foundFlag)
	{
		prevFrame = animationData.keyFrames[lastIdx];
		nextFrame = animationData.keyFrames[lastIdx];
	}

	float blendValue = foundFlag ? (tick - prevFrame.tick) / (nextFrame.tick - prevFrame.tick) : 0.0f;
	resultFrame.tick = tick;
	resultFrame.position = prevFrame.position + (nextFrame.position - prevFrame.position) * blendValue;
	XMStoreFloat4(&resultFrame.rotation, XMQuaternionSlerp(XMLoadFloat4(&prevFrame.rotation), XMLoadFloat4(&nextFrame.rotation), blendValue));
	resultFrame.scale = prevFrame.scale + (nextFrame.scale - prevFrame.scale) * blendValue;

	return resultFrame;
}
