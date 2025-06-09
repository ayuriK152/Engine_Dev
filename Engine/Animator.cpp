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
	_currentTick += TIME->DeltaTime() * ticksPerSecond;

	if (_currentTick > _currentAnimation->GetDuration()) {
		if (_isLoop)
			_currentTick = 0.0f;
		else
			_currentTick = _currentAnimation->GetDuration();
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
	vector<shared_ptr<Transform>> childs = GetTransform()->GetChilds();
	for (int i = 0; i < childs.size(); i++)
	{
		shared_ptr<Transform> child = childs[i];
		childs.insert(childs.end(), child->GetChilds().begin(), child->GetChilds().end());
		string objName = child->GetGameObject()->name;
		Animation::KeyFrame* keyFrame = _currentAnimation->Interpolate(objName, _currentTick);

		if (keyFrame == nullptr)
			continue;

		XMMATRIX matScale = XMMatrixScaling(keyFrame->scale.x, keyFrame->scale.y, keyFrame->scale.z);
		XMMATRIX matRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&keyFrame->rotation));
		XMMATRIX matTranslate = XMMatrixTranslation(keyFrame->position.x, keyFrame->position.y, keyFrame->position.z);

		XMFLOAT4X4 finalMat;
		XMStoreFloat4x4(&finalMat, matScale * matRotation * matTranslate);
		child->SetLocalMatrix(finalMat);
	}
}
