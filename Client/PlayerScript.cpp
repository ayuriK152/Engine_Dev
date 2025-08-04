#include "pch.h"
#include "PlayerScript.h"

void PlayerScript::Init()
{
	gameObject = GetGameObject();
	transform = gameObject->GetTransform();
	animator = gameObject->GetComponent<Animator>();
}

void PlayerScript::Update()
{
	// Camera Controll
	{
		transform->Rotate(Vector3(0.0f, 2.0f * TIME->DeltaTime(), 0.0f));
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

	Vector3 targetPos = transform->GetPosition();
	if (INPUTM->IsKeyPress(KeyValue::W))
	{
		transform->Translate(MathHelper::VectorMultiply(Vector3(0.0f, 0.0f, 1.0f), TIME->DeltaTime() * speed));
		//targetPos = MathHelper::VectorAddition(targetPos, MathHelper::VectorSubtract(Camera::GetEyePos(), ::transform->GetPosition()));
		//targetPos.y = ::transform->GetPosition().y;
		animator->SetCurrentAnimation("walk");

		flag = true;
	}
	if (INPUTM->IsKeyPress(KeyValue::S))
	{
		transform->Translate(MathHelper::VectorMultiply(Vector3(0.0f, 0.0f, 1.0f), -TIME->DeltaTime() * speed));
		//targetPos = MathHelper::VectorAddition(targetPos, MathHelper::VectorSubtract(::transform->GetPosition(), Camera::GetEyePos()));
		animator->SetCurrentAnimation("walk");

		flag = true;
	}
	if (INPUTM->IsKeyPress(KeyValue::A))
	{
		transform->Translate(MathHelper::VectorMultiply(Vector3(1.0f, 0.0f, 0.0f), -TIME->DeltaTime() * speed));
		animator->SetCurrentAnimation("walk");

		flag = true;
	}
	if (INPUTM->IsKeyPress(KeyValue::D))
	{
		transform->Translate(MathHelper::VectorMultiply(Vector3(1.0f, 0.0f, 0.0f), TIME->DeltaTime() * speed));
		animator->SetCurrentAnimation("walk");

		flag = true;
	}

	if (flag)
		transform->LookAt(targetPos);

	return flag;
}
