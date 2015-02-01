#include "GameModePlay.h"

// Game.cpp
// Week 11, In-class Lab

// Needed header files 
#include "Debug.h"
#include "EventMngr.h"
#include "Input.h"
#include "Model.h"
#include "SkinnedMesh.h"
#include "Entity.h"
#include "Game.h"

///////////////////////////
//USED FOR TESTING
///////////////////////////
static int TEXT_SIZE = 40;
/////////////////////////////////////////////////
const static string LEVEL_TO_LOAD = "Level02.dat";
const char* LEVEL_NAME = "Cave";
const static int LOOP_SOUND_CONSTANTLY = -1;

/////////////////////////////////////////////////
cGameModePlay::cGameModePlay()
: m_entities(NULL), m_level(NULL), m_physics(NULL),
TimerUpdate("TIMER_UPDATE_EVENT"),
TimePause("TIMER_PAUSE_EVENT"),
GraphicLock("GRAPHICS_LOCK_EVENT"),
GraphicUnlock("GRAPHICS_UNLOCK_EVENT"),
GraphicClear("GRAPHICS_CLEAR_EVENT"),
GraphicPresent("GRAPHICS_PRESENT_EVENT"),
LoadTexture("GRAPHICS_TEXTURE_LOAD_EVENT"),
DrawTexture("GRAPHICS_TEXTURE_DRAW_EVENT"),
GraphicDisableZBuff("GRAPHICS_DISABLE_ZBUFFER"),
GraphicEnableZBuff("GRAPHICS_ENABLE_ZBUFFER"),
AudioLoadSound("AUDIO_LOAD_EVENT"),
AudioDeleteSound("AUDIO_DELETE_EVENT"),
AudioPlaySound("AUDIO_PLAY_EVENT"),
AudioIsPlayingSound("AUDIO_ISPLAYING_REQUEST"),
AudioStopSound("AUDIO_STOP_EVENT"),
AudioStopAllSound("AUDIO_STOPALL_EVENT"),
KeyDownEvent("KEY_DOWN_EVENT"),
WindowWidthReq("WINDOW_WIDTH_REQUEST"),
TEMP_SKY(".\\Textures\\Sky.png"),
//HUDBaseIMG(".\\Textures\\PlayModeHud.png"),
HUDBaseIMG(".\\Textures\\actionHud.png"),
BGSound(".\\Audio\\BackgroundSound.ogg"),

DrawTextEvent("GRAPHICS_TEXT_DRAW_EVENT"),
TextUsed("Viner Hand ITC")
{
	g_eventMngr.PostEvent( new cEvent(LoadTexture, TEMP_SKY.GetID()) );
	g_eventMngr.PostEvent( new cEvent(LoadTexture, HUDBaseIMG.GetID()) );

	//g_eventMngr.RegisterListener(TimerUpdate, this);
	//g_eventMngr.RegisterListener(KeyDownEvent, this);
}

/////////////////////////////////////////////////
cGameModePlay::~cGameModePlay() 
{
	#ifdef MY_DEBUG
		debug << "~cGameModePlay start" << endl;
	#endif

	if(m_entities != NULL)
		delete m_entities;
	m_entities = NULL;

	if(m_level != NULL)
		delete m_level;
	m_level = NULL;

	if(m_physics != NULL)
		delete m_physics;
	m_physics = NULL;

	#ifdef MY_DEBUG
		debug << "~cGameModePlay end" << endl;
	#endif
}

/////////////////////////////////////////////////
bool cGameModePlay::Init(const string& file)
{  
	#ifdef MY_DEBUG
		debug << "Begin cGameModePlay::Init()" << endl;
	#endif

	m_entities = new cEntityMngr;
	if(m_entities == NULL)
	{
		ErrorMsg("Entity manager creation");
		return false;
	}

	m_physics = new cPhysics;

	m_level = new cLevel;
	m_level->Init(LEVEL_TO_LOAD);

	g_eventMngr.PostEvent(new cEvent(
		AudioLoadSound, BGSound.GetStr()));
	g_eventMngr.PostEvent(new cEvent(
		AudioPlaySound, BGSound.GetID(), LOOP_SOUND_CONSTANTLY));

	#ifdef MY_DEBUG
		debug << "End cGameModePlay::Init()" << endl;
	#endif	
	return true;
}

/////////////////////////////////////////////////
bool cGameModePlay::Activate()
{
	#ifdef MY_DEBUG
		debug << "Begin cGameModePlay::Activate()" << endl;
	#endif

	g_eventMngr.SendEvent( &cEvent(GraphicEnableZBuff));
	g_eventMngr.RegisterListener(TimerUpdate, this);
	g_eventMngr.RegisterListener(KeyDownEvent, this);

	cEvent e(WindowWidthReq);
	g_eventMngr.SendEvent( &e);

	locNameRect.top = 10;
	locNameRect.bottom = locNameRect.top + TEXT_SIZE;
	locNameRect.right = e.GetParam(0).m_asInt;
	locNameRect.left = (long)(locNameRect.right - (locNameRect.right * 0.16f));

	#ifdef MY_DEBUG
		debug << "End cGameModePlay::Activate()" << endl;
	#endif
	return true;
}

/////////////////////////////////////////////////
bool cGameModePlay::Deactivate()
{
	#ifdef MY_DEBUG
		debug << "Begin cGameModePlay::Deactivate()" << endl;
	#endif

	g_eventMngr.RemoveListener(TimerUpdate, this);
	g_eventMngr.RemoveListener(KeyDownEvent, this);

	//g_eventMngr.Trigger();

	#ifdef MY_DEBUG
		debug << "End cGameModePlay::Deactivate()" << endl;
	#endif
	return true;
}

/////////////////////////////////////////////////
bool cGameModePlay::Run()
{
	//debug << "Begin cGameModePlay::Run()" << endl;

	m_physics->setDT(dt);
	m_entities->UpdateEntities(dt);

	g_eventMngr.Trigger();

	// Clear the render buffer
	g_eventMngr.PostEvent(new cEvent(GraphicClear));

	// Lock the graphics device
	g_eventMngr.PostEvent(new cEvent(GraphicLock));

	destPos = &sVector2d(0.0f, 0.0f);
	g_eventMngr.PostEvent( new cEvent(GraphicDisableZBuff));
	g_eventMngr.PostEvent(new cEvent(DrawTexture, 
									 TEMP_SKY.GetID(), 
									 (int)0xFFFFFFFF,
									 NULL,
									 NULL ));
	g_eventMngr.PostEvent( new cEvent(GraphicEnableZBuff));
	g_eventMngr.PostEvent(new cEvent(DrawTexture, 
									 HUDBaseIMG.GetID(), 
									 (int)0xFFFFFFFF,
									 NULL,
									 NULL ));

	format = DT_CENTER;
	color = 0xFF000000;
	g_eventMngr.PostEvent(new cEvent(DrawTextEvent, TextUsed.GetID(), 
								LEVEL_NAME, &locNameRect /*NULL*/, 
								&format/*NULL*/, &color/*NULL*/, 
								&TEXT_SIZE));

	m_entities->DrawEntities();   

	// Unlock graphics device
	g_eventMngr.PostEvent(new cEvent(GraphicUnlock));

	// Swap the front and back buffers
	g_eventMngr.PostEvent(new cEvent(GraphicPresent));

	cEvent e(AudioIsPlayingSound, BGSound.GetID());
	g_eventMngr.SendEvent(&e);
	
//	if (!e.GetParam(1).m_asBool)
//	{
//		debug << "BG music stopped, restarting! " << endl;
//		g_eventMngr.SendEvent(&cEvent(
//				AudioPlaySound, BGSound.GetID(), LOOP_SOUND_CONSTANTLY));
//	}

	//debug << "End cGame::Run()" << endl;
	return true;
}

/////////////////////////////////////////////////
void cGameModePlay::HandleEvent(cEvent* e)
{
	//debug << "Begin cGame::HandleEvent(" << e->GetEventID().GetStr() << ")" << endl;

	EventID eid = e->GetEventID();
	if(eid == TimerUpdate)
	{
		dt = e->GetParam(0).m_asFloat;
/*		#ifdef MY_DEBUG
			if (1/dt < 30)
				debug << "<30\t";
			if (1/dt < 20)
				debug << "<20\t";
			if (1/dt < 10)
				debug << "<10\t";
			debug << dt << endl;
		#endif*/
	}
/*	else if(eid == KeyDownEvent)
	{
		if(e->GetParam(0).m_asInt == 0x01) //ESCAPE Key
		{
			//debug << "ESCAPE PRESSED" << endl;
			g_eventMngr.SendEvent( &cEvent(TimePause) );
			//PostMessage(m_hWnd, WM_DESTROY, 0, 0);
		}
		else if(e->GetParam(0).m_asInt == PAUSE_KEY) //P Key
		{
			m_timer->TogglePause();
		}
	}*/

	//debug << "End cGame::HandleEvent(" << e->GetEventID().GetStr() << ")" << endl;
}