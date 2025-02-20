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
}

bool InputManager::IsKeyDown(KeyValue key)
{
	return _keyStates[key] == KeyState::Down;
}

bool InputManager::IsKeyPress(KeyValue key)
{
	return _keyStates[key] == KeyState::Press;
}
