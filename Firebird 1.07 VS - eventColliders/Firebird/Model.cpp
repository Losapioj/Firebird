#include <string>
#include <sstream>
using std::string;
#include "Debug.h"
#include "Game.h"
#include "Model.h"

/////////////////////////////////////////////////
LPDIRECT3DDEVICE9 cModel::s_dev = NULL;

/////////////////////////////////////////////////
cXModel::cXModel ()
: m_mesh(NULL), m_textures(NULL), 
m_materials(NULL), m_subsetCount(0),
LoadTexture("GRAPHICS_TEXTURE_LOAD_EVENT"),
AttachTexture("GRAPHICS_TEXTURE_ATTACH_EVENT")
{}

/////////////////////////////////////////////////
cXModel::~cXModel ()
{
	#ifdef MY_DEBUG
		debug << "~cXModel start" << endl;
	#endif

	if(m_textures != NULL)
		delete[] m_textures;
	m_textures = NULL;

	if(m_materials != NULL)
		delete[] m_materials;
	m_materials = NULL;

	if(m_mesh != NULL)
		m_mesh->Release();
	m_mesh = NULL;
	
	#ifdef MY_DEBUG
		debug << "~cXModel end" << endl;
	#endif
}

/////////////////////////////////////////////////
bool cXModel::Init (const string& xfile)
{
	//debug << "Begin cXModel::Init (" << xfile << ")\n";
	ID3DXBuffer* adj = NULL;
	ID3DXBuffer* mtr = NULL;
	DWORD numMtr = 0;

	HRESULT hr = D3DXLoadMeshFromX(xfile.c_str(), 
		D3DXMESH_MANAGED, s_dev, &adj, &mtr, NULL, 
		&m_subsetCount, &m_mesh);

	if(hr != D3D_OK)
	{
		string Msg = "Mesh creation failed: " + xfile;
		ErrorMsg(Msg);
		return false;
	}

	m_materials = new D3DMATERIAL9[m_subsetCount];
	if (m_materials == NULL)
		return false;

	//m_textures = new IDirect3DTexture9*[m_subsetCount];
	m_textures = new GraphicID[m_subsetCount];
	if(m_textures == NULL)
		return false;

	if(mtr != NULL && m_subsetCount != 0)
	{
		D3DXMATERIAL* mtrs = 
			(D3DXMATERIAL*)mtr->GetBufferPointer();

		for(UINT i = 0; i < m_subsetCount; ++i)
		{
			mtrs[i].MatD3D.Ambient = mtrs[i].MatD3D.Diffuse;
			m_materials[i] = mtrs[i].MatD3D;

			if (mtrs[i].pTextureFilename != NULL)
			{
				stringstream ss;
				ss << ".\\Textures\\" << mtrs[i].pTextureFilename;
				m_textures[i].SetStr( ss.str().c_str() );
//				ss.str().f
				g_eventMngr.SendEvent(&cEvent(LoadTexture, m_textures[i].GetID()));
			}
		}
	}

	mtr->Release();

	DWORD* adjacencyInfo = 
		(DWORD*)adj->GetBufferPointer();

	m_mesh->OptimizeInplace (
		D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | 
		D3DXMESHOPT_VERTEXCACHE, adjacencyInfo, 
		NULL, NULL, NULL);

	adj->Release();
	//debug << "End cXModel::Init ()\n";
	return true;
}

/////////////////////////////////////////////////
bool cXModel::Draw(D3DXMATRIX* world)
{
	//debug << "Begin cXModel::Draw()" << endl;
	//s_dev->SetRenderState(D3DRS_LIGHTING, true);

	s_dev->SetTransform(D3DTS_WORLD, world);
	D3DMATERIAL9 mat;
	s_dev->GetMaterial(&mat);

	for (UINT i = 0; i < m_subsetCount; ++i)
	{
		s_dev->SetMaterial(&m_materials[i]);
		g_eventMngr.SendEvent(&cEvent(AttachTexture, m_textures[i].GetID(), 0));

		m_mesh->DrawSubset(i);
	}

	s_dev->SetMaterial (&mat);
	s_dev->SetTexture (0, NULL);

	//debug << "End cXModel::Draw()" << endl;
	return true;
}