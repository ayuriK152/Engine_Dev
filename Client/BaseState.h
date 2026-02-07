#pragma once

class Script;

template <typename T>
class BaseState
{
public:
	virtual ~BaseState() { }

	virtual void StateStart(T* owner) { }
	virtual void StateUpdate(T* owner) { }
};

