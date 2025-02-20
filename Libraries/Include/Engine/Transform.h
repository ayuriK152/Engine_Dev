#pragma once
#include "Component.h"

class Transform : public Component
{
	using Super = Component;
public:
	Transform();
	virtual ~Transform();

public:
	void UpdateTransform();

	Vector3 GetLocalPosition() { return _localPosition; }
	void SetLocalPosition(const Vector3& position) { _localPosition = position; UpdateTransform(); }
	Vector3 GetLocalRotation() { return MathHelper::RadianToDegree(_localRotation); }
	void SetLocalRotation(const Vector3& rotation) {
		SetLocalRotationRadian(MathHelper::DegreeToRadian(rotation));
	}
	Vector3 GetLocalRotationRadian() { return _localRotation; }
	void SetLocalRotationRadian(const Vector3& rotation) { 
		_localRotation.x = rotation.x;
		_localRotation.y = rotation.y;
		_localRotation.z = rotation.z;
		UpdateTransform(); 
	}
	Vector3 GetLocalScale() { return _localScale; }
	void SetLocalScale(const Vector3& scale) { _localScale = scale; UpdateTransform(); }

	Vector3 GetPosition() { return _position; }
	void SetPosition(const Vector3& worldPosition);
	Vector3 GetRotation() { return MathHelper::RadianToDegree(_rotation); }
	void SetRotation(const Vector3& worldRotation) { SetRotationRadian(MathHelper::DegreeToRadian(worldRotation)); }
	Vector3 GetRotationRadian() { return _rotation; }
	void SetRotationRadian(const Vector3& worldRotation);
	Vector3 GetScale() { return _scale; }
	void SetScale(const Vector3& worldScale);

	Vector3 GetLook();
	Vector3 GetUp();

	void Translate(const Vector3& moveVec);

	void Rotate(const Vector3& angle);
	void Rotate(const XMVECTOR& angle);

	void LookAt(const Vector3& targetPos);

	XMFLOAT4X4 GetWorldMatrix();

	bool HasParent() { return _parent != nullptr; }

private:
	Vector3 _localPosition;
	Vector3 _localRotation;
	Vector3 _localScale;

	Vector3 _position;
	Vector3 _rotation;
	Vector3 _scale;

	XMFLOAT4X4 _matLocal;
	XMFLOAT4X4 _matWorld;

	shared_ptr<Transform> _parent;
	vector<shared_ptr<Transform>> _childs;
};

