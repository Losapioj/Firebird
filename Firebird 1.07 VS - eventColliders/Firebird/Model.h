#ifndef __MODEL_H__
#define __MODEL_H__

#include <d3d9.h>
#include <d3dx9.h>

#include <fstream>
using std::ifstream;

#include <string>
using std::string;

/////////////////////////////////////////////////
class cModel
{
protected:
	static LPDIRECT3DDEVICE9 s_dev;

public:
	virtual ~cModel(){}

	virtual bool Init(const string&) = 0;
	virtual bool Draw(D3DXMATRIX*) = 0;
	static void SetDevice(LPDIRECT3DDEVICE9 dev)
	{  s_dev = dev;  }
};

/////////////////////////////////////////////////
class cXModel : public cModel
{
protected:
	LPD3DXMESH m_mesh;
	GraphicID* m_textures;
	//IDirect3DTexture9** m_textures;
	D3DMATERIAL9* m_materials;

	DWORD m_subsetCount;

	EventID LoadTexture;
	EventID AttachTexture;

public:
	cXModel ();
	virtual ~cXModel ();

	virtual bool Init(const string&);
	virtual bool Draw(D3DXMATRIX*);
};
/////////////////////////////////////////////////
#endif // __MODEL_H__