#include "pch.h"
#include "PlayerScript.h"

shared_ptr<GameObject> gameObject;
shared_ptr<Transform> transform;
shared_ptr<Animator> animator;
float speed = 1.75f;

void PlayerScript::Init()
{
	gameObject = GetGameObject();
	::transform = gameObject->GetTransform();
	animator = gameObject->GetComponent<Animator>();
}

void PlayerScript::Update()
{
	// Camera Controll
	{

	}

	if (!Move())
	{
		animator->SetCurrentAnimation("idle");
	}
}

bool PlayerScript::Move()
{
	// �ܼ��� �̵� Ű�� �����°� ���η� �������� �Ǵ�.
	// ��� ������ �ؾ� �� ��Ȯ��. ���� ����.

	bool flag = false;

	Vector3 targetPos = ::transform->GetPosition();
	if (INPUTM->IsKeyPress(KeyValue::W))
	{
		::transform->Translate(MathHelper::VectorMultiply(Vector3(0.0f, 0.0f, 1.0f), TIME->DeltaTime() * speed));
		//targetPos = MathHelper::VectorAddition(targetPos, MathHelper::VectorSubtract(Camera::GetEyePos(), ::transform->GetPosition()));
		//targetPos.y = ::transform->GetPosition().y;
		animator->SetCurrentAnimation("walk");

		flag = true;
	}
	if (INPUTM->IsKeyPress(KeyValue::S))
	{
		::transform->Translate(MathHelper::VectorMultiply(Vector3(0.0f, 0.0f, 1.0f), -TIME->DeltaTime() * speed));
		//targetPos = MathHelper::VectorAddition(targetPos, MathHelper::VectorSubtract(::transform->GetPosition(), Camera::GetEyePos()));
		animator->SetCurrentAnimation("walk");

		flag = true;
	}
	if (INPUTM->IsKeyPress(KeyValue::A))
	{
		::transform->Translate(MathHelper::VectorMultiply(Vector3(1.0f, 0.0f, 0.0f), -TIME->DeltaTime() * speed));
		animator->SetCurrentAnimation("walk");

		flag = true;
	}
	if (INPUTM->IsKeyPress(KeyValue::D))
	{
		::transform->Translate(MathHelper::VectorMultiply(Vector3(1.0f, 0.0f, 0.0f), TIME->DeltaTime() * speed));
		animator->SetCurrentAnimation("walk");

		flag = true;
	}

	if (flag)
		::transform->LookAt(targetPos);

	return flag;
}
