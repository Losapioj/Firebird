#include "GameModeMenu.h"
#include "Debug.h"

/////////////////////////////////////////////////
cGameModeMenu::cGameModeMenu(void)
: //LoadSurface("GRAPHICS_SURFACE_LOAD_EVENT"),
SaveBackbuffer("GRAPHIC_SURFACE_SAVE_BACKBUFFER_EVENT"),
DrawSurface("GRAPHICS_SURFACE_DRAW_EVENT"),
LoadTexture("GRAPHICS_TEXTURE_LOAD_EVENT"),
DrawTexture("GRAPHICS_TEXTURE_DRAW_EVENT"),
GraphicDisableZBuff("GRAPHICS_DISABLE_ZBUFFER"),
GraphicEnableZBuff("GRAPHICS_ENABLE_ZBUFFER"),
WinWidthReq("WINDOW_WIDTH_REQUEST"),
WinHeightReq("WINDOW_HEIGHT_REQUEST"),
GraphicLock("GRAPHICS_LOCK_EVENT"),
GraphicUnlock("GRAPHICS_UNLOCK_EVENT"),
GraphicClear("GRAPHICS_CLEAR_EVENT"),
GraphicPresent("GRAPHICS_PRESENT_EVENT"),
bgTextureID(".\\Textures\\Hud2.png"),
ScreenCopySurface("BACKGROUND_COPY_ID")
//GraphicBackbufferImgReq("GRAPHIC_BACKBUFFER_IMG_REQUEST")
{
	//GraphicID bgImg(".\\Textures\\Terain_Bump_Bushes.png".jpg");
	
	cEvent* e1 = new cEvent( LoadTexture, bgTextureID.GetID() );
	//cEvent* e2 = new cEvent( LoadTexture, bgImg.GetID() );

	g_eventMngr.PostEvent(e1);
	//g_eventMngr.PostEvent(e2);

}

/////////////////////////////////////////////////
cGameModeMenu::~cGameModeMenu(void)
{
	#ifdef MY_DEBUG
		debug << "~cGameModeMenu start" << endl;
	#endif

	while(!m_buttonList.empty())
	{
		ButtonIter it = m_buttonList.begin();
		delete it->second;
		it->second = NULL;
		m_buttonList.erase (it);
	}

	#ifdef MY_DEBUG
		debug << "~cGameModeMenu End" << endl;
	#endif
}

/////////////////////////////////////////////////
bool cGameModeMenu::Init(const string& file)
{
	#ifdef MY_DEBUG
	debug << "Begin cGameModeMenu::Init()" << endl;
	#endif
	//ifstream fin(file.c_str());
	//if(!fin)  return false;

	//string str;
	//fin >> str;

	#ifdef MY_DEBUG
		debug << "End cGameModeMenu::Init()" << endl;
	#endif
	return true;
}

/////////////////////////////////////////////////
bool cGameModeMenu::Activate()
{
#ifdef MY_DEBUG
	debug << "Begin cGameModeMenu::Activate()" << endl;
#endif

	g_eventMngr.SendEvent( &cEvent(GraphicDisableZBuff));

	g_eventMngr.SendEvent( &cEvent(SaveBackbuffer, ScreenCopySurface.GetID()));
	

#ifdef MY_DEBUG
	debug << "End cGameModeMenu::Activate()" << endl;
#endif
	return true;
}

/////////////////////////////////////////////////
bool cGameModeMenu::Deactivate()
{
#ifdef MY_DEBUG
	debug << "Begin cGameModeMenu::Deactivate()" << endl;
#endif


#ifdef MY_DEBUG
	debug << "End cGameModeMenu::Deactivate()" << endl;
#endif
	return true;
}

/////////////////////////////////////////////////
bool cGameModeMenu::Run()
{
	//debug << "Begin cGameModeMenu::Run()" << endl;

	//get window height and width
	cEvent heightEvent(WinHeightReq);
	g_eventMngr.SendEvent(&heightEvent);

	cEvent widthEvent(WinWidthReq);
	g_eventMngr.SendEvent(&widthEvent);
	
	//calculate matrix for texture to draw
	D3DXMatrixIdentity(&m_worldMatrix);

	float destX = ((widthEvent.GetParam(0).m_asInt / 2.0f) - 
				   (heightEvent.GetParam(0).m_asInt / 2.0f));
	float destY = 0.0f;
	float scaleX = 0.5f;
	float scaleY = 0.5f;


	destPos.m_x = ((widthEvent.GetParam(0).m_asInt / 2.0f) - 
				   (heightEvent.GetParam(0).m_asInt / 2.0f));
	destPos.m_y = 0.0f;
	scaleVec.m_x = 1.0f;
	scaleVec.m_y = 1.0f;


	g_eventMngr.Trigger();

	// Clear the render buffer
	g_eventMngr.PostEvent(new cEvent(GraphicClear));

	// Lock the graphics device
	g_eventMngr.PostEvent(new cEvent(GraphicLock));

//	sVector2d* vecPTR;
//	VOID* temp = (VOID*) &destPos;
//	vecPTR = (sVector2d*) temp;

	//debug << "GameModeMenu::Run" << endl;
	//debug << "\t" << "vector position in memory before event: " << &destPos << endl;
	//debug << "\t" << "Vector x and y: " << destPos.m_x << " " << destPos.m_y << endl;
//	g_eventMngr.PostEvent(new cEvent(DrawTexture, 
//									 bgImg.GetID(), 
//									 (int)0xFFFFFFFF,
//									 NULL,
//									 NULL ));
	g_eventMngr.PostEvent(new cEvent(DrawSurface, ScreenCopySurface.GetID(), NULL, NULL));
	g_eventMngr.PostEvent(new cEvent(DrawTexture, 
									 bgTextureID.GetID(), 
									 (int)0xFFFFFFFF,
									 /*&destPos*/NULL,
									 /*&scaleVec*/NULL ));

	ButtonIter it;
	for (it = m_buttonList.begin(); it != m_buttonList.end(); it++)
	{
		//each button posts draw events for themselves
		it->second->Draw();
	}

	// Unlock graphics device
	g_eventMngr.PostEvent(new cEvent(GraphicUnlock));

	// Swap the front and back buffers
	g_eventMngr.PostEvent(new cEvent(GraphicPresent));
	
//	debug << "cGameModeMenu::Run events posted" << endl;
	return true;
}

/////////////////////////////////////////////////
void cGameModeMenu::HandleEvent(cEvent* e)
{
}