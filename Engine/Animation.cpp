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
	_boneAnimations[animation.boneName] = animation;
}

XMMATRIX Animation::GetAnimationMatTest(string boneName)
{
	KeyFrame testData = _boneAnimations[boneName].keyFrames[3];
	XMMATRIX matScale = XMMatrixScaling(testData.scale.x, testData.scale.y, testData.scale.z);
	XMMATRIX matRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&testData.rotation));
	XMMATRIX matTranslate = XMMatrixTranslation(testData.position.x, testData.position.y, testData.position.z);

	return matScale * matRotation * matTranslate;
}
