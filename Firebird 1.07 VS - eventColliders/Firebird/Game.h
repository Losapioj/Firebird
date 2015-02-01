
#ifndef __GAME_H__
#define __GAME_H__

/////////////////////////////////////////////////
// Needed header files 
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "EventMngr.h"
#include "EntityMngr.h"
#include "Level.h"
#include "Physics.h"

/////////////////////////////////////////////////
class iGame //: public iListener
{
protected:
	

public:
	virtual ~iGame() {}

	virtual bool Activate() = 0;
	virtual bool Deactivate() = 0;

	virtual bool Init(const string& file) = 0;
	virtual bool Run() = 0;
	virtual void HandleEvent(cEvent*) = 0;
};

/////////////////////////////////////////////////
#endif // __GAME_H__