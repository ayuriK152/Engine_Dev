#pragma once

enum KeyValue
{
	A		=		'A',
	B		=		'B',
	C		=		'C',
	D		=		'D',
	E		=		'E',
	F		=		'F',
	G		=		'G',
	H		=		'H',
	I		=		'I',
	J		=		'J',
	K		=		'K',
	L		=		'L',
	M		=		'M',
	N		=		'N',
	O		=		'O',
	P		=		'P',
	Q		=		'Q',
	R		=		'R',
	S		=		'S',
	T		=		'T',
	U		=		'U',
	V		=		'V',
	W		=		'W',
	X		=		'X',
	Y		=		'Y',
	Z		=		'Z',
	ESC		=	VK_ESCAPE,
	F1		=	VK_F1,
	F2		=	VK_F2,
	F3		=	VK_F3,
	F4		=	VK_F4,
	F5		=	VK_F5,
	F6		=	VK_F6,
	F7		=	VK_F7,
	F8		=	VK_F8,
	F9		=	VK_F9,
	F10		=	VK_F10,
	F11		=	VK_F11,
	F12		=	VK_F12,
	CTRL	=	VK_CONTROL,
	SHIFT	=	VK_SHIFT,
	ALT		=	VK_MENU
};

enum MouseButton
{

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

	bool IsMouseCenterFixed() { return _isMouseCenterFixed; }
	void SetMouseCenterFixMode(bool value) { _isMouseCenterFixed = value; }

	Vector2 GetMousePosition() { return _mousePosition; }
	Vector2 GetMouseDelta() { return _mouseDelta; }
	bool IsMouseLeftButtonDown() { return _isMouseLeftButtonDown; }
	bool IsMouseRightButtonDown() { return _isMouseRightButtonDown; }

	void OnMouseClick(USHORT buttonFlags);
	void OnMouseMove(int x, int y);

private:
	bool _isMouseMoving = false;
	bool _isMouseCenterFixed = false;
	bool _isMouseRightButtonDown = false;
	bool _isMouseLeftButtonDown = false;
	Vector2 _mousePosition;
	Vector2 _mouseDelta;
	unordered_map<KeyValue, KeyState> _keyStates;
};

