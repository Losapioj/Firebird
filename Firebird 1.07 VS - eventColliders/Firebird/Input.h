#ifndef __INPUT_H__
#define __INPUT_H__

/////////////////////////////////////////////////
// Identify the version of DirectInput 
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "EventData.h"

/////////////////////////////////////////////////
class cInput
{
private:
	// Pointer to the DirectInput Interface object
	LPDIRECTINPUT8 m_dinput;

	//keyboard input 
	LPDIRECTINPUTDEVICE8 m_keyboard;
	char m_keys[2][256];

	//mouse input
	LPDIRECTINPUTDEVICE8 m_mouse;
	DIMOUSESTATE m_mouseState[2];

	int m_curIndex, m_oldIndex;

	EventID KeyDownEvent;
	EventID KeyPressEvent;
	EventID KeyUpEvent;
	EventID MouseMoveEvent;
	EventID MouseScrollEvent;
	EventID MouseButtonDownEvent;
	EventID MouseButtonPressEvent;
	EventID MouseButtonUpEvent;
	//EventID WinHandleRequest;

	int MouseX();
	int MouseY();
	int MouseZ();

	bool ButtonDown(int);
	bool ButtonPress(int);
	bool ButtonUp(int);

	bool KeyDown(int);
	bool KeyPress(int);
	bool KeyUp(int);

public:
	// Default constructor
	cInput();

	// Destructor
	~cInput();  

	// Initialization
	bool Init(HWND);

	void PollInput();   
};

/////////////////////////////////////////////////
#endif // __INPUT_H__