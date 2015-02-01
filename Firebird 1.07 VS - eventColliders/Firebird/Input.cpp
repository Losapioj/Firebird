/*#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>*/
#include "Debug.h"
#include "Input.h"
#include "Game.h"

/////////////////////////////////////////////////
// Needed library files
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

/////////////////////////////////////////////////
cInput::cInput()
: m_dinput(NULL), m_keyboard(NULL), 
m_mouse(NULL), m_curIndex(0), m_oldIndex(1),
KeyDownEvent("KEY_DOWN_EVENT"),
KeyPressEvent("KEY_PRESS_EVENT"),
KeyUpEvent("KEY_UP_EVENT"),
MouseMoveEvent("MOUSE_MOVE_EVENT"),
MouseScrollEvent("MOUSE_SCROLL_EVENT"),
MouseButtonDownEvent("MOUSE_BUTTON_DOWN_EVENT"),
MouseButtonPressEvent("MOUSE_BUTTON_HELD_EVENT"),
MouseButtonUpEvent("MOUSE_BUTTON_UP_EVENT")//,
//WinHandleRequest("WINDOW_HANDLE_REQUEST")
{
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 256; ++j)
			m_keys[i][j] = 0;

		ZeroMemory(&m_mouseState[i], sizeof(DIMOUSESTATE));
	}
}

/////////////////////////////////////////////////
cInput::~cInput()
{
	#ifdef MY_DEBUG
		debug << "~cInput start" << endl;
	#endif

	if (m_keyboard != NULL)
	{
		m_keyboard->Unacquire ();
		m_keyboard->Release ();
		m_keyboard = NULL;
	}

	if (m_mouse != NULL)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = NULL;
	}

	if (m_dinput != NULL)
		m_dinput->Release();
	m_dinput = NULL;

	#ifdef MY_DEBUG
		debug << "~cInput end" << endl;
	#endif
}

/////////////////////////////////////////////////
bool cInput::Init(HWND hWnd)
{
	//debug << "Begin cInput::Init()" << endl;

	/*cEvent winReq(WinHandleRequest);
	g_eventMngr.SendEvent(&winReq);
	HWND hwnd = (HWND)(winReq.GetParam(0).m_asInt);*/

	//initialize DirectInput object
	DirectInput8Create(GetModuleHandle(NULL), 
		DIRECTINPUT_VERSION, IID_IDirectInput8, 
		(void**)&m_dinput, NULL);  

	//initialize the keyboard
	m_dinput->CreateDevice(GUID_SysKeyboard, 
		&m_keyboard, NULL);

	//set the data format for keyboard input
	m_keyboard->SetDataFormat(&c_dfDIKeyboard);

	//set the cooperative level
	m_keyboard->SetCooperativeLevel(hWnd, 
		DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

	//acquire the keyboard
	m_keyboard->Acquire();

	//initialize the mouse
	m_dinput->CreateDevice(GUID_SysMouse, 
		&m_mouse, NULL);

	//set the data format for mouse input
	m_mouse->SetDataFormat(&c_dfDIMouse);

	//set the cooperative level
	m_mouse->SetCooperativeLevel(hWnd, 
		DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

	//acquire the mouse 
	m_mouse->Acquire();

	//debug << "End cInput::Init()" << endl;
	return true;
}

/////////////////////////////////////////////////
void cInput::PollInput()
{
	//debug << "Begin cInput::PollInput()"  << endl;
	m_oldIndex = m_curIndex;
	m_curIndex = (m_curIndex + 1) % 2;

	if(m_keyboard != NULL)
	{
		m_keyboard->GetDeviceState(256, 
			(LPVOID)&m_keys[m_curIndex]);

		cEvent* ked = NULL;
		for (int i = 0; i < 256; ++i)
		{
			// if the key was up last frame
			// but is pressed this frame
			if(KeyDown(i))
			{
				ked = new cEvent(KeyDownEvent, sVarient(i));
				//debug << "Creating keyboard event at: " << ked << endl;
				g_eventMngr.PostEvent(ked);
				ked = NULL;
			}

			// if the key was down last frame
			// and is still down this frame
			if(KeyPress(i))
			{
				ked = new cEvent(KeyPressEvent, sVarient(i));
				//debug << "Creating keyboard event at: " << ked << endl;
				g_eventMngr.PostEvent(ked);
				ked = NULL;
			}

			// if the key was down  last frame
			// but is up this frame
			if(KeyUp(i))
			{
				ked = new cEvent(KeyUpEvent, sVarient(i));
				//debug << "Creating keyboard event at: " << ked << endl;
				g_eventMngr.PostEvent(ked);
				ked = NULL;
			}
		}
	}

	cEvent* med = NULL;
	int x = MouseX(), 
		y = MouseY(), 
		z = MouseZ();
	if(m_mouse != NULL)
	{
		m_mouse->GetDeviceState(
			sizeof(m_mouseState[m_curIndex]), 
			(LPVOID)(&m_mouseState[m_curIndex]));

		if(x != 0 || y != 0)
		{
			med = new cEvent(MouseMoveEvent, 
				sVarient(x), sVarient(y), sVarient(0), 
				sVarient(-1));
			g_eventMngr.PostEvent(med);
		}

		if (z != 0)
		{
			med = new cEvent(MouseScrollEvent, 
				sVarient(0), sVarient(0), sVarient(z), 
				sVarient(-1));
			g_eventMngr.PostEvent(med);
		}

		for(int i = 0; i < 4; ++i)
		{
			if(ButtonDown(i))
			{
				med = new cEvent(
					MouseButtonDownEvent, sVarient(0), 
					sVarient(0), sVarient(0), sVarient(i));
				g_eventMngr.PostEvent(med);
			}

			if(ButtonPress(i))
			{
				med = new cEvent(
					MouseButtonPressEvent, sVarient(0), 
					sVarient(0), sVarient(0), sVarient(i));
				g_eventMngr.PostEvent(med);
			}

			if(ButtonUp(i))
			{
				med = new cEvent(MouseButtonUpEvent,
					sVarient(0), sVarient(0), sVarient(0), 
					sVarient(i));
				g_eventMngr.PostEvent(med);
			}
		}
	}
	//debug << "End cInput::PollInput()"  << endl;
} 

/////////////////////////////////////////////////  
int cInput::MouseX()
{  return m_mouseState[m_curIndex].lX;  }

/////////////////////////////////////////////////
int cInput::MouseY()
{  return m_mouseState[m_curIndex].lY;  }

/////////////////////////////////////////////////
int cInput::MouseZ()
{  return m_mouseState[m_curIndex].lZ;  }

/////////////////////////////////////////////////
// return true if the key was not pressed last 
// frame but is pressed this frame
bool cInput::KeyDown(int i)
{
	return(
		(((m_keys[m_oldIndex][i]) & 0x80) == 0) &&
		(((m_keys[m_curIndex][i]) & 0x80) != 0));
}

/////////////////////////////////////////////////
// if the key was down last frame and is still
// down this frame
bool cInput::KeyPress(int i)
{
	return(
		(((m_keys[m_oldIndex][i]) & 0x80) != 0) && 
		(((m_keys[m_curIndex][i]) & 0x80) != 0));
}

/////////////////////////////////////////////////
// if the key was down  last frame but is up 
// this frame
bool cInput::KeyUp(int i)
{
	return(
		(((m_keys[m_oldIndex][i]) & 0x80) != 0) && 
		(((m_keys[m_curIndex][i]) & 0x80) == 0));
}

/////////////////////////////////////////////////
bool cInput::ButtonDown(int i)
{
	return(
		(((m_mouseState[m_oldIndex].rgbButtons[i]) & 
		0x80) == 0) &&
		(((m_mouseState[m_curIndex].rgbButtons[i]) & 
		0x80) != 0));
}

/////////////////////////////////////////////////
bool cInput::ButtonPress(int i)
{
	return(
		(((m_mouseState[m_oldIndex].rgbButtons[i]) & 
		0x80) != 0) &&
		(((m_mouseState[m_curIndex].rgbButtons[i]) & 
		0x80) != 0));
}

/////////////////////////////////////////////////
bool cInput::ButtonUp(int i)
{
	return(
		(((m_mouseState[m_oldIndex].rgbButtons[i]) & 
		0x80) != 0) &&
		(((m_mouseState[m_curIndex].rgbButtons[i]) & 
		0x80) == 0));
}