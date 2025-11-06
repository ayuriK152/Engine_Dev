#include "pch.h"
#include "Animator.h"

Animator::Animator() : Component(ComponentType::Animator)
{
	_isPlayOnInit = true;
	_isPlaying = true;
	_isLoop = true;
	_isCurrentAnimationEnd = false;

	_currentAnimation = EMPTY_ANIMATION;
	_nextAnimation = EMPTY_ANIMATION;
}

Animator::~Animator()
{

}

void Animator::Init()
{
	_isPlaying = _isPlayOnInit;
	UpdateChildList();
}

void Animator::Update()
{
	if (_isPreviewMode)
	{
		if (_previewAnimation == nullptr || !_isPreviewPlaying)
			return;

		UpdateBoneTransform();

		float ticksPerSecond = (GetPreviewAnimation()->GetTicksPerSecond() != 0.0f) ? GetPreviewAnimation()->GetTicksPerSecond() : 25.0f;
		_previewTick += TIME->DeltaTime() * ticksPerSecond;

		if (_previewTick > GetPreviewAnimation()->GetDuration()) {
			_previewTick = 0.0f;
		}
	}

	else
	{
		if (_currentAnimation == EMPTY_ANIMATION || !_isPlaying)
			return;

		UpdateBoneTransform();
		UpdateAnimationEvent();

		if (_isInTransition) {
			_transitionElapsedTime += TIME->DeltaTime();
			if (_transitionElapsedTime >= _transitionTime) {
				_isInTransition = false;
				_currentAnimation = _nextAnimation;
				_nextAnimation = EMPTY_ANIMATION;
				_currentTick = _transitionTick;
				_transitionTick = 0.0f;
				_transitionElapsedTime = 0.0f;
				_currentAnimationSpeed = _nextAnimationSpeed;

				_nextAnimationEventIndex = 0;
			}
			else {
				float ticksPerSecond = _animations[_nextAnimation]->GetTicksPerSecond();
				_transitionTick += TIME->DeltaTime() * ticksPerSecond * _nextAnimationSpeed;
			}
		}

		float ticksPerSecond = (GetCurrentAnimation()->GetTicksPerSecond() != 0.0f) ? GetCurrentAnimation()->GetTicksPerSecond() : 25.0f;
		_currentTick += TIME->DeltaTime() * ticksPerSecond * _currentAnimationSpeed;

		if (_currentTick > GetCurrentAnimation()->GetDuration()) {
			if (_isLoop)
			{
				_currentTick = 0.0f;
				_currentAnimationEventIndex = 0;
			}
			else
			{
				_isCurrentAnimationEnd = true;
				_currentTick = GetCurrentAnimation()->GetDuration();
			}
		}
		else
		{
			_isCurrentAnimationEnd = false;
		}
	}
}

void Animator::PlayAnimation()
{
	if (_currentAnimation == EMPTY_ANIMATION)
	{
		cout << "Current Animation is null!" << endl;
		return;
	}

	_isPlaying = true;
	_isCurrentAnimationEnd = false;
}

void Animator::PauseAnimation()
{
	if (_currentAnimation == EMPTY_ANIMATION)
	{
		cout << "Current Animation is null!" << endl;
		return;
	}

	_isPlaying = false;
}

void Animator::UpdateChildList()
{
	_childs = GetTransform()->GetChilds();
	for (int i = 0; i < _childs.size(); i++)
	{
		_childs.insert(_childs.end(), _childs[i]->GetChilds().begin(), _childs[i]->GetChilds().end());
	}
	_lastKeyframeIndex.clear();
	_lastKeyframeIndex.resize(_childs.size(), 0);
}

void Animator::UpdateBoneTransform()
{
	if (_isPreviewMode)
	{
		for (int i = 0; i < _childs.size(); i++)
		{
			string objName = _childs[i]->GetGameObject()->GetName();
			Animation::KeyFrame currKeyFrame = GetPreviewAnimation()->Interpolate(objName, _previewTick, _lastKeyframeIndex[i]);

			if (currKeyFrame.tick < 0.0)
				continue;

			XMMATRIX matScale = XMMatrixScaling(currKeyFrame.scale.x, currKeyFrame.scale.y, currKeyFrame.scale.z);
			XMMATRIX matRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&currKeyFrame.rotation));
			XMMATRIX matTranslation = XMMatrixTranslation(currKeyFrame.position.x, currKeyFrame.position.y, currKeyFrame.position.z);

			_childs[i]->SetLocalMatrix(matScale * matRotation * matTranslation);
		}
	}

	else
	{
		for (int i = 0; i < _childs.size(); i++)
		{
			string objName = _childs[i]->GetGameObject()->GetName();
			Animation::KeyFrame currKeyFrame = GetCurrentAnimation()->Interpolate(objName, _currentTick, _lastKeyframeIndex[i]);

			if (currKeyFrame.tick < 0.0)
				continue;

			if (_isInTransition && _animations.contains(_nextAnimation))
			{
				// 다음 애니메이션 키프레임
				int dummyIdx = 0;
				Animation::KeyFrame nextFrame = _animations[_nextAnimation]->Interpolate(objName, _transitionTick, dummyIdx);

				// 가중치 계산
				float alpha = _transitionElapsedTime / _transitionTime;
				alpha = std::clamp(alpha, 0.0f, 1.0f);

				// Lerp / Slerp
				Vector3 pos = currKeyFrame.position.Lerp(nextFrame.position, alpha);
				Vector3 scale = currKeyFrame.scale.Lerp(nextFrame.scale, alpha);
				Vector4 rot;
				XMStoreFloat4(&rot, XMQuaternionSlerp(XMLoadFloat4(&currKeyFrame.rotation), XMLoadFloat4(&nextFrame.rotation), alpha));

				XMMATRIX matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
				XMMATRIX matRotation = XMMatrixRotationQuaternion(XMQuaternionSlerp(XMLoadFloat4(&currKeyFrame.rotation), XMLoadFloat4(&nextFrame.rotation), alpha));
				XMMATRIX matTranslation = XMMatrixTranslation(pos.x, pos.y, pos.z);

				_childs[i]->SetLocalMatrix(matScale * matRotation * matTranslation);
			}
			else
			{
				XMMATRIX matScale = XMMatrixScaling(currKeyFrame.scale.x, currKeyFrame.scale.y, currKeyFrame.scale.z);
				XMMATRIX matRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&currKeyFrame.rotation));
				XMMATRIX matTranslation = XMMatrixTranslation(currKeyFrame.position.x, currKeyFrame.position.y, currKeyFrame.position.z);

				_childs[i]->SetLocalMatrix(matScale * matRotation * matTranslation);
			}
		}
	}
}

void Animator::SetCurrentAnimation(const string& animationName, float _transitionTime)
{
	if (_currentAnimation == EMPTY_ANIMATION || _transitionTime == 0.0f)
	{
		_currentAnimation = animationName;
		_currentTick = 0.0f;
		return;
	}
	_nextAnimation = animationName;
	_transitionElapsedTime = 0.0f;
	_transitionTick = 0.0f;
	_isInTransition = true;
}

void Animator::AddAnimation(shared_ptr<Animation> animation)
{
	_animations[animation->GetName()] = animation;
	if (_currentAnimation == EMPTY_ANIMATION) {
		_currentAnimation = animation->GetName();
	}
}

void Animator::UpdateAnimationEvent()
{
	// Current Animation Event
	if (_animationEvents.contains(_currentAnimation))
	{
		if (_currentAnimationEventIndex < _animationEvents[_currentAnimation].size()) {
			AnimationEvent currentEvent = _animationEvents[_currentAnimation][_currentAnimationEventIndex];
			if (currentEvent.Tick <= _currentTick) {
				if (currentEvent.type == AnimationEventTypes::Speed) {
					_currentAnimationSpeed = currentEvent.datas[0].x;
				}
				if (_animationEvents[_currentAnimation][_currentAnimationEventIndex].type == AnimationEventTypes::Attack) {
					Vector3 offset(currentEvent.datas[0].x, currentEvent.datas[0].y, currentEvent.datas[0].z);
					Vector3 scale(currentEvent.datas[1].x, currentEvent.datas[1].y, currentEvent.datas[1].z);
					Attack(offset, scale, currentEvent.datas[0].w, currentEvent.datas[1].w == 1 ? true : false);
				}

				_currentAnimationEventIndex++;
			}
		}
	}
	else
	{
		_currentAnimationSpeed = 1.0f;
		_currentAnimationEventIndex = 0;
	}

	// Next Animation Event
	if (_animationEvents.contains(_nextAnimation))
	{
		if (_nextAnimationEventIndex < _animationEvents[_nextAnimation].size()) {
			AnimationEvent nextEvent = _animationEvents[_nextAnimation][_nextAnimationEventIndex];
			if (nextEvent.Tick <= _transitionTick) {
				if (nextEvent.type == AnimationEventTypes::Speed) {
					_nextAnimationSpeed = nextEvent.datas[0].x;
				}
				if (nextEvent.type == AnimationEventTypes::Attack) {
					Vector3 offset(nextEvent.datas[0].x, nextEvent.datas[0].y, nextEvent.datas[0].z);
					Vector3 scale(nextEvent.datas[1].x, nextEvent.datas[1].y, nextEvent.datas[1].z);
					Attack(offset, scale, nextEvent.datas[0].w, nextEvent.datas[1].w == 1 ? true : false);
				}

				_nextAnimationEventIndex++;
			}
		}
	}
	else
	{
		_nextAnimationSpeed = 1.0f;
		_nextAnimationEventIndex = 0;
	}
}

void Animator::LoadAnimationEvents(const string& path)
{
	tinyxml2::XMLDocument doc;

	doc.LoadFile(path.c_str());

	XMLNode* node = doc.FirstChild();
	XMLElement* animation = node->FirstChildElement();
	while (true)
	{
		if (animation == nullptr)
			break;
		string animationName = animation->Attribute("Name");
		XMLElement* event = animation->FirstChildElement();
		while (true)
		{
			if (event == nullptr)
				break;
			AnimationEvent animEvent;
			animEvent.Tick = event->FloatAttribute("Tick");
			string eventName(event->Name());
			if (eventName == "Speed") {
				animEvent.type = AnimationEventTypes::Speed;

				animEvent.datas[0].x = event->FloatAttribute("Speed");
			}
			if (eventName == "Attack") {
				animEvent.type = AnimationEventTypes::Attack;

				animEvent.datas[0].x = event->FloatAttribute("OffsetX");
				animEvent.datas[0].y = event->FloatAttribute("OffsetY");
				animEvent.datas[0].z = event->FloatAttribute("OffsetZ");
				animEvent.datas[0].w = event->FloatAttribute("Damage");

				animEvent.datas[1].x = event->FloatAttribute("ScaleX", 1.0f);
				animEvent.datas[1].y = event->FloatAttribute("ScaleY", 1.0f);
				animEvent.datas[1].z = event->FloatAttribute("ScaleZ", 1.0f);
				animEvent.datas[1].w = event->BoolAttribute("IsHostile") ? 1 : 0;
			}
			_animationEvents[animationName].push_back(animEvent);
			event = event->NextSiblingElement();
		}

		animation = node->NextSiblingElement();
	}
}

void Animator::SetPreviewMode(bool value)
{
	if (value)
	{
		_isPreviewMode = true;
		_isPreviewPlaying = true;
	}
	else
	{
		_isPreviewMode = false;
		_isPreviewPlaying = false;
		_previewAnimation = nullptr;
		_previewTick = 0.0f;
	}
}

void Animator::Attack(Vector3 offset, Vector3 scale, float damage, bool isHostile)
{
	shared_ptr<GameObject> attackColliderObj = make_shared<GameObject>();
	attackColliderObj->GetTransform()->SetPosition(GetTransform()->GetPosition());
	attackColliderObj->GetTransform()->SetQuaternion(GetTransform()->GetQuaternion());

	auto attackCollider = make_shared<BoxCollider>();
	attackCollider->SetTrigger(true);
	attackCollider->SetOffset(offset);
	attackColliderObj->AddComponent(attackCollider);

	attackColliderObj->Delete(1.0f);
	RENDER->AddGameObject(attackColliderObj);
}
