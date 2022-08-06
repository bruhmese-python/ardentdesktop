#pragma once

#include <irrlicht.h>
#include "driverChoice.h"

class MyEventReceiver : public irr::IEventReceiver
{
public:
	struct SMouseState
	{
		irr::core::position2di Position;
		bool LeftButtonDown, RightButtonDown;
		SMouseState() : LeftButtonDown(false) , RightButtonDown(false) { }
	} MouseState;

	virtual bool OnEvent(const irr::SEvent& event)
	{
		if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
		{
			switch (event.MouseInput.Event)
			{
			case irr::EMIE_LMOUSE_PRESSED_DOWN:
				MouseState.LeftButtonDown = true;
				break;

			case irr::EMIE_LMOUSE_LEFT_UP:
				MouseState.LeftButtonDown = false;
				break;
			case irr::EMIE_RMOUSE_PRESSED_DOWN:
				MouseState.RightButtonDown = true;
				break;

			case irr::EMIE_RMOUSE_LEFT_UP:
				MouseState.RightButtonDown = false;
				break;

			case irr::EMIE_MOUSE_MOVED:
				MouseState.Position.X = event.MouseInput.X;
				MouseState.Position.Y = event.MouseInput.Y;
				break;

			default:
				break;
			}
		}

		return false;
	}

	const SMouseState& GetMouseState(void) const
	{
		return MouseState;
	}


	MyEventReceiver()
	{
	}

private:
	irr::SEvent::SJoystickEvent JoystickState;
};