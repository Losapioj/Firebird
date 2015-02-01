#ifndef __APPLICATION_H__
#define __APPLICATION_H__

/////////////////////////////////////////////////
#include <windows.h>
#include <string>

using std::string;

#include "EventMngr.h"
#include "Timer.h"
#include "Input.h"
#include "Audio.h"
#include "Window.h"
#include "GameModes.h"

/////////////////////////////////////////////////
class cApplication : public iListener
{
protected:
	string m_appName;
	HINSTANCE m_hInst;
	HWND m_hWnd;

	cTimer* m_timer;
	cWindow* m_window;
	cAudio* m_audio;
	cInput* m_input;

/*	typedef vector<cGame*> GameModeList;
	typedef GameModeList::iterator GameModeIter;

	GameModeList m_gameModes;*/

	typedef map<int, iGame*> GameModeList;
	typedef GameModeList::iterator GameModeIter;

	GameModeList m_gameModes;
	iGame* m_curGameMode;
//	cGame* m_game;

	EventID keyUpEvent;
	GraphicID loadFontEvent;

	GraphicID fontUsed;

public:
	cApplication();
	virtual ~cApplication();
	bool Init(const string&, HINSTANCE);
	int Run();
	virtual void HandleEvent(cEvent*);
};

/////////////////////////////////////////////////
#endif // __APPLICATION_H__