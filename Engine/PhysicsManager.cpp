#include "pch.h"
#include "PhysicsManager.h"

PhysicsManager::~PhysicsManager()
{

}

void PhysicsManager::Init()
{

}

void PhysicsManager::FixedUpdate()
{

}

void PhysicsManager::Update()
{
	if (_colliders.size() == 0)
		return;

	for (auto& collider : _colliders)
	{
		collider->_isOnColliding = false;
		collider->Update();
	}

	for (int i = 0; i < _colliders.size() - 1; i++)
	{
		for (int j = i + 1; j < _colliders.size(); j++)
		{
			Vector3 normal;
			float depth;
			CollisionInfo collInfo = _colliders[i]->CheckCollide(_colliders[j]);

			if (collInfo.IsCollide)
			{
				_colliders[i]->_isOnColliding = true;
				_colliders[j]->_isOnColliding = true;

				shared_ptr<Rigidbody> rba = _colliders[i]->GetGameObject()->GetComponent<Rigidbody>();
				shared_ptr<Rigidbody> rbb = _colliders[j]->GetGameObject()->GetComponent<Rigidbody>();

				if (rba != nullptr || rbb != nullptr)
					ResolvePenetration(collInfo, rba, rbb);
			}
		}
	}
}

void PhysicsManager::LateUpdate()
{

}

void PhysicsManager::DeleteCollider(shared_ptr<Collider> collider)
{
	for (int i = 0; i < _colliders.size(); i++)
	{
		if (_colliders[i] == collider)
		{
			_colliders.erase(_colliders.begin() + i);
			break;
		}
	}
}

void PhysicsManager::ResolvePenetration(CollisionInfo& collInfo, shared_ptr<Rigidbody>& rba, shared_ptr<Rigidbody>& rbb)
{
	// 회전을 구현해두긴 했는데 조금 이상함. 개선 작업을 추후에 진행할 예정. 그 전까지 건드리지 않기.

	float massA = rba != nullptr ? rba->mass : 0.0f;
	float massB = rbb != nullptr ? rbb->mass : 0.0f;

	float resolveValue = massA == 0.0f || massB == 0.0f ? 1.0f : 0.5f;

	Vector3 correctionA = collInfo.Normal * collInfo.Depth * resolveValue;
	Vector3 correctionB = collInfo.Normal * collInfo.Depth * resolveValue;

	// 회전 운동 임펄스
	Vector3 impulse = collInfo.Normal * collInfo.Depth;
	
	if (rba != nullptr)
	{
		Vector3 pos = rba->GetTransform()->GetPosition();
		Vector3 vel = (rba->GetVelocity() * rba->elasticModulus).Reflect(collInfo.Normal);
		rba->SetVelocity(vel);
		Vector3 finalPos = pos - correctionA + vel * collInfo.Depth * resolveValue;
		//Vector3 finalPos = MathHelper::VectorAddition(MathHelper::VectorSubtract(pos, correctionA), MathHelper::VectorMultiply(vel, collInfo.Depth * resolveValue));

		rba->GetTransform()->SetPosition(finalPos);

		float velocityPower = XMVector3Length(XMLoadFloat3(&vel)).m128_f32[0];
		Vector3 r =collInfo.ContactPoint - rba->GetTransform()->GetPosition();
		Vector3 torque;
		XMStoreFloat3(&torque, XMVector3Cross(XMLoadFloat3(&impulse), XMLoadFloat3(&r)) * velocityPower);
		rba->AddTorque(torque);
	}

	if (rbb != nullptr)
	{
		Vector3 pos = rbb->GetTransform()->GetPosition();
		Vector3 vel = (rbb->GetVelocity() * rbb->elasticModulus).Reflect(collInfo.Normal);
		rbb->SetVelocity(vel);
		Vector3 finalPos = pos + correctionB + vel * collInfo.Depth * resolveValue;

		rbb->GetTransform()->SetPosition(finalPos);

		float velocityPower = XMVector3Length(XMLoadFloat3(&vel)).m128_f32[0];
		Vector3 r = collInfo.ContactPoint - rbb->GetTransform()->GetPosition();
		Vector3 torque;
		XMStoreFloat3(&torque, XMVector3Cross(XMLoadFloat3(&-impulse), XMLoadFloat3(&r)) * velocityPower);
		rbb->AddTorque(torque);
	}
}
