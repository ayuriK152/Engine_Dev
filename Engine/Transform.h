#pragma once
#include "Component.h"

class Transform : public Component
{
	using Super = Component;
public:
	Transform();
	virtual ~Transform();

	void Update() override;
	void Render() override;

public:

	Vector3 GetLocalPosition() { 
		if (_isDirty)
			UpdateTransform();

		return _localPosition; 
	}
	void SetLocalPosition(const Vector3& position) { 
		_localPosition = position; 
		SetDirtyFlag();
	}
	
	// Get/Set Local Rotation With Degree
	Vector3 GetLocalRotation() {
		if (_isDirty)
			UpdateTransform();

		return MathHelper::RadianToDegree(_localRotation);
	}
	void SetLocalRotation(const Vector3& rotation) {
		SetLocalRotationRadian(MathHelper::DegreeToRadian(rotation));
	}
	// Get/Set Local Rotation With Radian
	Vector3 GetLocalRotationRadian() {
		if (_isDirty)
			UpdateTransform();

		return _localRotation;
	}
	void SetLocalRotationRadian(const Vector3& rotation);

	Vector4 GetLocalQuaternion() {
		if (_isDirty)
			UpdateTransform();

		return _quaternion;
	}
	void SetLocalQuaternion(const Vector4& quaternion);
	void SetLocalQuaternion(const XMVECTOR& quaternion);

	Vector3 GetLocalScale() {
		if (_isDirty)
			UpdateTransform();

		return _localScale;
	}
	void SetLocalScale(const Vector3& scale) { 
		_localScale = scale; 
		SetDirtyFlag();
	}

	Vector3 GetPosition() { 
		if (_isDirty)
			UpdateTransform();

		return _position; 
	}
	void SetPosition(const Vector3& worldPosition);

	// Get/Set Rotation With Degree
	Vector3 GetRotation() {
		if (_isDirty)
			UpdateTransform();

		return MathHelper::RadianToDegree(_rotation); 
	}
	void SetRotation(const Vector3& worldRotation) { SetRotationRadian(MathHelper::DegreeToRadian(worldRotation)); }

	// Get/Set Rotation With Radian
	Vector3 GetRotationRadian() {
		if (_isDirty)
			UpdateTransform(); 

		return _rotation;
	}
	void SetRotationRadian(const Vector3& worldRotation);
	Vector3 GetScale() {
		if (_isDirty)
			UpdateTransform();

		return _scale;
	}
	void SetScale(const Vector3& worldScale);

	XMVECTOR GetQuaternion() {
		if (_isDirty)
			UpdateTransform();

		return XMLoadFloat4(&_quaternion);
	}
	XMMATRIX GetRotationMatrix();

	Vector3 GetRight();
	Vector3 GetLeft();
	Vector3 GetUp();
	Vector3 GetDown();
	Vector3 GetLook();
	Vector3 GetBack();

	void Translate(const Vector3& moveVec);

	void Rotate(const Vector3& angle);
	void Rotate(const Vector4& quat);
	void Rotate(const XMVECTOR& angle);

	void LookAt(const Vector3& targetPos);
	void LookAtWithNoRoll(const Vector3& targetPos);

	void SetLocalMatrix(XMFLOAT4X4 mat);
	void SetWorldMatrix(XMFLOAT4X4 mat);

	XMFLOAT4X4 GetLocalMatrix();
	XMFLOAT4X4 GetWorldMatrix();

	shared_ptr<Transform> GetParent() { return _parent; }
	void SetParent(shared_ptr<Transform> parent);

	const vector<shared_ptr<Transform>>& GetChilds() { return _childs; }
	void AddChild(shared_ptr<Transform> child);

	bool HasParent() { return _parent != nullptr; }

	void SetDirtyFlag();

private:
	void UpdateTransform();

private:
	bool _isDirty = false;

	Vector3 _localPosition;
	Vector3 _localRotation;
	Vector3 _localScale;

	Vector3 _position;
	Vector3 _rotation;
	Vector3 _scale;

	Vector4 _quaternion;

	XMFLOAT4X4 _matLocal;
	XMFLOAT4X4 _matWorld;

	shared_ptr<Transform> _parent;
	vector<shared_ptr<Transform>> _childs;
};

