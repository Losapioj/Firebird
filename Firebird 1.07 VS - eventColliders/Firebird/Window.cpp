#include <fstream>
using std::ifstream;

#include <iomanip>
using std::boolalpha;

#include "Debug.h"
#include "Window.h"

/////////////////////////////////////////////////
/* COMMON RESOLUTIONS:
	4X3: **lose 12.5% (1/8) on each side of 16x9 img**
		640x480
		800x600
		1024x768
		(1200x900)
		1280x960
		1400x1050
		1600x1200
		
	16X10: **lose 5.0% (1/20) on each side of 16x9 img**
		1280x800
		1440x900
		1680x1050
		1920x1200
		2560x1600

	16X9:
		1280x720
		1600x900
		1920x1080
		2560x1440
*/
const static int DEFAULT_WIDTH = 0; //1024
const static int DEFAULT_HEIGHT = 0; //768
const static bool DEFAULT_FULLSCREEN = false;
const static bool DEFAULT_WINDOWED = false;

/////////////////////////////////////////////////
cWindow::cWindow ()
: m_width(DEFAULT_WIDTH), m_height(DEFAULT_HEIGHT), 
m_fullscreen(DEFAULT_FULLSCREEN), 
m_windowed(DEFAULT_WINDOWED), m_hWnd(0),
AppNameReq("APPLICATION_TITLE_REQUEST"),
AppHandleReq("APPLICATION_HANDLE_REQUEST"),
WinHandleReq("WINDOW_HANDLE_REQUEST"),
WinWidthReq("WINDOW_WIDTH_REQUEST"),
WinHeightReq("WINDOW_HEIGHT_REQUEST"),
WinFullScreenReq("WINDOW_FULLSCREEN_REQUEST")
{
	g_eventMngr.RegisterListener(WinHandleReq, this);
	g_eventMngr.RegisterListener(WinWidthReq, this);
	g_eventMngr.RegisterListener(WinHeightReq, this);
	g_eventMngr.RegisterListener(WinFullScreenReq, this);
}

/////////////////////////////////////////////////
cWindow::~cWindow ()
{
	if(m_graphics != NULL)
		delete m_graphics;
	m_graphics = NULL;
}

/////////////////////////////////////////////////
HWND cWindow::Init(const string& appName, 
				   HINSTANCE hInst)
{
	#ifdef MY_DEBUG
		debug << "Begin cWindow::Init()" << endl;
	#endif

	// Declare and initialize the window class 
	// registration structure
	WNDCLASSEX wc = {sizeof(WNDCLASSEX), 
		CS_HREDRAW | CS_VREDRAW, WinProc, 0, 0, 
		hInst, NULL, LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)GetStockObject(WHITE_BRUSH), NULL, 
		appName.c_str(), NULL};

	// Register window class
	if (!RegisterClassEx (&wc))
	{
		ErrorMsg("Window registration failed");
		return NULL;
	}

	// Define window style
	// Style differs for windowed vs. full screen
	int style = 0;
	//GetDesktopResolution(m_width, m_height);
/*	if(m_fullscreen)
	{
		if (m_windowed)
		{
			//GetDesktopResolution(m_width, m_height);
			style = WS_OVERLAPPEDWINDOW;
			m_hWnd = CreateWindow(
				appName.c_str(), appName.c_str(), style, 0, 0,  
				CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInst, NULL);
		}
		else
		{
			//style = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP;
		}
		
	}
	else
	{
		style = WS_OVERLAPPEDWINDOW;
		m_hWnd = CreateWindow(
				appName.c_str(), appName.c_str(), style, 0, 0,  
				m_width, m_height, NULL, NULL, hInst, NULL);
	}*/

	if(m_fullscreen)
		style = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP;
	else
		style = WS_OVERLAPPEDWINDOW;

	// Create window instance
	m_hWnd = CreateWindow(
				appName.c_str(), appName.c_str(), style, 0, 0,  
				m_width, m_height, NULL, NULL, hInst, NULL);
	

	if (m_hWnd == NULL)
	{
		ErrorMsg("Window creation failed");
		return NULL;
	}

	//set window to fit screen.
	
	if (m_width <= 0 || m_height <= 0)
	{		
		RECT winRect;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &winRect, 0);
		SetWindowPos(m_hWnd, HWND_TOP, winRect.left, winRect.top, 
			winRect.right, winRect.bottom, SWP_NOCOPYBITS|SWP_NOACTIVATE);
//		debug << "WORK AREA (L, T, R, B): " << winRect.left << ", " << winRect.top
//			<< ", " << winRect.right << ", " << winRect.bottom << endl;
		m_width = winRect.right - winRect.left;
		m_height = winRect.bottom - winRect.top;
/*		
		GetDesktopResolution( (m_width), (m_height) );
		SetWindowPos(m_hWnd, HWND_TOP, 0, 0, m_width, m_height, 
				SWP_NOMOVE|SWP_NOACTIVATE);
*/
	}

	m_graphics = new cGraphics;
	if(!m_graphics->Init(m_hWnd, m_width, m_height, 
		m_fullscreen))
		return NULL;

	// Display window
	ShowWindow (m_hWnd, SW_SHOWNORMAL);
	UpdateWindow (m_hWnd);

	#ifdef MY_DEBUG
		debug << "End cWindow::Init()" << endl;
	#endif
	return m_hWnd;
}

/////////////////////////////////////////////////
void cWindow::HandleEvent(cEvent* e)
{
	//debug << "Begin cWindow::HandleEvent() " << endl;
	if(e->GetEventID() == WinHandleReq)
		e->SetParam(0, sVarient((int)m_hWnd));

	else if(e->GetEventID() == WinWidthReq)
	{
		//debug << m_width << endl;
		e->SetParam(0, sVarient((int)m_width));
	}

	else if(e->GetEventID() == WinHeightReq)
		e->SetParam(0, sVarient((int)m_height));

	else if(e->GetEventID() == WinFullScreenReq)
		e->SetParam(0, sVarient(m_fullscreen));
	
	//debug << "End cWindow::HandleEvent() " << endl;
}

/////////////////////////////////////////////////
LRESULT CALLBACK WinProc (HWND hwnd, UINT msg, 
						  WPARAM wParam, 
						  LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage (0);
		return 0;
	}

	return DefWindowProc (hwnd, msg, wParam, lParam);
}

// Get the horizontal and vertical screen sizes in pixel
void GetDesktopResolution(UINT& horizontal, UINT& vertical)
{
   RECT desktop;
   // Get a handle to the desktop window
   const HWND hDesktop = GetDesktopWindow();
   // Get the size of screen to the variable desktop
   GetWindowRect(hDesktop, &desktop);
   // The top left corner will have coordinates (0,0)
   // and the bottom right corner will have coordinates
   // (horizontal, vertical)
   horizontal = desktop.right;
   vertical = desktop.bottom;
}