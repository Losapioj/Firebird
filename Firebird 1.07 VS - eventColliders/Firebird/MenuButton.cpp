#include "MenuButton.h"

static int TEXT_SIZE = 30;
/////////////////////////////////////////////////
cMenuButton::cMenuButton(void)
{
}

/////////////////////////////////////////////////
cMenuButton::cMenuButton(RECT* dest, const char* name,
						 DWORD* nameFormat, DWORD* colors, 
						 GraphicID textureID)
: DrawTexture("GRAPHICS_TEXTURE_DRAW_EVENT"),
DrawTextEvent("GRAPHICS_TEXT_DRAW_EVENT"),
TextUsed("Viner Hand ITC")
{
	m_drawMe = false;
	m_active = 0;
	m_drawTo.left = dest->left;
	m_drawTo.top = dest->top;
	m_drawTo.right = dest->right;
	m_drawTo.bottom = dest->bottom;
	m_buttonName = name;
	m_textColors[0] = &(colors[0]);
	m_textColors[1] = &(colors[1]);
	m_buttonTexture = textureID;
}

/////////////////////////////////////////////////
cMenuButton::~cMenuButton(void)
{
	//buttons themselves deleted by menu holding them, just empty pointers
	while (!m_showOnActivate.empty())
	{
		m_showOnActivate.erase(m_showOnActivate.begin());
	}
}

/////////////////////////////////////////////////
bool cMenuButton::AddButtonToActivate(cMenuButton* add)
{
	m_showOnActivate.push_back(add);
	return true;
}

/////////////////////////////////////////////////
bool cMenuButton::Draw()
{
	if (m_drawMe)
	{
		g_eventMngr.PostEvent(new cEvent(DrawTexture, 
									 m_buttonTexture.GetID(), 
									 (int)0xFFFFFFFF,
									 &m_drawTo,
									 /*&scaleVec*/NULL ));
		g_eventMngr.PostEvent(new cEvent(DrawTextEvent, TextUsed.GetID(), 
										m_buttonName.c_str(), &m_drawTo, 
										DT_CENTER, m_textColors[m_active], 
										&TEXT_SIZE ));
	}
	return true;
}

/////////////////////////////////////////////////
void cMenuButton::OnClick()
{
}
/////////////////////////////////////////////////
void cMenuButton::HandleEvent(cEvent*)
{
}