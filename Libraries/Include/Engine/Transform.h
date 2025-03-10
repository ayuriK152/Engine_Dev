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
	
	// Get/Set Local Rotation With Degree
	Vector3 GetLocalRotation() { return MathHelper::RadianToDegree(_localRotation); }
	void SetLocalRotation(const Vector3& rotation) {
		SetLocalRotationRadian(MathHelper::DegreeToRadian(rotation));
	}
	// Get/Set Local Rotation With Radian
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

	// Get/Set Rotation With Degree
	Vector3 GetRotation() { return MathHelper::RadianToDegree(_rotation); }
	void SetRotation(const Vector3& worldRotation) { SetRotationRadian(MathHelper::DegreeToRadian(worldRotation)); }
	// Get/Set Rotation With Radian
	Vector3 GetRotationRadian() { return _rotation; }
	void SetRotationRadian(const Vector3& worldRotation);
	Vector3 GetScale() { return _scale; }
	void SetScale(const Vector3& worldScale);

	Vector3 GetRight();
	Vector3 GetLeft();
	Vector3 GetUp();
	Vector3 GetDown();
	Vector3 GetLook();
	Vector3 GetBack();

	void Translate(const Vector3& moveVec);

	void Rotate(const Vector3& angle);
	void Rotate(const XMVECTOR& angle);

	void LookAt(const Vector3& targetPos);

	XMFLOAT4X4 GetWorldMatrix();
	XMFLOAT4X4 GetTexTransform() { return _texTransform; }		// 반드시 수정

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
	XMFLOAT4X4 _texTransform;

	shared_ptr<Transform> _parent;
	vector<shared_ptr<Transform>> _childs;
};

