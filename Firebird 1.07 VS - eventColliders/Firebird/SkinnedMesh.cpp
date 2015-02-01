#include "Debug.h"
#include "Game.h"
#include "SkinnedMesh.h"

/////////////////////////////////////////////////
HRESULT BoneLoader::CreateFrame(
								LPCSTR Name, LPD3DXFRAME *ppNewFrame)
{
	//debug << "\nBegin BoneLoader::CreateFrame()\n";
	Bone* newBone = new Bone;
	//debug /*<< "BoneLoader::CreateFrame() creating Bone " */<< newBone << "\t";
	ZeroMemory(newBone, sizeof(Bone));

	// Copy name
	if(Name != NULL)
	{
		//debug << /*"Bone name: " <<*/ Name << "\t";
		int len = strlen(Name) + 1;
		newBone->Name = new char[len];
		//debug /*<< "BoneLoader::CreateFrame() Creating bone name at "*/ << (void*)(newBone->Name) << "\n";
		strcpy_s(newBone->Name, len, Name);
		//debug << "Bone name: " << newBone->Name << "\n";
	}

	// Set the transform matrices
	D3DXMatrixIdentity(&newBone->TransformationMatrix);
	D3DXMatrixIdentity(&newBone->m_world);

	// Return the new bone
	*ppNewFrame = (D3DXFRAME*)newBone;

	//debug << "End BoneLoader::CreateFrame()\n\n";
	return S_OK;
}

/////////////////////////////////////////////////
HRESULT BoneLoader::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	//debug << "\nBegin BoneLoader::DestroyFrame()\n";

	if(pFrameToFree)
	{
		// Free name string
		//debug << "Deleting bone name at " << (void*)(pFrameToFree->Name) << "\n";
		delete[] pFrameToFree->Name;

		if(pFrameToFree->pMeshContainer != NULL)
			DestroyMeshContainer(pFrameToFree->pMeshContainer);

		if(pFrameToFree->pFrameFirstChild != NULL)
			DestroyFrame(pFrameToFree->pFrameFirstChild);

		if(pFrameToFree->pFrameSibling != NULL)
			DestroyFrame(pFrameToFree->pFrameSibling);

		// Free frame
		//debug << "Deleting Bone at " << pFrameToFree << "\n";
		delete pFrameToFree;
	}

	pFrameToFree = NULL;
	//debug << "End BoneLoader::DestroyFrame()\n";
	return S_OK;
}

/////////////////////////////////////////////////
HRESULT BoneLoader::CreateMeshContainer(
										LPCSTR Name, const D3DXMESHDATA* pMeshData, 
										const D3DXMATERIAL* pMaterials, 
										const D3DXEFFECTINSTANCE* pEffesctInstances, 
										DWORD NumMaterials, const DWORD* pAdjacency, 
										LPD3DXSKININFO pSkinInfo, 
										LPD3DXMESHCONTAINER* ppNewMeshContainer)
{
	//debug << "\nBegin BoneLoader::CreateMeshContainer()\n";
	// Create new bone mesh
	BoneMesh* boneMesh = new BoneMesh;
	ZeroMemory(boneMesh, sizeof(BoneMesh));
	//debug /*<< "BoneLoader::CreateMeshContainer() creating BoneMesh at " */<< boneMesh << "\t";

	// Get mesh data
	boneMesh->OriginalMesh = pMeshData->pMesh;
	boneMesh->MeshData.pMesh = pMeshData->pMesh;
	boneMesh->MeshData.Type = pMeshData->Type;

	// Add reference so the mesh is not deallocated
	pMeshData->pMesh->AddRef();

	LPDIRECT3DDEVICE9 dev = NULL;
	pMeshData->pMesh->GetDevice(&dev);

	boneMesh->textures = new GraphicID[NumMaterials];
	// Copy materials and load textures just like with a static mesh
	for(DWORD i = 0; i < NumMaterials; ++i)
	{
		D3DXMATERIAL mtrl;
		memcpy(&mtrl, &pMaterials[i], sizeof(D3DXMATERIAL));
		mtrl.MatD3D.Ambient.r = mtrl.MatD3D.Diffuse.r * 0.5f;
		mtrl.MatD3D.Ambient.g = mtrl.MatD3D.Diffuse.g * 0.5f;
		mtrl.MatD3D.Ambient.b = mtrl.MatD3D.Diffuse.b * 0.5f;
		mtrl.MatD3D.Specular.r = mtrl.MatD3D.Diffuse.r * 0.2f;
		mtrl.MatD3D.Specular.g = mtrl.MatD3D.Diffuse.g * 0.2f;
		mtrl.MatD3D.Specular.b = mtrl.MatD3D.Diffuse.b * 0.2f;
		mtrl.MatD3D.Power = 1.0f;
		boneMesh->materials.push_back(mtrl.MatD3D);
		//IDirect3DTexture9* tex = NULL;
		EventID LoadTexture("GRAPHICS_TEXTURE_LOAD_EVENT");

		if(mtrl.pTextureFilename != NULL)
		{      
			boneMesh->textures[i].SetStr(
				mtrl.pTextureFilename);

			g_eventMngr.SendEvent(&cEvent(LoadTexture, 
				boneMesh->textures[i].GetID()));
		}
	}

	if(pSkinInfo != NULL)
	{
		// Get skin info
		boneMesh->pSkinInfo = pSkinInfo;

		// Add reference so SkinInfo isn't deallocatd
		pSkinInfo->AddRef();

		// Clone mesh and store in boneMesh->MeshData.pMesh
		pMeshData->pMesh->CloneMeshFVF(
			D3DXMESH_MANAGED, 
			pMeshData->pMesh->GetFVF(), dev, 
			&boneMesh->MeshData.pMesh);

		// Get attribute table
		boneMesh->MeshData.pMesh->GetAttributeTable(
			NULL, &boneMesh->NumAttributeGroups);

		boneMesh->attributeTable = 
			new D3DXATTRIBUTERANGE[boneMesh->NumAttributeGroups];
		//debug << boneMesh->attributeTable << "\t";

		boneMesh->MeshData.pMesh->GetAttributeTable(
			boneMesh->attributeTable, NULL);

		// Create bone offset and current matrices
		int NumBones= pSkinInfo->GetNumBones();

		boneMesh->boneOffsetMatrices = new D3DXMATRIX[NumBones];

		boneMesh->currentBoneMatrices = new D3DXMATRIX[NumBones];

		// Get bone offset matrices
		for(int i = 0; i <NumBones; ++i)
			boneMesh->boneOffsetMatrices[i] = *(boneMesh->pSkinInfo->GetBoneOffsetMatrix(i));
	}

	// Set ppNewMeshContainer to the newly created boneMesh container
	*ppNewMeshContainer = boneMesh;

	//debug << "End BoneLoader::CreateMeshContainer()\n\n";
	return S_OK;
}

/////////////////////////////////////////////////
HRESULT BoneLoader::DestroyMeshContainer(
	LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	//debug << "Begin BoneLoader::DestroyMeshContainer()\n";
	BoneMesh *boneMesh = (BoneMesh*)pMeshContainerToFree;

	//Release textures
	int numTextures = (int)boneMesh->NumAttributeGroups;//boneMesh->textures.size();

	delete[] boneMesh->textures;

	//Release mesh data
	if(boneMesh->MeshData.pMesh)
		boneMesh->MeshData.pMesh->Release();
	if(boneMesh->pSkinInfo)
		boneMesh->pSkinInfo->Release();
	if(boneMesh->OriginalMesh)
		boneMesh->OriginalMesh->Release();

	if(boneMesh->currentBoneMatrices)
		delete [] boneMesh->currentBoneMatrices;
	if(boneMesh->boneOffsetMatrices)
		delete [] boneMesh->boneOffsetMatrices;
	if(boneMesh->attributeTable)
		delete [] boneMesh->attributeTable;
	if(boneMesh->boneMatrixPtrs)
		delete [] boneMesh->boneMatrixPtrs;

	delete boneMesh;

	//debug << "End BoneLoader::DestroyMeshContainer() \n";
	return S_OK;
}

/////////////////////////////////////////////////
cSkinnedMesh::cSkinnedMesh()
: m_rootBone(NULL),
//LoadTexture("GRAPHICS_TEXTURE_LOAD_EVENT"),
AttachTexture("GRAPHICS_TEXTURE_ATTACH_EVENT")
{
	//debug << "cSkinnedMesh::cSkinnedMesh() \n";
}

/////////////////////////////////////////////////
cSkinnedMesh::~cSkinnedMesh()
{
	//debug << "Begin cSkinnedMesh::~cSkinnedMesh() \n";

	BoneLoader boneHierarchy;
	boneHierarchy.DestroyFrame(m_rootBone);

	//debug << "End cSkinnedMesh::~cSkinnedMesh() \n";
}

/////////////////////////////////////////////////
void cSkinnedMesh::UpdateMatrices(Bone* bone, D3DXMATRIX* parentMatrix)
{
	//debug << "Begin cSkinnedMesh::UpdateMatrices() \n";
	if(bone == NULL)return;

	//Calculate the combined transformation matrix
	bone->m_world = 
		bone->TransformationMatrix * (*parentMatrix);

	//Perform the same calculation on all siblings
	if(bone->pFrameSibling)
	{
		UpdateMatrices((Bone*)bone->pFrameSibling, 
			parentMatrix);
	}

	//... and all children
	if(bone->pFrameFirstChild)
	{
		UpdateMatrices((Bone*)bone->pFrameFirstChild, 
			&bone->m_world);
	}
	//debug << "End cSkinnedMesh::UpdateMatrices() \n";
}

/////////////////////////////////////////////////
bool cSkinnedMesh::Init(const string& file)
{
	//debug << "Begin cSkinnedMesh::Init() \n";
	BoneLoader loader;

	HRESULT hr = D3DXLoadMeshHierarchyFromX(file.c_str(), 
		D3DXMESH_MANAGED, s_dev, &loader, 
		NULL, &m_rootBone, NULL);

	if(hr != D3D_OK)
	{
		ErrorMsg("Skinned mesh failed to load");
		return false;
	}

	//debug << "DONE LOADING SKINNED MESH\n";
	SetupBoneMatrixPointers((Bone*)m_rootBone);

	D3DXMATRIX i;

	D3DXMatrixIdentity(&i);

	UpdateMatrices((Bone*)m_rootBone, &i);
	//debug << "End cSkinnedMesh::Init() \n";
	return true;
}

/////////////////////////////////////////////////
void cSkinnedMesh::SetupBoneMatrixPointers(Bone* bone)
{
	//debug << "Begin cSkinnedMesh::SetupBoneMatrixPointers() \n";
	// Find all bones containing a mesh
	if(bone->pMeshContainer != NULL)
	{
		BoneMesh* boneMesh = (BoneMesh*)bone->pMeshContainer;

		// For the bones with skinned meshes, setup the pointers
		if(boneMesh->pSkinInfo != NULL)
		{
			// Get num bones influencing this mesh
			int NumBones = boneMesh->pSkinInfo->GetNumBones();

			// Create an array of pointers with NumBones pointers
			boneMesh->boneMatrixPtrs = new D3DXMATRIX*[NumBones];
			//debug << "Creating Bone Matrices at " << boneMesh->boneMatrixPtrs << "\n";

			// Fill array
			for(int i = 0; i < NumBones; ++i)
			{
				// Find influencing bone by name...
				Bone* b = (Bone*)D3DXFrameFind(m_rootBone,
					boneMesh->pSkinInfo->GetBoneName(i));

				// ...and store pointer to it in the array
				if(b != NULL)
				{
					boneMesh->boneMatrixPtrs[i] = &b->m_world;
				}
				else
				{
					boneMesh->boneMatrixPtrs[i] = NULL;
				}
			}
		}
	}

	// Traverse the rest of the hierarchy
	if(bone->pFrameSibling != NULL)
		SetupBoneMatrixPointers((Bone*)bone->pFrameSibling);

	if(bone->pFrameFirstChild != NULL)
		SetupBoneMatrixPointers((Bone*)bone->pFrameFirstChild);

	//debug << "End cSkinnedMesh::SetupBoneMatrixPointers()" << endl;
}

/////////////////////////////////////////////////
void cSkinnedMesh::Render(Bone* bone)
{
	//debug << "Begin cSkinnedMesh::Render()" << endl;
	if(bone == NULL)
		bone = (Bone*)m_rootBone;

	// If there is a mesh to render...
	if(bone->pMeshContainer != NULL)
	{
		BoneMesh* boneMesh = (BoneMesh*)bone->pMeshContainer;

		if(boneMesh->pSkinInfo != NULL)
		{
			// Set up bone transforms
			int numBones = boneMesh->pSkinInfo->GetNumBones();
			for(int i = 0; i < numBones; ++i)
			{
				D3DXMatrixMultiply(
					&boneMesh->currentBoneMatrices[i],
					&boneMesh->boneOffsetMatrices[i],
					boneMesh->boneMatrixPtrs[i]);
			}

			// Update the skinned mesh
			BYTE* src = NULL;
			BYTE* dest = NULL;
			boneMesh->OriginalMesh->LockVertexBuffer(
				D3DLOCK_READONLY, (void**)&src);
			boneMesh->MeshData.pMesh->LockVertexBuffer(
				0, (void**)&dest);

			boneMesh->pSkinInfo->UpdateSkinnedMesh(boneMesh->currentBoneMatrices,
				NULL, src, dest);

			boneMesh->MeshData.pMesh->UnlockVertexBuffer();
			boneMesh->OriginalMesh->UnlockVertexBuffer();

			// Render the mesh
			for(DWORD i = 0; i < boneMesh->NumAttributeGroups; ++i)
			{
				int mtrl = boneMesh->attributeTable[i].AttribId;
				s_dev->SetMaterial(&(boneMesh->materials[mtrl]));
				//s_dev->SetTexture(0, boneMesh->textures[i]);

				g_eventMngr.SendEvent(&cEvent(AttachTexture, boneMesh->textures[i].GetID(), 0));

				boneMesh->MeshData.pMesh->DrawSubset(mtrl);
			}
		}
	}

	// Render siblings & children
	if(bone->pFrameSibling != NULL)
		Render((Bone*)bone->pFrameSibling);

	if(bone->pFrameFirstChild != NULL)
		Render((Bone*)bone->pFrameFirstChild);

	//debug << "End cSkinnedMesh::Render()" << endl;
}

/////////////////////////////////////////////////
bool cSkinnedMesh::Draw (D3DXMATRIX* world)
{
	//debug << "Begin cSkinnedMesh::Draw()" << endl;
	s_dev->SetTransform(D3DTS_WORLD, world);
	Render(NULL);
	//debug << "End cSkinnedMesh::Draw()" << endl;
	return true;
}