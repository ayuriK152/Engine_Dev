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
		transform->Translate(Vector3(0.0f, 0.0f, 1.0f) * TIME->DeltaTime() * speed);
		animator->SetCurrentAnimation("walk");
		targetPos = targetPos + Vector3(0.0f, 0.0f, -1.0f);

		flag = true;
	}
	if (INPUTM->IsKeyPress(KeyValue::S))
	{
		transform->Translate(Vector3(0.0f, 0.0f, -1.0f) * TIME->DeltaTime() * speed);
		animator->SetCurrentAnimation("walk");
		targetPos = targetPos + Vector3(0.0f, 0.0f, 1.0f);

		flag = true;
	}
	if (INPUTM->IsKeyPress(KeyValue::A))
	{
		transform->Translate(Vector3(-1.0f, 0.0f, 0.0f) * TIME->DeltaTime() * speed);
		animator->SetCurrentAnimation("walk");
		targetPos = targetPos + Vector3(1.0f, 0.0f, 0.0f);

		flag = true;
	}
	if (INPUTM->IsKeyPress(KeyValue::D))
	{
		transform->Translate(Vector3(1.0f, 0.0f, 0.0f) * TIME->DeltaTime() * speed);
		animator->SetCurrentAnimation("walk");
		targetPos = targetPos + Vector3(-1.0f, 0.0f, 0.0f);

		flag = true;
	}

	if (flag)
		transform->LookAt(targetPos);

	return flag;
}
