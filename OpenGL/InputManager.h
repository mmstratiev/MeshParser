#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <QPoint>
#include <QCursor>

class CInputManager
{
public:
	static void		SetKeyPressed(Qt::Key key, bool isPressed);
	static void		SetButtonPressed(Qt::MouseButton button, bool isPressed);

	static bool		IsKeyPressed(Qt::Key key);
	static bool		IsButtonPressed(Qt::MouseButton button);

	static void		UpdateMousePosition();
	static QPoint	GetMousePosition();
	static QPoint	GetMouseDelta();

private:
	static QPoint MouseCurrPos;
	static QPoint MouseLastPos;
	static std::vector<Qt::Key>			PressedKeys;
	static std::vector<Qt::MouseButton> PressedButtons;
};

#endif // INPUTMANAGER_H
