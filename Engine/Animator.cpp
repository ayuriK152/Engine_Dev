#include "pch.h"
#include "Animator.h"

Animator::Animator() : Component(ComponentType::Animator)
{
	_isPlaying = true;
	_isLoop = true;

	_currentAnimation = EMPTY_CURRENT_ANIMATION;
}

Animator::~Animator()
{

}

void Animator::Init()
{
	_isPlaying = _isPlayOnInit;
}

void Animator::Update()
{
	if (_currentAnimation == EMPTY_CURRENT_ANIMATION || !_isPlaying)
		return;

	UpdateBoneTransform();

	float ticksPerSecond = (GetCurrentAnimation()->GetTicksPerSecond() != 0.0f) ? GetCurrentAnimation()->GetTicksPerSecond() : 25.0f;
	_currentTick += TIME->DeltaTime() * ticksPerSecond;

	if (_currentTick > GetCurrentAnimation()->GetDuration()) {
		if (_isLoop)
			_currentTick = 0.0f;
		else
			_currentTick = GetCurrentAnimation()->GetDuration();
	}
}

void Animator::PlayAnimation()
{
	if (_currentAnimation == EMPTY_CURRENT_ANIMATION)
	{
		cout << "Current Animation is null!" << endl;
		return;
	}

	_isPlaying = true;
}

void Animator::PauseAnimation()
{
	if (_currentAnimation == EMPTY_CURRENT_ANIMATION)
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
		Animation::KeyFrame* keyFrame = GetCurrentAnimation()->Interpolate(objName, _currentTick);

		if (keyFrame == nullptr)
			continue;

		XMMATRIX matScale = XMMatrixScaling(keyFrame->scale.x, keyFrame->scale.y, keyFrame->scale.z);
		XMMATRIX matRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&keyFrame->rotation));
		XMMATRIX matTranslate = XMMatrixTranslation(keyFrame->position.x, keyFrame->position.y, keyFrame->position.z);

		XMFLOAT4X4 finalMat;
		XMStoreFloat4x4(&finalMat, matScale * matRotation * matTranslate);
		child->SetLocalMatrix(finalMat);

		delete keyFrame;
	}
}
