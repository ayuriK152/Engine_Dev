#pragma once

class Script;

class BaseState
{
public:
	virtual ~BaseState() { }

	virtual void StateStart() { }
	virtual void StateUpdate() { }
};

