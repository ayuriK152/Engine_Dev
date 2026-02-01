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
	if (animation.boneName == "") return;

	// 추가하려는 데이터보다 배열이 작은 경우 추가 공간 할당
	int animationDataAssignSize = animation.boneId - _animationDatas.size() + 1;
	for (; animationDataAssignSize > 0; animationDataAssignSize--) {
		_animationDatas.push_back(AnimationData());
	}
	_animationDatas[animation.boneId] = animation;
}

Animation::KeyFrame Animation::Interpolate(int boneIdx, float tick, int& lastIdx)
{
	KeyFrame resultFrame;

	if (boneIdx >= _animationDatas.size())
		return resultFrame;

	if (_animationDatas[boneIdx].keyFrames.empty())
		return resultFrame;

	if (_animationDatas[boneIdx].keyFrames.size() == 1)
		return _animationDatas[boneIdx].keyFrames[0];

	// 이전 키프레임 인덱스를 그대로 사용 가능하면 사용.
	const AnimationData& animationData = _animationDatas[boneIdx];
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
	resultFrame.position = prevFrame.position + (nextFrame.position - prevFrame.position) * blendValue;
	XMStoreFloat4(&resultFrame.rotation, XMQuaternionSlerp(XMLoadFloat4(&prevFrame.rotation), XMLoadFloat4(&nextFrame.rotation), blendValue));
	resultFrame.scale = prevFrame.scale + (nextFrame.scale - prevFrame.scale) * blendValue;

	return resultFrame;
}
