#pragma once

enum KeyValue
{
	A = 'A',
	D = 'D',
	S = 'S',
	W = 'W'
};

enum KeyState
{
	Idle,
	Down,
	Press,
	Up
};

class InputManager
{
	DECLARE_SINGLE(InputManager);
public:
	void Initialize();
	void Update();

	bool IsKeyPress(KeyValue key);

private:
	unordered_map<KeyValue, KeyState> _keyStates;
};

