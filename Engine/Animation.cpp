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

Animation::KeyFrame* Animation::Interpolate(const string& boneName, float time)
{
	if (!_boneAnimations.contains(boneName))
		return nullptr;
	if (_boneAnimations[boneName].keyFrames.empty())
		return nullptr;
	if (_boneAnimations[boneName].keyFrames.size() == 1)
	{
		KeyFrame* resultFrame = new KeyFrame();
		resultFrame->time = _boneAnimations[boneName].keyFrames[0].time;
		resultFrame->position = _boneAnimations[boneName].keyFrames[0].position;
		resultFrame->rotation = _boneAnimations[boneName].keyFrames[0].rotation;
		resultFrame->scale = _boneAnimations[boneName].keyFrames[0].scale;

		return resultFrame;
	}

	AnimationData animationData = _boneAnimations[boneName];
	KeyFrame prevFrame, nextFrame;
	KeyFrame* resultFrame = new KeyFrame();
	for (int i = 0; i < animationData.keyFrames.size() - 1; i++)
	{
		if (time >= animationData.keyFrames[i].time && time <= animationData.keyFrames[i + 1].time)
		{
			prevFrame = animationData.keyFrames[i];
			nextFrame = animationData.keyFrames[i + 1];
			break;
		}
	}

	float blendValue = (time - prevFrame.time) / (nextFrame.time - prevFrame.time);
	resultFrame->time = time;

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

void Animation::AnimationTest(shared_ptr<Transform> transform)
{
	string boneName = transform->GetGameObject()->name;
	KeyFrame* keyFrame = Interpolate(boneName, 0.0f);

	if (keyFrame == nullptr)
		return;

	XMMATRIX matScale = XMMatrixScaling(keyFrame->scale.x, keyFrame->scale.y, keyFrame->scale.z);
	XMMATRIX matRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&keyFrame->rotation));
	XMMATRIX matTranslate = XMMatrixTranslation(keyFrame->position.x, keyFrame->position.y, keyFrame->position.z);

	XMFLOAT4X4 finalMat;
	XMStoreFloat4x4(&finalMat, matScale * matRotation * matTranslate);
	transform->SetLocalMatrix(finalMat);
}
