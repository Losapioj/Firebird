#ifndef __GAME_MODE_PLAY_H__
#define __GAME_MODE_PLAY_H__

/////////////////////////////////////////////////
#include "Game.h"

/////////////////////////////////////////////////
class cGameModePlay : public iGame, iListener
{
private:
	cEntityMngr* m_entities;
	cLevel* m_level;
	cPhysics* m_physics;
	float dt;

	//used to draw text
	RECT locNameRect;
	int format;
	int color;

	EventID TimerUpdate;
	EventID TimePause;
	EventID KeyDownEvent;

	EventID GraphicLock;
	EventID GraphicUnlock;
	EventID GraphicClear;
	EventID GraphicPresent;

	EventID DrawTextEvent;
	EventID LoadTexture;
	EventID DrawTexture;
	EventID GraphicDisableZBuff;
	EventID GraphicEnableZBuff;
	EventID WindowWidthReq;

	EventID AudioLoadSound;
	EventID AudioDeleteSound;
	EventID AudioPlaySound;
	EventID AudioIsPlayingSound;
	EventID AudioStopSound;
	EventID AudioStopAllSound;

	//vars to draw textures
	GraphicID TEMP_SKY;
	sVector2d* destPos;
	//HUD ID's
	GraphicID HUDBaseIMG;
	//text ID
	GraphicID TextUsed;

	AudioID BGSound;

public:
	cGameModePlay(void);
	~cGameModePlay(void);

	bool Activate();
	bool Deactivate();

	virtual bool Init(const string& file);
	virtual bool Run();
	virtual void HandleEvent(cEvent*);
};

/////////////////////////////////////////////////
#endif