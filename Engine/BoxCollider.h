#pragma once
#include "Collider.h"

class BoxCollider : public Collider
{
	using Super = Collider;
public:
	BoxCollider();
	virtual ~BoxCollider();

public:
	virtual bool IsCollide(shared_ptr<Collider>& other)override;

	BoundingOrientedBox GetBoundingBox() { return _boundingBox; }

private:
	
	BoundingOrientedBox _boundingBox;
};

