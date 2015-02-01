#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <d3d9.h>
#include <d3dx9.h>

#include <fstream>
#include <map>
#include <string>
using std::ofstream;
using std::ifstream;
using std::endl;
using std::map;
using std::string;

#include "HashString.h"
#include "Camera.h"
#include "Model.h"
#define D3D_DEBUG_INFO


/////////////////////////////////////////////////
// Represents all graphical functionality
class cGraphics : public iListener
{
protected:
	typedef map<GraphicID, cModel*> MDL_LIST;
	typedef MDL_LIST::iterator MDL_ITER;

	//used for model textures and 2d GUI textures
	typedef map<GraphicID, LPDIRECT3DTEXTURE9> TXR_LIST;
	typedef TXR_LIST::iterator TXR_ITER;

	//used to display text
	typedef map <GraphicID, LPD3DXFONT> FONT_LIST;
	typedef FONT_LIST::iterator FONT_ITER;

	//NOT CURRENTLY USED
	typedef map<GraphicID, LPDIRECT3DSURFACE9> SRF_LIST;
	typedef SRF_LIST::iterator SRF_ITER;

	cCamera* m_camera;
	LPDIRECT3D9 m_d3d;
	LPDIRECT3DDEVICE9 m_dev;
	LPDIRECT3DSURFACE9 m_backBuffer;

	FONT_LIST m_fonts;
	MDL_LIST m_models;
	TXR_LIST m_textures;
	SRF_LIST m_surfaces;

	//used for 2D textures
	LPD3DXSPRITE m_sprite;

	//EventID AppNameReq;
	//EventID AppHandleReq;

	EventID LockEvent;
	EventID UnlockEvent;
	EventID ClearEvent;
	EventID PresentEvent;

	EventID LoadModelEvent;
	EventID LoadTextureEvent;
	EventID AttachTextureEvent;
	EventID LoadSurfaceEvent;
	EventID SaveSurfaceEvent;
	EventID SaveBackbufferEvent;
	EventID LoadTextFontEvent;

	EventID RenderModelEvent;
	EventID DrawTextureEvent;
	EventID DrawSurfaceEvent;
	EventID DrawTextEvent;

	EventID DisableZBuffEvent;
	EventID EnableZBuffEvent;

	//used to get data
	EventID WindowWidthReq;
	EventID WindowHeightReq;

	

	/*GraphicID StaticModelType;
	GraphicID DynamicModelType;*/

	//HELPER FUNCTIONS
	//called from DrawTexture to resize font if requested
	bool ResizeFont(const GraphicID& id, int newSize);

public:
	cGraphics();
	~cGraphics();
	bool Init(HWND, int, int, bool);
	virtual void HandleEvent(cEvent*);

	bool Lock();
	bool Unlock();
	void ClearScreen(char, char, char);
	void Display();

	bool LoadTextures(const char*);
	bool LoadModel(const char* type,
		const char* file);

	bool RenderModel(const GraphicID&, D3DXMATRIX*);

	bool LoadTexture(const GraphicID&);
	bool AttachTexture(const GraphicID&, int);
	bool LoadSurface(const GraphicID& id, RECT* srcRect,
					 RECT* destRect, DWORD colorKey);
	bool SaveSurface(const GraphicID& id, LPDIRECT3DSURFACE9 srcSurface);

	// calls savesurface with backbuffer information
	//bool SaveBackbuffer();

	bool DrawSurface(const GraphicID& id, RECT* src, RECT* dest);

	bool DrawTexture (const GraphicID& id, int colorMod, 
					  sVector2d* destPos, sVector2d* scaleVec);

	bool LoadTextFont(GraphicID id, int fontHeight);
	bool DrawMyText(const GraphicID& fontID, 
				    const string& s, RECT* rc, DWORD* format, 
				    DWORD* color, int* newFontSize);
};

/////////////////////////////////////////////////
#endif // __GRAPHICS_H__