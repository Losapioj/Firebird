#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <windows.h>
#include <fstream>
using std::ifstream;

#include "EventMngr.h"
#include "Graphics.h"

/////////////////////////////////////////////////
LRESULT CALLBACK WinProc(HWND, UINT, 
						 WPARAM, LPARAM);

void GetDesktopResolution(UINT& horizontal, UINT& vertical);

/////////////////////////////////////////////////
class cWindow : public iListener
{
protected:
	HWND m_hWnd;
	UINT m_width, m_height;
	bool m_fullscreen, m_windowed;
	cGraphics* m_graphics;

	EventID AppNameReq;
	EventID AppHandleReq;

	EventID WinHandleReq;
	EventID WinWidthReq;
	EventID WinHeightReq;
	EventID WinFullScreenReq;

public:
	cWindow();
	~cWindow();
	HWND Init(const string&, HINSTANCE);
	virtual void HandleEvent(cEvent*);

	UINT GetWidth() const
	{  return m_width;  }
	UINT GetHeight() const
	{  return m_height;  }
	bool IsFullscreen() const
	{  return m_fullscreen;  }
	HWND GetWindowHandle() const
	{  return m_hWnd;  }
};

/////////////////////////////////////////////////
LRESULT CALLBACK WinProc (HWND, UINT, 
						  WPARAM, LPARAM);

#endif // __WINDOW_H__