#include "pch.h"
#include "InputManager.h"

void InputManager::Initialize()
{
	auto _keyValues = magic_enum::enum_values<KeyValue>();
	for (auto& keyValue : _keyValues)
	{
		auto keyEnum = magic_enum::enum_cast<KeyValue>(keyValue);
		_keyStates[keyEnum.value()] = KeyState::Idle;
	}
}

void InputManager::Update()
{
	MouseButtonUpdate();

	if (!_isMouseMoving)
	{
		_mouseDelta = Vector2(0.0f, 0.0f);
	}

	auto _keyValues = magic_enum::enum_values<KeyValue>();
	for (auto& keyValue : _keyValues)
	{
		auto keyEnum = magic_enum::enum_cast<KeyValue>(keyValue);
		if (GetAsyncKeyState(keyValue) & 0x8000)
		{
			if (_keyStates[keyEnum.value()] == KeyState::Idle)
				_keyStates[keyEnum.value()] = KeyState::Down;
			else if (_keyStates[keyEnum.value()] == KeyState::Down)
				_keyStates[keyEnum.value()] = KeyState::Press;
		}
		else
		{
			if (_keyStates[keyEnum.value()] == KeyState::Down || 
				_keyStates[keyEnum.value()] == KeyState::Press)
				_keyStates[keyEnum.value()] = KeyState::Up;
			else
				_keyStates[keyEnum.value()] = KeyState::Idle;
		}
	}

	_isMouseMoving = false;
}

bool InputManager::IsKeyDown(KeyValue key)
{
	return _keyStates[key] == KeyState::Down;
}

bool InputManager::IsKeyPress(KeyValue key)
{
	return _keyStates[key] == KeyState::Press;
}

void InputManager::OnMouseClick(USHORT buttonFlags)
{
	if (buttonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
	{
		_mouseStates.LeftButton = KeyState::Down;
	}
	else if (buttonFlags & RI_MOUSE_LEFT_BUTTON_UP)
	{
		_mouseStates.LeftButton = KeyState::Up;
	}

	if (buttonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
	{
		_mouseStates.RightButton = KeyState::Down;
	}
	else if (buttonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
	{
		_mouseStates.RightButton = KeyState::Up;
	}
}

void InputManager::OnMouseMove(int x, int y)
{
	_isMouseMoving = true;
	_mouseDelta = Vector2((float)x, (float)y);

	if (_isMouseCenterFixed)
	{
		RECT rect;
		GetClientRect(GRAPHIC->GetMainWnd(), &rect);
		POINT center = { (rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2 };
		ClientToScreen(GRAPHIC->GetMainWnd(), &center);
		_mousePosition.x = center.x;
		_mousePosition.y = center.y;
		SetCursorPos(center.x, center.y);
		//SetCursorPos(_mousePosition.x, _mousePosition.y);
	}
}

void InputManager::MouseButtonUpdate()
{
	if (_mouseStates.LeftButton == KeyState::Down)
	{
		if (_mouseStates.IsLeftButtonLazyUpdate)
		{
			_mouseStates.IsLeftButtonLazyUpdate = false;
			_mouseStates.LeftButton = KeyState::Press;
		}
		else
		{
			_mouseStates.IsLeftButtonLazyUpdate = true;
		}
	}

	if (_mouseStates.LeftButton == KeyState::Up)
	{
		if (_mouseStates.IsLeftButtonLazyUpdate)
		{
			_mouseStates.IsLeftButtonLazyUpdate = false;
			_mouseStates.LeftButton = KeyState::Idle;
		}
		else
		{
			_mouseStates.IsLeftButtonLazyUpdate = true;
		}
	}

	if (_mouseStates.RightButton == KeyState::Down)
	{
		if (_mouseStates.IsRightButtonLazyUpdate)
		{
			_mouseStates.IsRightButtonLazyUpdate = false;
			_mouseStates.RightButton = KeyState::Press;
		}
		else
		{
			_mouseStates.IsRightButtonLazyUpdate = true;
		}
	}

	if (_mouseStates.RightButton == KeyState::Up)
	{
		if (_mouseStates.IsRightButtonLazyUpdate)
		{
			_mouseStates.IsRightButtonLazyUpdate = false;
			_mouseStates.RightButton = KeyState::Idle;
		}
		else
		{
			_mouseStates.IsRightButtonLazyUpdate = true;
		}
	}
}
