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
	void SetLocalPosition(Vector3 position) { _localPosition = position; UpdateTransform(); }
	Vector3 GetLocalRotation() { return _localRotation; }
	void SetLocalRotation(Vector3 rotation) { _localRotation = rotation; UpdateTransform(); }
	Vector3 GetLocalScale() { return _localScale; }
	void SetLocalScale(Vector3 scale) { _localScale = scale; UpdateTransform(); }

	Vector3 GetPosition() { return _position; }
	void SetPosition(Vector3 position) { _position = position; }
	Vector3 GetRotation() { return _rotation; }
	void SetRotation(Vector3 rotation) { _rotation = rotation; }
	Vector3 GetScale() { return _scale; }
	void SetScale(Vector3 scale) { _scale = scale; }

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

