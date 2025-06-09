#include "pch.h"
#include "Animator.h"

Animator::Animator() : Component(ComponentType::Animator)
{
	_isPlaying = true;
	_isLoop = true;
}

Animator::~Animator()
{

}

void Animator::Update()
{
	if (_currentAnimation == nullptr || !_isPlaying)
		return;

	UpdateBoneTransform();

	float ticksPerSecond = (_currentAnimation->GetTicksPerSecond() != 0.0f) ? _currentAnimation->GetTicksPerSecond() : 25.0f;
	_currentTime += TIME->DeltaTime() * ticksPerSecond;

	if (_currentTime > _currentAnimation->GetDuration()) {
		if (_isLoop)
			_currentTime = 0.0f;
		else
			_currentTime = _currentAnimation->GetDuration();
	}
}

void Animator::PlayAnimation()
{
	if (_currentAnimation == nullptr)
	{
		cout << "Current Animation is null!" << endl;
		return;
	}

	_isPlaying = true;
}

void Animator::PauseAnimation()
{
	if (_currentAnimation == nullptr)
	{
		cout << "Current Animation is null!" << endl;
		return;
	}

	_isPlaying = false;
}

void Animator::UpdateBoneTransform()
{
	shared_ptr<SkinnedMeshRenderer> renderer = static_pointer_cast<SkinnedMeshRenderer>(GetGameObject()->GetComponent<MeshRenderer>());
	if (renderer == nullptr)
	{
		cout << "No Available Renderer!" << endl;
		return;
	}

	vector<shared_ptr<Transform>> boneTransforms = renderer->GetBoneTransforms();
	for (const auto& boneTransform : boneTransforms)
	{
		string boneName = boneTransform->GetGameObject()->name;
		Animation::KeyFrame* keyFrame = _currentAnimation->Interpolate(boneName, _currentTime);

		if (keyFrame == nullptr)
			continue;

		XMMATRIX matScale = XMMatrixScaling(keyFrame->scale.x, keyFrame->scale.y, keyFrame->scale.z);
		XMMATRIX matRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&keyFrame->rotation));
		XMMATRIX matTranslate = XMMatrixTranslation(keyFrame->position.x, keyFrame->position.y, keyFrame->position.z);

		XMFLOAT4X4 finalMat;
		XMStoreFloat4x4(&finalMat, matScale * matRotation * matTranslate);
		boneTransform->SetLocalMatrix(finalMat);
	}
}
