#include "InputManager.h"
#include <algorithm>

std::vector<Qt::Key> CInputManager::PressedKeys;
std::vector<Qt::MouseButton> CInputManager::PressedButtons;
QPoint CInputManager::MouseCurrPos;
QPoint CInputManager::MouseLastPos;

void CInputManager::SetKeyPressed(Qt::Key key, bool isPressed)
{
	auto it = std::find(PressedKeys.begin(), PressedKeys.end(), key);
	if(isPressed)
	{
		if(it == PressedKeys.end())
		{
			PressedKeys.push_back(key);
		}
	}
	else
	{
		if(it != PressedKeys.end())
		{
			PressedKeys.erase(it);
		}
	}
}

void CInputManager::SetButtonPressed(Qt::MouseButton button, bool isPressed)
{
	auto it = std::find(PressedButtons.begin(), PressedButtons.end(), button);

	if(isPressed)
	{
		if(it == PressedButtons.end())
		{
			PressedButtons.push_back(button);
		}
	}
	else
	{
		if(it != PressedButtons.end())
		{
			PressedButtons.erase(it);
		}
	}
}

bool CInputManager::IsKeyPressed(Qt::Key key)
{
	return std::find(PressedKeys.begin(), PressedKeys.end(), key) != PressedKeys.end();
}

bool CInputManager::IsButtonPressed(Qt::MouseButton button)
{
	return std::find(PressedButtons.begin(), PressedButtons.end(), button) != PressedButtons.end();
}

QPoint CInputManager::GetMousePosition()
{
	return QCursor::pos();
}

QPoint CInputManager::GetMouseDelta()
{
	return MouseCurrPos-MouseLastPos;
}

void CInputManager::UpdateMousePosition()
{
	MouseLastPos = MouseCurrPos;
	MouseCurrPos = QCursor::pos();
}
