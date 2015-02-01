#ifndef __GAME_MODE_MENU_H__
#define __GAME_MODE_MENU_H__

/////////////////////////////////////////////////
#include "Game.h"
#include "MenuButton.h"

/////////////////////////////////////////////////
class cGameModeMenu : public iGame, iListener
{
private:
	typedef map<int, cMenuButton*> ButtonList;
	typedef ButtonList::iterator ButtonIter;

	ButtonList m_buttonList;

	//ID to save backbuffer img to, to draw in background
	//GraphicID m_BackbufferImg;

	EventID GraphicLock;
	EventID GraphicUnlock;
	EventID GraphicClear;
	EventID GraphicPresent;

	EventID DrawSurface;
	EventID DrawTexture;
	//EventID LoadSurface;
	EventID LoadTexture;
	EventID SaveBackbuffer;
	EventID GraphicDisableZBuff;
	EventID GraphicEnableZBuff;

	EventID WinWidthReq;
	EventID WinHeightReq;

//	EventID GraphicBackbufferImgReq;

	//map of textures for menu
	GraphicID bgTextureID;
	GraphicID ScreenCopySurface;

	sVector2d destPos;
	sVector2d scaleVec;

	//used to draw surfaces
	//RECT m_destRect;

	//sent to graphics to draw sprites
	D3DXMATRIX m_worldMatrix;
	D3DXMATRIX m_transMatrix;
    D3DXMATRIX m_scaleMatrix;
    

public:
	cGameModeMenu(void);
	~cGameModeMenu(void);

	bool Activate();
	bool Deactivate();

	bool Init(const string& file);
	bool Run();
	void HandleEvent(cEvent*);
};

/////////////////////////////////////////////////
#endif