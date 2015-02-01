
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <sstream>

#include "Debug.h"
#include "EventMngr.h"
#include "Model.h"
#include "SkinnedMesh.h"
#include "Graphics.h"

/////////////////////////////////////////////////
// Needed library files
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

/////////////////////////////////////////////////
const static sVector3d CLS_COLOR (252, 154, 216);
const static int TRANSPARENCY_COLOR_KEY = 0xffff00ff;
const static DWORD TRANSPARENCY_IGNORE_THRESHOLD = (DWORD)0x000000E0;

const static float FRUSTUM_NEAR = 1.0f;
const static float FRUSTUM_FAR = 500.0f;

const static float FOV_IN_DEG = 110.0f;

/////////////////////////////////////////////////
cGraphics::cGraphics()
: m_d3d (NULL), m_dev(NULL), m_camera(NULL), m_sprite(NULL),
LockEvent("GRAPHICS_LOCK_EVENT"),
UnlockEvent("GRAPHICS_UNLOCK_EVENT"),
ClearEvent("GRAPHICS_CLEAR_EVENT"),
PresentEvent("GRAPHICS_PRESENT_EVENT"),

LoadModelEvent("GRAPHICS_MODEL_LOAD_EVENT"),
RenderModelEvent("GRAPHICS_MODEL_RENDER_EVENT"),

LoadSurfaceEvent("GRAPHICS_SURFACE_LOAD_EVENT"),
SaveSurfaceEvent("GRAPHIC_SURFACE_SAVE_EVENT"),
SaveBackbufferEvent("GRAPHIC_SURFACE_SAVE_BACKBUFFER_EVENT"),
DrawSurfaceEvent("GRAPHICS_SURFACE_DRAW_EVENT"),

LoadTextureEvent("GRAPHICS_TEXTURE_LOAD_EVENT"),
AttachTextureEvent("GRAPHICS_TEXTURE_ATTACH_EVENT"),
DrawTextureEvent("GRAPHICS_TEXTURE_DRAW_EVENT"),

DisableZBuffEvent("GRAPHICS_DISABLE_ZBUFFER"),
EnableZBuffEvent("GRAPHICS_ENABLE_ZBUFFER"),
LoadTextFontEvent("GRAPHICS_TEXT_FONT_LOAD_EVENT"),
DrawTextEvent("GRAPHICS_TEXT_DRAW_EVENT"),


WindowWidthReq("WINDOW_WIDTH_REQUEST"),
WindowHeightReq("WINDOW_HEIGHT_REQUEST")
{
	g_eventMngr.RegisterListener(LockEvent, this);
	g_eventMngr.RegisterListener(UnlockEvent, this);
	g_eventMngr.RegisterListener(ClearEvent, this);
	g_eventMngr.RegisterListener(PresentEvent, this);
	g_eventMngr.RegisterListener(LoadModelEvent, this);
	g_eventMngr.RegisterListener(RenderModelEvent, this);
	g_eventMngr.RegisterListener(LoadTextureEvent, this);
	g_eventMngr.RegisterListener(AttachTextureEvent, this);
	g_eventMngr.RegisterListener(DrawTextureEvent, this);
	g_eventMngr.RegisterListener(LoadSurfaceEvent, this);
	g_eventMngr.RegisterListener(DrawSurfaceEvent, this);
	g_eventMngr.RegisterListener(LoadTextFontEvent, this);
	g_eventMngr.RegisterListener(DrawTextEvent, this);
	g_eventMngr.RegisterListener(DisableZBuffEvent, this);
	g_eventMngr.RegisterListener(EnableZBuffEvent, this);

	g_eventMngr.RegisterListener(SaveSurfaceEvent, this);
	g_eventMngr.RegisterListener(SaveBackbufferEvent, this);
}

/////////////////////////////////////////////////
cGraphics::~cGraphics()
{
#ifdef MY_DEBUG
	debug << "~cGraphics start" << endl;
#endif

	while(!m_models.empty())
	{
#ifdef MY_DEBUG
		debug << "DELETING model" << endl;
#endif
		MDL_ITER it = m_models.begin();
		if(it->second != NULL)
			delete it->second;

		m_models.erase(it);
	}

	while(!m_textures.empty())
	{
#ifdef MY_DEBUG
		debug << "DELETING texture" << endl;
#endif
		TXR_ITER it = m_textures.begin();
		if(it->second != NULL)
			it->second->Release();

		m_textures.erase(it);
	}

	while(!m_fonts.empty())
	{
#ifdef MY_DEBUG
		debug << "DELETING font" << endl;
#endif
		FONT_ITER it = m_fonts.begin();
		if(it->second != NULL)
			it->second->Release();
		m_fonts.erase(it);
	}

	while(!m_surfaces.empty())
	{
#ifdef MY_DEBUG
		debug << "DELETING surface" << endl;
#endif
		SRF_ITER it = m_surfaces.begin();
		if(it->second != NULL)
			it->second->Release();
		m_surfaces.erase(it);
	}

	if(m_camera)  
	{
#ifdef MY_DEBUG
		debug << "DELETING graphics::m_camera" << endl;
#endif
		delete m_camera;
	}

	m_camera = NULL;

	if(m_d3d) 
	{
#ifdef MY_DEBUG
		debug << "DELETING graphics::m_d3d" << endl;
#endif
		m_d3d->Release();
	}
	m_d3d = NULL;

	if(m_dev)
	{
#ifdef MY_DEBUG
		debug << "DELETING graphics::m_dev" << endl;
#endif
		m_dev->Release();
	}
	m_dev = NULL;

	if(m_sprite)
	{
#ifdef MY_DEBUG
		debug << "DELETING graphics::m_sprite" << endl;
#endif
		m_sprite->Release();
	}
	m_sprite = NULL;

	if(m_backBuffer)
	{
#ifdef MY_DEBUG
		debug << "DELETING graphics::m_backBuffer" << endl;
#endif
		m_backBuffer->Release();
	}
	m_backBuffer = NULL;

#ifdef MY_DEBUG
	debug << "~cGraphics end" << endl;
#endif
}

/////////////////////////////////////////////////
bool cGraphics::Init(HWND hWnd, int width, 
					 int height, bool fullscreen)
{
#ifdef MY_DEBUG
	debug << "Begin cGraphics::Init()" << endl;
#endif

	// Create and verify Direct3D interface object
	m_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if(m_d3d == NULL)
	{
		ErrorMsg("ERROR: Direct3D Initialization ");
		return false;
	}

	// Create & load Presentation Parameter struct
	D3DPRESENT_PARAMETERS pp;
	ZeroMemory(&pp, sizeof(pp));

	// Assign attributes that don't 
	// depend on window state
	pp.Windowed = !fullscreen;
	pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pp.EnableAutoDepthStencil = true;
	pp.AutoDepthStencilFormat = D3DFMT_D16;
	pp.PresentationInterval =
		D3DPRESENT_INTERVAL_IMMEDIATE;

	if (pp.Windowed)
		// if windowed mode, use desktop pixel format
		pp.BackBufferFormat = D3DFMT_UNKNOWN;

	else
	{
		// if full screen mode...
		pp.BackBufferFormat = D3DFMT_X8R8G8B8;
		pp.BackBufferCount = 1;
		pp.BackBufferWidth = width;
		pp.BackBufferHeight = height;
		pp.hDeviceWindow = hWnd;
	}

	// Check graphics chip's ability to handle 
	// vertex processing
	D3DCAPS9 caps;
	int vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	m_d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL, &caps);
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else 
		ErrorMsg("Software Vertex Processing ONLY");




	// Create & verify graphic device interface object
	HRESULT hr = m_d3d->CreateDevice(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, 
		vp, &pp, &m_dev);
	if(hr != D3D_OK)
	{
		ErrorMsg("Graphics Initialization");
		return false;
	}

	cModel::SetDevice(m_dev);

	// Create and set projection matrix
	D3DXMATRIX projection;
	D3DXMatrixPerspectiveFovLH(
		&projection, DegToRad(FOV_IN_DEG)/2, //function takes FOV per half screen, divide by two
		(float)width / (float)height, 
		FRUSTUM_NEAR, FRUSTUM_FAR);
	m_dev->SetTransform(D3DTS_PROJECTION, &projection);

	// Establish light source
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = D3DXCOLOR(0.8862745f, 0.8823529f, 0.7019608f, 1.0f);
	light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.Specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	light.Direction = D3DXVECTOR3(-0.5f, -1.0f, 1.0f);
	D3DXVec3Normalize((D3DXVECTOR3*)(&light.Direction), 
		(D3DXVECTOR3*)(&light.Direction));

	//test and activate alpha testing to ignore pixels with alpha
	//should prevent invisible textures from being drawn to zbuffer
	if (caps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL)
	{
		m_dev->SetRenderState(D3DRS_ALPHAREF, TRANSPARENCY_IGNORE_THRESHOLD);
		m_dev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
		m_dev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	}
	else
	{
		ErrorMsg("CAN NOT DO ALPHA TESTS, reverting to alphablend");
	}
	m_dev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	m_dev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_dev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);


	m_dev->SetRenderState(D3DRS_LIGHTING, true);
	m_dev->SetRenderState(D3DRS_SPECULARENABLE, true);
	m_dev->SetLight(0, &light);
	m_dev->LightEnable(0, true);

	m_dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_dev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_dev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	//for sprites
	D3DXCreateSprite(m_dev, &m_sprite);

	//for surfaces
	m_dev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_backBuffer);

	m_camera = new cCamera;
	m_camera->Init();

#ifdef MY_DEBUG
	debug << "End cGraphics::Init()" << endl;
#endif
	return true;
}

/////////////////////////////////////////////////
void cGraphics::HandleEvent(cEvent* e)
{
	//debug << "Begin cGraphics::HandleEvent()" << endl;
	EventID id = e->GetEventID();
	if(id == LockEvent)
		Lock();

	else if(id == UnlockEvent)
		Unlock();

	else if(id == ClearEvent)
		ClearScreen((char)CLS_COLOR.m_x, (char)CLS_COLOR.m_y, (char)CLS_COLOR.m_z);

	else if(id == PresentEvent)
		Display();

	else if(id == LoadModelEvent)
		LoadModel(e->GetParam(0).m_asString, 
		e->GetParam(1).m_asString);

	else if(id == RenderModelEvent)
	{
		GraphicID* id = (GraphicID*)(e->GetParam(0).m_asPointer);
		RenderModel(*id,
			(D3DXMATRIX*)e->GetParam(1).m_asPointer);
	}

	else if(id == LoadTextureEvent)
	{
		GraphicID texture;
		texture.SetID(e->GetParam(0).m_asInt);
		LoadTexture(texture);
	}

	else if(id == AttachTextureEvent)
	{
		GraphicID texture;
		texture.SetID(e->GetParam(0).m_asInt);
		AttachTexture(texture, e->GetParam(1).m_asInt);
	}  

	else if(id == DrawTextureEvent)
	{
		GraphicID texture;
		texture.SetID(e->GetParam(0).m_asInt);
		int colorMod = e->GetParam(1).m_asInt;
		sVector2d* destPos = (sVector2d*)(e->GetParam(2).m_asPointer);
		sVector2d* centerPos = (sVector2d*)(e->GetParam(3).m_asPointer);
		

		//debug << "Graphics::HandleEvent" << endl;
		//debug << "\t" << "vector position in memory after event: " << destPos << endl;
		//debug << "\t" << "Vector x and y: " <<  destPos->m_x << " " << destPos->m_y << endl;

		DrawTexture(texture, colorMod, destPos, centerPos);
	}

	else if (id == LoadTextFontEvent)
	{
		GraphicID font;
		font.SetStr(e->GetParam(0).m_asString);
		int fontHeight = e->GetParam(1).m_asInt;
		LoadTextFont(font, fontHeight);
	}

	else if (id == DrawTextEvent)
	{
		//debug << "Graphics::HandleEvent DrawTextEvent Start" << endl;
		GraphicID font;
		font.SetID(e->GetParam(0).m_asInt);
		const string toWrite = e->GetParam(1).m_asString;
		RECT* rc = (RECT*)e->GetParam(2).m_asPointer;
		DWORD* format = (DWORD*)e->GetParam(3).m_asPointer;
		DWORD* color = (DWORD*)e->GetParam(4).m_asPointer;
		int* fontSize = (int*)e->GetParam(5).m_asPointer;

		//debug << "Graphics::HandleEvent DrawTextEvent calling F(x)" << endl;
		DrawMyText(font, toWrite, rc, format, color, fontSize);
		//debug << "Graphics::HandleEvent DrawTextEvent End" << endl;
	}

	else if(id == LoadSurfaceEvent)
	{
		GraphicID texture;
		texture.SetStr(e->GetParam(0).m_asString);
		RECT* src( (RECT*)(e->GetParam(1).m_asPointer) );
		RECT* dest( (RECT*)(e->GetParam(2).m_asPointer) );
		DWORD colorKey = (DWORD)(e->GetParam(3).m_asInt);
		LoadSurface(texture, src, dest, colorKey );
	} 

	else if(id == DrawSurfaceEvent)
	{
		GraphicID texture;
		texture.SetID(e->GetParam(0).m_asInt);
		RECT* src( (RECT*)(e->GetParam(1).m_asPointer) );
		RECT* dest( (RECT*)(e->GetParam(2).m_asPointer) );
		DrawSurface(texture, src, dest);
	} 
	else if(id == DisableZBuffEvent)
	{
		m_dev->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
		m_dev->SetRenderState(D3DRS_ZWRITEENABLE, false);
	}
	else if(id == EnableZBuffEvent)
	{
		m_dev->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		m_dev->SetRenderState(D3DRS_ZWRITEENABLE, true);
	}
	else if(id == SaveSurfaceEvent)
	{
		GraphicID id;
		id.SetID(e->GetParam(0).m_asInt);
		LPDIRECT3DSURFACE9 surface = ((LPDIRECT3DSURFACE9)e->GetParam(1).m_asPointer);
		SaveSurface(id, surface);
	}
	else if(id == SaveBackbufferEvent)
	{
		GraphicID id;
		id.SetID(e->GetParam(0).m_asInt);
		SaveSurface(id, m_backBuffer);
	}

	//debug << "End cGraphics::HandleEvent()" << endl;
}

/////////////////////////////////////////////////
bool cGraphics::Lock()
{ 
	//debug << "Begin cGraphics::Lock()" << endl;
	m_dev->SetTransform(D3DTS_VIEW, 
		&(m_camera->CalcViewMatrix()));

	if (m_dev->BeginScene() != D3D_OK)
		return  false;

	//debug << "End cGraphics::Lock()" << endl;
	return true;
}

/////////////////////////////////////////////////
bool cGraphics::Unlock()
{
	//debug << "Begin cGraphics::Unlock()" << endl;
	if(m_dev->EndScene() != D3D_OK)
		return false;

	//debug << "End cGraphics::Unlock()" << endl;
	return true;
}

/////////////////////////////////////////////////
void cGraphics::ClearScreen(char r, char g, 
							char b)
{
	//debug << "Begin cGraphics::ClearScreen()" << endl;
	m_dev->Clear (0, NULL, 
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
		D3DCOLOR_XRGB(r, g, b), 1.0f, 0);
	//debug << "End cGraphics::ClearScreen()" << endl;
}

/////////////////////////////////////////////////
void cGraphics::Display()
{
	m_dev->Present (NULL, NULL, NULL, NULL);
}

/////////////////////////////////////////////////
bool cGraphics::LoadModel(const char* type,
						  const char* file)
{
	//debug << "Begin cGraphics::LoadModel(" << type << ", " << file << ")" << endl;
	MDL_ITER it = m_models.find(file);
	if(it != m_models.end())
		return true;

	cModel* model = NULL;
	if(strcmp(type, "static") == 0)
		model = new cXModel;

	else if (strcmp(type, "skinned") == 0)
		model = new cSkinnedMesh;

	else
	{
		ErrorMsg("Unknown model type");
		return false;
	}

	if(!model->Init(file))
	{
		stringstream Msg;
		Msg << "Model not loaded: " << file;
		ErrorMsg(Msg.str());
		return false;
	}
	GraphicID File(file);
	m_models[File] = model;

	//debug << "End cGraphics::LoadModel()" << endl;
	return true;
}

/////////////////////////////////////////////////
bool cGraphics::RenderModel(const GraphicID& id, 
							D3DXMATRIX* world)
{
	//debug << "Begin cGraphics::RenderModel(" << id.GetStr() << ")" << endl;
	MDL_ITER it = m_models.find(id);
	if(it == m_models.end())
	{
		string idStr = id.GetStr();
		string errorMsg = "Render failed: " + idStr;
		ErrorMsg(errorMsg);
		return false;
	}

	bool flag = m_models[id]->Draw(world);

	//debug << "End cGraphics::RenderModel()" << endl;
	return flag;
}

/////////////////////////////////////////////////
bool cGraphics::LoadTexture(const GraphicID& id)
{
	//debug << "Begin cGraphics::LoadTexture(" << id.GetStr() << ")\n";

	TXR_ITER it = m_textures.find(id);
	if(it != m_textures.end())
	{
		//debug << "Texture Loading: Texture already exists" << endl;
		return true;
	}

	//debug << id.GetStr() << " loading..." << endl;
	D3DXIMAGE_INFO imageInfo;
	HRESULT hr = D3DXGetImageInfoFromFile (id.GetStr(), &imageInfo);
	if(hr != D3D_OK)
	{
		FileNotFoundMsg(id.GetStr());
		return false;
	}

	IDirect3DTexture9* temp = NULL;
//	HRESULT hr = D3DXCreateTextureFromFile(
//		m_dev, id.GetStr(), &temp);
	hr = D3DXCreateTextureFromFileEx(
			m_dev, id.GetStr(),imageInfo.Width, 
			imageInfo.Height, 1, D3DPOOL_DEFAULT, 
			D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, 
			D3DX_DEFAULT, D3DX_DEFAULT, TRANSPARENCY_COLOR_KEY,
			&imageInfo, NULL, &temp);
	if(hr != D3D_OK)
	{
		FileNotFoundMsg(id.GetStr());
		return false;
	}

	m_textures[id] = temp;

	//debug << "End cGraphics::LoadTexture()" << endl;
	return true;
}

/////////////////////////////////////////////////
bool cGraphics::AttachTexture(const GraphicID& id, int stage)
{
#ifdef MY_DEBUG
	//debug << "Begin cGraphics::AttachTexture()" << endl; //id.GetStr() << endl; ")\n";
#endif
	TXR_ITER it = m_textures.find(id);
	if(it == m_textures.end())
		return false;

	HRESULT hr = m_dev->SetTexture(stage, m_textures[id]);
#ifdef MY_PROFILE
	if (hr != D3D_OK)
	{
		ErrorMsg("cGraphics::AttachTexture SetTexture Failed");
	}
#endif
#ifdef MY_DEBUG
	//debug << "End cGraphics::AttachTexture()\n";
#endif
	return true;
}

/////////////////////////////////////////////////
bool cGraphics::DrawTexture (const GraphicID& id, int colorMod, 
							 sVector2d* destPos = NULL, sVector2d* scaleVec = NULL)  
{
	//if scale is NULL, make it full screen
	D3DXMATRIX scale;
	if (scaleVec == NULL)
	{
		//get width and height of img, to compare to width and height of window
		D3DSURFACE_DESC surfaceDesc;
		m_textures[id]->GetLevelDesc(0, &surfaceDesc);
		cEvent widthReq(WindowWidthReq);
		cEvent heightReq(WindowHeightReq);
		g_eventMngr.SendEvent( &widthReq );
		g_eventMngr.SendEvent( &heightReq );
		UINT windowWidth = widthReq.GetParam(0).m_asInt;
		UINT windowHeight = heightReq.GetParam(0).m_asInt;

//		debug << "window width and height: " << windowWidth << "\n\t" << windowHeight << endl;
		float xScalar = (float)windowWidth / (float)surfaceDesc.Width;
		float yScalar = (float)windowHeight / (float)surfaceDesc.Height;

		D3DXMatrixScaling(&scale, xScalar, yScalar, 1.0f);
	}
	else
	{
		D3DXMatrixScaling(&scale, scaleVec->m_x, scaleVec->m_y, 1.0f);
	}
	m_sprite->SetTransform(&scale);

	//if NULL, set pos to 0, 0
	D3DXVECTOR3 vecPos;
	if (destPos == NULL)
	{
		vecPos.x = vecPos.y = 0.0f;
		vecPos.z = 0.0f;
	}
	else
	{
		vecPos.x = destPos->m_x;
		vecPos.y = destPos->m_y;
		vecPos.z = 0.0f;
	}

	m_sprite->Begin(D3DXSPRITE_ALPHABLEND);
	m_sprite->Draw(m_textures[id], NULL, NULL, &vecPos, colorMod); //D3DXVECTOR3(0/*(float)(srcRect->left)*/, 0/*(float)(srcRect->top)*/, 0.0f)
	m_sprite->End();

	return true;
}

/////////////////////////////////////////////////
bool cGraphics::LoadTextFont(GraphicID id, int fontHeight)
{
	D3DXFONT_DESC fontDesc;
	LPD3DXFONT font = NULL;

	ZeroMemory(&fontDesc, sizeof (fontDesc));
	string fontName = id.GetStr();
	fontName.copy((char*)fontDesc.FaceName,fontName.length());
	//(char*)(fontDesc.FaceName) = (char*)fontName.c_str();
	fontDesc.Height = fontHeight;
	HRESULT hr = D3DXCreateFontIndirect(m_dev, &fontDesc, &font);
	if (hr != D3D_OK)
	{
		ErrorMsg("cGraphics::LoadTextFont font creation fail");
		return false;
	}

	m_fonts[id] = font;

	return true;
}

/////////////////////////////////////////////////
bool cGraphics::DrawMyText (const GraphicID& fontID, 
						    const string& s, RECT* rc, DWORD* format, 
						    DWORD* color, int* newFontSize)
{
//	debug << "string: " << s.c_str() << endl;
//	debug << "Checkpoint 1" << endl;

	RECT tempRect;
	DWORD tempFMT;
	DWORD tempColor;

	//check for font in list, return if not there
	FONT_ITER it = m_fonts.find(fontID);
	if(it == m_fonts.end())
	{
		ErrorMsg("cGraphics::DrawText invalid font sent");
		return false;
	}

	//debug << "font found" << endl;
	//resize font if requested
	if (newFontSize != NULL)
	{
		//debug << "font size needed" << endl;
#ifdef MY_DEBUG
		//debug << "resize font called with val: " << (int)(*newFontSize) << endl;
#endif
		//debug << "font size needed" << endl;
		ResizeFont( fontID, (int)(*newFontSize) );
	}
	//debug << "font size updated" << endl;

	//check if format is NULL, if null, default to center text, no clip
	if (format == NULL)
	{
#ifdef MY_DEBUG
		debug << "\tUsing Default format" << endl;
#endif
		tempFMT = DT_CENTER | DT_NOCLIP;
	}
	else
	{
#ifdef MY_DEBUG
		debug << "\tUsing Passed format: " << *format << endl;
#endif
		tempFMT = *format;
	}

	//check if rect is NULL, default to full screen
	if (rc == NULL)
	{
#ifdef MY_DEBUG
		debug << "\tUsing Default RECT" << endl;
#endif
		cEvent widthReq(WindowWidthReq);
		cEvent heightReq(WindowHeightReq);
		g_eventMngr.SendEvent( &widthReq );
		g_eventMngr.SendEvent( &heightReq );

		tempRect.top = 0;
		tempRect.left = 0;
		tempRect.bottom = heightReq.GetParam(0).m_asInt;
		tempRect.right = widthReq.GetParam(0).m_asInt;
	}
	//if rect has no height or width, a calcrect is requested
	else if (rc->top == rc->bottom || rc->left == rc->right)
	{
#ifdef MY_DEBUG
		debug << "\tCalculating RECT" << endl;
#endif
		it->second->DrawText(
		NULL, s.c_str(), -1, &tempRect, DT_CALCRECT, 
		0xFFFFFFFF);
	}
	else
	{
#ifdef MY_DEBUG
		debug << "\tUsing Passed RECT: (T, L, B, R)" 
			<< rc->top << ", "<< rc->left << ", "
			<< rc->bottom << ", " << rc->right << endl;
#endif
		tempRect = *rc;
	}
	
	//if color is NULL or invalid, make white
	if (color == NULL || *color < 0x0 || *color > 0xFFFFFFFF)
	{
#ifdef MY_DEBUG
		debug << "\tUsing Default color" << endl;
#endif
		tempColor = 0xFFFFFFFF;
		//color = &tempColor;
	}
	else
	{
#ifdef MY_DEBUG
		debug << "\tUsing Passed color: " << *color << endl;
#endif
		tempColor = *color;
	}

	it->second->DrawText(
		NULL, s.c_str(), -1, &tempRect, tempFMT, 
		tempColor);

	return true;
}

/////////////////////////////////////////////////
bool cGraphics::ResizeFont(const GraphicID& id, int newSize)
{
	//debug << "cGraphics::ResizeFont started" << endl;
	D3DXFONT_DESC desc;
	m_fonts[id]->GetDesc(&desc);
	if (desc.Height != newSize)
	{
		//LPD3DXFONT font = NULL;

		//debug << "cGraphics::ResizeFont new size needed" << endl;
		desc.Height = newSize;
		//debug << "cGraphics::ResizeFont deleting old font" << endl;
		//m_fonts.erase(id);
		//m_fonts[id] = NULL;
		//debug << "cGraphics::ResizeFont old font deleted" << endl;
		HRESULT hr = D3DXCreateFontIndirect(m_dev, &desc, &m_fonts[id]);
		if (hr != D3D_OK)
		{
			ErrorMsg("cGraphics::LoadTextFont font creation fail");
			return false;
		}
		//debug << "cGraphics::ResizeFont new font created" << endl;
		//m_fonts[id] = font;
	}
	//debug << "cGraphics::ResizeFont ending" << endl;
	return true;
}

/////////////////////////////////////////////////
bool cGraphics::LoadSurface(const GraphicID& id, RECT* srcRect, 
							RECT* destRect, DWORD colorKey)
{
#ifdef MY_DEBUG
	debug << "cGraphics::LoadSurface Begin" << endl;
	debug << "SURFACE ID: " << id.GetID() << endl;
#endif

	D3DXIMAGE_INFO imageInfo;
	LPDIRECT3DSURFACE9 image = NULL;

	//get image info from file
	HRESULT hr = D3DXGetImageInfoFromFile (id.GetStr(), &imageInfo);
#ifdef MY_PROFILE
	if(hr != D3D_OK)
	{
		ErrorMsg("cGraphics::LoadSurface image not loaded");
		return false;
	}
#endif

	//get backbuffer info to allow StretchRect to work between surfaces
	D3DSURFACE_DESC backDesc;
	hr = m_backBuffer->GetDesc(&backDesc);
#ifdef MY_PROFILE
	if(hr != D3D_OK)
	{
		ErrorMsg("cGraphics::LoadSurface backbuffer description");
		return false;
	}
#ifdef MY_DEBUG
	else
	{
		debug << imageInfo.Width << " " << imageInfo.Height << endl;
		debug << backDesc.Format << " " << backDesc.MultiSampleType << " " 
			  << backDesc.MultiSampleQuality << endl;
	}
#endif
#endif

	//create surface using backbuffer data
	hr = m_dev->CreateRenderTarget(
		imageInfo.Width, imageInfo.Height,
					backDesc.Format, backDesc.MultiSampleType,
					backDesc.MultiSampleQuality, FALSE, &image, NULL);

#ifdef MY_PROFILE
	if(hr != D3D_OK)
	{
		ErrorMsg("cGraphics::LoadSurface surface not created");
		debug << hr << endl;
		return false;
	}
#endif

	//load image on to surface
	hr = D3DXLoadSurfaceFromFile(image, NULL, 
			destRect, id.GetStr(), srcRect, D3DX_DEFAULT, 
			colorKey, NULL);
#ifdef MY_PROFILE
	if(hr != D3D_OK)
	{
		ErrorMsg("cGraphics::LoadSurface surface not loaded with img");
		return false;
	}
#endif

	//save surface
	m_surfaces[id] = image;

#ifdef MY_DEBUG
	debug << "cGraphics::LoadSurface End" << endl;
#endif
	return true;
}

/////////////////////////////////////////////////
bool cGraphics::SaveSurface(const GraphicID& id, LPDIRECT3DSURFACE9 srcSurface)
{
	debug << "cGraphics::SaveSurface Begin" << endl;
	HRESULT hr;
	

	//if surface saving to exists, delete for overwriting.
	SRF_ITER it = m_surfaces.find(id);
	if (it != m_surfaces.end())
	{
		it->second->Release();
	}

	//create new surface
	LPDIRECT3DSURFACE9 temp = NULL;
	D3DSURFACE_DESC srcDesc;
	hr = srcSurface->GetDesc(&srcDesc);
#ifdef MY_PROFILE
	if(hr != D3D_OK)
	{
		ErrorMsg("cGraphics::SaveSurface source surface description");
		return false;
	}
#ifdef MY_DEBUG
	else
	{
		debug << srcDesc.Format << " " << srcDesc.MultiSampleType << " " 
			  << srcDesc.MultiSampleQuality << endl;
	}
#endif
#endif
	//create surface using source surface data
	hr = m_dev->CreateRenderTarget(
					srcDesc.Width, srcDesc.Height,
					srcDesc.Format, srcDesc.MultiSampleType,
					srcDesc.MultiSampleQuality, FALSE, &temp, NULL);
#ifdef MY_PROFILE
	if(hr != D3D_OK)
	{
		ErrorMsg("cGraphics::SaveSurface surface not created");
		debug << hr << endl;
		return false;
	}
#endif

	hr = D3DXLoadSurfaceFromSurface(temp, NULL, NULL,
							srcSurface, NULL, NULL, D3DX_DEFAULT, 0);
#ifdef MY_PROFILE
	if (hr != D3D_OK)
	{
		ErrorMsg("cGraphics::SaveSurface surface saving failed");
		return false;
	}
#endif

	//save new surface
	m_surfaces[id] = temp;

	debug << "cGraphics::SaveSurface End" << endl;
	return true;
}

/////////////////////////////////////////////////
bool cGraphics::DrawSurface (const GraphicID& id, RECT* src, RECT* dest)  
{
#ifdef MY_DEBUG
	//debug << "cGraphics::DrawSurface Begin" << endl;
	//debug << "SURFACE ID: " << id.GetID() << endl;
	//debug << m_surfaces.empty() << endl;
#endif

#ifdef MY_PROFILE
	SRF_ITER it = m_surfaces.find (id);
	if (it == m_surfaces.end ())
	{
		ErrorMsg("cGraphics::DrawSurface Surface ID not found");
		return false;
	}
#endif
	//HRESULT hr = m_dev->StretchRect(m_surfaces[id], NULL, 
	//						m_backBuffer, NULL, D3DTEXF_NONE);
	HRESULT hr = D3DXLoadSurfaceFromSurface(m_backBuffer, NULL, NULL, m_surfaces[id], NULL, NULL, D3DX_DEFAULT, 0);
#ifdef MY_PROFILE
	if(hr != D3D_OK)
	{
		ErrorMsg("cGraphics::DrawSurface StretchRect failed");
		debug << "\t" << hr << endl;
		return false;
	}
#endif

#ifdef MY_DEBUG
	//debug << "cGraphics::DrawSurface End" << endl;
#endif
	return true;
}