#pragma once

enum KeyValue
{
	A =		'A',
	B =		'B',
	C =		'C',
	D =		'D',
	E =		'E',
	F =		'F',
	G =		'G',
	H =		'H',
	I =		'I',
	J =		'J',
	K =		'K',
	L =		'L',
	M =		'M',
	N =		'N',
	O =		'O',
	P =		'P',
	Q =		'Q',
	R =		'R',
	S =		'S',
	T =		'T',
	U =		'U',
	V =		'V',
	W =		'W',
	X =		'X',
	Y =		'Y',
	Z =		'Z',
	ESC =	VK_ESCAPE,
	F1 =	VK_F1,
	F2 =	VK_F2,
	F3 =	VK_F3,
	F4 =	VK_F4,
	F5 =	VK_F5,
	F6 =	VK_F6,
	F7 =	VK_F7,
	F8 =	VK_F8,
	F9 =	VK_F9,
	F10 =	VK_F10,
	F11 =	VK_F11,
	F12 =	VK_F12
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
	~InputManager() = default;

public:
	void Initialize();
	void Update();

	bool IsKeyDown(KeyValue key);
	bool IsKeyPress(KeyValue key);

private:
	unordered_map<KeyValue, KeyState> _keyStates;
};

