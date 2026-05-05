#include "pch.h"
#include "InputManager.h"

InputManager* InputManager::s_instance = nullptr;

InputManager* InputManager::GetInstance()
{
	if (s_instance == nullptr)
		s_instance = new InputManager();
	return s_instance;
}

Bulb::ProcessResult InputManager::Delete()
{
	if (s_instance != nullptr) {
		delete s_instance;
		s_instance = nullptr;
		return Bulb::ProcessResult::SUCCESS;
	}
	return Bulb::ProcessResult::FAILED_INSTANCE_NOT_FOUND;
}

void InputManager::Init()
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
		_mouseDelta = Bulb::Vector2(0.0f, 0.0f);
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
	if (ImGui::GetIO().WantCaptureKeyboard) return false;

	return _keyStates[key] == KeyState::Down;
}

bool InputManager::IsKeyPress(KeyValue key)
{
	if (ImGui::GetIO().WantCaptureKeyboard) return false;

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
	if (ImGui::GetIO().WantCaptureMouse) return;

	_isMouseMoving = true;
	_mouseDelta = Bulb::Vector2((float)x, (float)y);

	if (_isMouseCenterFixed) {
		static RECT rect;
		GetClientRect(GRAPHIC->GetMainWnd(), &rect);

		static POINT center;
		center = { (rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2 };
		ClientToScreen(GRAPHIC->GetMainWnd(), &center);
		_mousePosition = center;
		SetCursorPos(center.x, center.y);
	}
	else {
		GetCursorPos(&_mousePosition);
		ScreenToClient(GRAPHIC->GetMainWnd(), &_mousePosition);
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
