#pragma once

enum KeyValue
{
	A = 'A',
	B = 'B',
	C = 'C',
	D = 'D',
	E = 'E',
	F = 'F',
	G = 'G',
	H = 'H',
	I = 'I',
	J = 'J',
	K = 'K',
	L = 'L',
	M = 'M',
	N = 'N',
	O = 'O',
	P = 'P',
	Q = 'Q',
	R = 'R',
	S = 'S',
	T = 'T',
	U = 'U',
	V = 'V',
	W = 'W',
	X = 'X',
	Y = 'Y',
	Z = 'Z',
	ESC = VK_ESCAPE
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

	bool IsKeyDown(KeyValue key);
	bool IsKeyPress(KeyValue key);

private:
	unordered_map<KeyValue, KeyState> _keyStates;
};

