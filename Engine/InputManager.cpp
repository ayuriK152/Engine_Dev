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

void InputManager::OnMouseMove(int x, int y)
{
	_isMouseMoving = true;
	_mouseDelta = Vector2((float)x, (float)y);

	if (_isMouseCenterFixed)
	{
		RECT rectClient, rectWindow;
		GetClientRect(GRAPHIC->GetMainWnd(), &rectClient);
		GetWindowRect(GRAPHIC->GetMainWnd(), &rectWindow);
		int centerX = (rectClient.left + rectClient.right) / 2;
		int centerY = (rectClient.top + rectClient.bottom) / 2;
		SetCursorPos(centerX + rectWindow.left, centerY + rectWindow.top);
		_mousePosition = Vector2((float)centerX, (float)centerY);
	}
}
