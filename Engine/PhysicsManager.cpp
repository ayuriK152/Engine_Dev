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
	float massA = rba != nullptr ? rba->mass : 0.0f;
	float massB = rbb != nullptr ? rbb->mass : 0.0f;

	float resolveValue = massA == 0.0f || massB == 0.0f ? 1.0f : 0.5f;

	Vector3 correctionA = MathHelper::VectorMultiply(collInfo.Normal, collInfo.Depth * resolveValue);
	Vector3 correctionB = MathHelper::VectorMultiply(collInfo.Normal, collInfo.Depth * resolveValue);

	if (rba != nullptr)
	{
		Vector3 pos = rba->GetTransform()->GetPosition();
		Vector3 vel = MathHelper::VectorReflect(MathHelper::VectorMultiply(rba->GetVelocity(), rba->elasticModulus), collInfo.Normal);
		rba->SetVelocity(vel);
		Vector3 finalPos = MathHelper::VectorAddition(MathHelper::VectorSubtract(pos, correctionA), MathHelper::VectorMultiply(vel, collInfo.Depth * resolveValue));

		rba->GetTransform()->SetPosition(finalPos);
	}

	if (rbb != nullptr)
	{
		Vector3 pos = rbb->GetTransform()->GetPosition();
		Vector3 vel = MathHelper::VectorReflect(MathHelper::VectorMultiply(rbb->GetVelocity(), rbb->elasticModulus), collInfo.Normal);
		rbb->SetVelocity(MathHelper::VectorReflect(MathHelper::VectorMultiply(rbb->GetVelocity(), rbb->elasticModulus), collInfo.Normal));
		Vector3 finalPos = MathHelper::VectorAddition(MathHelper::VectorAddition(pos, correctionB), MathHelper::VectorMultiply(vel, collInfo.Depth * resolveValue));

		rbb->GetTransform()->SetPosition(finalPos);
	}
}
