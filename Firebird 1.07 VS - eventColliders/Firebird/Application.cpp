// Class cApplication method implementations

#include "Debug.h"
#include "Application.h"

/////////////////////////////////////////////////
const static int ESCAPE_KEY = 0x01;
const static int PAUSE_KEY = 0x19;

//game mode controls
const static int MODE_PLAY = 0;
const static int MODE_PAUSE = 1;
const static int MODE_MAIN_MENU = 2;
const static int MODE_IN_GAME_MENU = 3;

/////////////////////////////////////////////////
cApplication::cApplication()
: m_hInst(0), m_timer(NULL), m_window(NULL),
m_audio(NULL), m_input(NULL), m_curGameMode(NULL),
keyUpEvent("KEY_UP_EVENT"),
loadFontEvent("GRAPHICS_TEXT_FONT_LOAD_EVENT"),
fontUsed("Viner Hand ITC")
{
	// Set up check for memory leaks
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

	// Always check for memory leaks just before app exits
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;

	_CrtSetDbgFlag(tmpDbgFlag);

	g_eventMngr.Init();

	g_eventMngr.RegisterListener(keyUpEvent, this);

	g_eventMngr.PostEvent(new cEvent(loadFontEvent, fontUsed.GetStr(), 12));
}

/////////////////////////////////////////////////
cApplication::~cApplication()
{
	#ifdef MY_DEBUG
		debug << "\n\n~cApplication start" << endl;
	#endif

	if(m_timer) delete m_timer;
	m_timer = NULL;

	if(m_window)  delete m_window;
	m_window = NULL;

	if(m_audio)  delete m_audio;
	m_audio = NULL;

	if(m_input)  delete m_input;
	m_input = NULL;


	while(!m_gameModes.empty())
	{
		#ifdef MY_DEBUG
			debug << "DELETING game mode" << endl;
		#endif
		GameModeIter it = m_gameModes.begin();
		delete it->second;
		it->second = NULL;
		m_gameModes.erase (it);
	}

	cHashString::Shutdown();

	#ifdef MY_DEBUG
		debug << "~cApplication end" << endl;
	#endif
}

/////////////////////////////////////////////////
bool cApplication::Init(const string& appName, 
						HINSTANCE hInst)
{
	#ifdef MY_DEBUG
		debug << "Begin cApplication::Init()" << endl;
	#endif

	m_appName = appName;
	m_hInst = hInst;

	m_timer = new cTimer;
	m_timer->TogglePause();

	m_window = new cWindow;
	m_hWnd = m_window->Init(m_appName, m_hInst);

	m_audio = new cAudio;
	m_audio->Init();

	m_input = new cInput;
	m_input->Init(m_hWnd);

	m_gameModes[MODE_PLAY] = (iGame*)(new cGameModePlay);
	m_gameModes[MODE_PLAY]->Init("a");
	m_curGameMode = m_gameModes[MODE_PLAY];
	

	m_gameModes[MODE_PAUSE] = (iGame*)(new cGameModeMenu);
	m_gameModes[MODE_PAUSE]->Init("a");

	g_eventMngr.Trigger();
	m_timer->TogglePause();
	m_curGameMode->Activate();

	#ifdef MY_DEBUG
		debug << "End cApplication::Init()\n\n\n";
	#endif
	return true;
}

/////////////////////////////////////////////////
int cApplication::Run()
{ 
	//debug << "Begin cApplication::Run()" << endl;
	MSG msg;
	bool done = false;
	while (!done)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				done = true;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_timer->Tick();
			if (!m_audio->Update())
			{
				ErrorMsg("Audio Crashed! Exiting now");
				PostMessage(m_hWnd, WM_DESTROY, 0, 0);
			}
			m_input->PollInput();
			g_eventMngr.Trigger();
			m_curGameMode->Run();
		}
	}

	//debug << "End cApplication::Run()" << endl;
	return msg.wParam;
}

/////////////////////////////////////////////////
void cApplication::HandleEvent(cEvent* e)
{  
	//debug << "Begin cApplication::HandleEvent(" 
	//      << e->GetEventID().GetStr() << ")" 
	//      << endl;

	EventID eventID = e->GetEventID();

	if(eventID == keyUpEvent)
	{
		if(e->GetParam(0).m_asInt == ESCAPE_KEY) //ESCAPE Key
		{
			m_curGameMode->Deactivate();
			if (m_curGameMode == m_gameModes[MODE_PLAY])
			{
				m_curGameMode = m_gameModes[MODE_PAUSE];
				m_curGameMode->Activate();
			}
			else if (m_curGameMode == m_gameModes[MODE_PAUSE])
			{
				m_curGameMode = m_gameModes[MODE_PLAY];
				m_curGameMode->Activate();
			}
			//
		}
		else if(e->GetParam(0).m_asInt == PAUSE_KEY) //P Key
		{
			m_timer->TogglePause();
		}
		else if(e->GetParam(0).m_asInt == DIK_TAB)
			PostMessage(m_hWnd, WM_DESTROY, 0, 0);
	}

	//debug << "End cApplication::HandleEvent()" << endl;
}