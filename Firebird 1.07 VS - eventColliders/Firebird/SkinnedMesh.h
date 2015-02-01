#ifndef __SKINNEDMESH_H__
#define __SKINNEDMESH_H__

#include <string>
using std::string;
#include <vector>
using std::vector;

#include <d3dx9.h>

#include "Model.h"

/////////////////////////////////////////////////
struct Bone : public D3DXFRAME
{
	D3DXMATRIX m_world;
};

/////////////////////////////////////////////////
class BoneLoader : public ID3DXAllocateHierarchy
{
public:

	virtual HRESULT __stdcall CreateFrame(
		LPCSTR Name,
		LPD3DXFRAME* ppNewFrame
		);

	virtual HRESULT __stdcall CreateMeshContainer(
		LPCSTR Name,
		CONST D3DXMESHDATA * pMeshData,
		CONST D3DXMATERIAL * pMaterials,
		CONST D3DXEFFECTINSTANCE * pEffectInstances,
		DWORD NumMaterials,
		CONST DWORD * pAdjacency,
		LPD3DXSKININFO pSkinInfo,
		LPD3DXMESHCONTAINER * ppNewMeshContainer
		);

	virtual HRESULT __stdcall DestroyFrame(
		LPD3DXFRAME pFrameToFree
		);

	virtual HRESULT __stdcall DestroyMeshContainer(
		LPD3DXMESHCONTAINER pMeshContainerToFree
		);

};

/////////////////////////////////////////////////
struct BoneMesh : public D3DXMESHCONTAINER
{
	ID3DXMesh* OriginalMesh;
	vector<D3DMATERIAL9> materials;
	//vector<IDirect3DTexture9*> textures;
	GraphicID* textures;

	DWORD NumAttributeGroups;
	D3DXATTRIBUTERANGE*attributeTable;
	D3DXMATRIX** boneMatrixPtrs;
	D3DXMATRIX* boneOffsetMatrices;
	D3DXMATRIX* currentBoneMatrices;
};

/////////////////////////////////////////////////
class cSkinnedMesh : public cModel
{
protected:
	D3DXFRAME* m_rootBone;

	EventID AttachTexture;

	void UpdateMatrices(Bone*, D3DXMATRIX*);
	void SetupBoneMatrixPointers(Bone*);

	void Render(Bone*);
public:
	cSkinnedMesh();
	virtual ~cSkinnedMesh();

	virtual bool Init(const string&);
	virtual bool Draw(D3DXMATRIX*);
};

/////////////////////////////////////////////////
#endif // __SKINNEDMESH_H__