#ifndef __MENU_BUTTON_H__
#define __MENU_BUTTON_H__

#include <D3D9.h>
//#include <vector>

#include "EventMngr.h"

class cMenuButton : public iListener
{
private:
	//bool to hold if this one gets drawn this frame
	bool m_drawMe;
	int m_active;
	//RECT drawTo
	RECT m_drawTo;
	//texture for button
	GraphicID m_buttonTexture;

	//text data for the name on button
	GraphicID TextUsed;
	string m_buttonName;
	D3DCOLOR* m_textColors[2];
	/////
	//two of these for each button
	//[0] = inactive, [1] = active

	
	/////

	//list of buttons to display on click
	vector<cMenuButton*> m_showOnActivate;

	EventID DrawTexture;
	EventID DrawTextEvent;
	//EventID 


	cMenuButton(void);
public:
	
	cMenuButton(RECT* dest, const char* name, 
				DWORD* nameFormat, DWORD* colors, 
				GraphicID textureID);
	~cMenuButton(void);

	bool AddButtonToActivate(cMenuButton* add);

	bool Draw();
	void OnClick();

	void Activate();
	void Deactivate();

	virtual void HandleEvent(cEvent*);
};

#endif