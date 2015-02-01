#include <algorithm>
#include <iomanip>
#include <sstream>

#include "debug.h"
#include "EventMngr.h"
#include "Level.h"

#include "BoundingVolume.h"
/*
/////////////////////////////////////////////////
sMapTile::sMapTile()
{
	for(int i = 0; i < 8; ++i)
		m_neighbors[i] = NULL;
}

/////////////////////////////////////////////////
sMapTile::sMapTile(DWORD x, DWORD y)
: m_loc(x,y)
{
	for(int i = 0; i < 8; ++i)
		m_neighbors[i] = NULL;
}
*/
/////////////////////////////////////////////////
cLevel::cLevel()  
: //TileWalkableRequest("LEVEL_TILE_WALKABLE_REQUEST"),
//FindPathRequest("LEVEL_FIND_PATH_REQUEST"),
GraphicLoadModel("GRAPHICS_MODEL_LOAD_EVENT"),
LoadEntity("ENTITY_LOAD_EVENT"),
MoveEntity("ENTITY_POSITION_EVENT"),
RotateEntity("ENTITY_ROTATE_EVENT"),
ScaleEntity("ENTITY_SCALE_EVENT"),
AttachModel("ENTITY_ATTACH_MODEL_EVENT"),
AttachHeightmap("ENTITY_ATTACH_HEIGHTMAP_EVENT"),
LoadCollidable("ENTITY_LOAD_COLLIDABLE_EVENT"),
InitPhysicsEvent("LEVEL_INIT_PHYSICS_EVENT")
{
	//g_eventMngr.RegisterListener(TileWalkableRequest, this);
	//g_eventMngr.RegisterListener(FindPathRequest, this);
}

/////////////////////////////////////////////////
cLevel::~cLevel()  
{
	#ifdef MY_DEBUG
		debug << "~cLevel start" << endl;
	#endif

	// Delete all tile objects
/*	while(!m_tiles.empty())
	{
		TILE_ITER it = m_tiles.begin();
		delete it->second;
		it->second = NULL;
		m_tiles.erase(it);
	}
*/
	#ifdef MY_DEBUG
		debug << "~cLevel end" << endl;
	#endif
}

/////////////////////////////////////////////////
bool cLevel::Init(const string& file)
{
	//debug << "Begin cLevel::Init(" << file << ")\n";

	ifstream fin(file.c_str());
	if(!fin)  return false;

	string str;
	fin >> str;

	m_id.SetStr(str.c_str());

	InitModels(fin);
	InitEntities(fin);
	InitPhysics(fin);

	fin.close();

	//debug << "End cLevel::Init()" << endl;
	return true;
}

/////////////////////////////////////////////////
bool cLevel::InitModels(ifstream& fin)
{
	#ifdef MY_DEBUG
		debug << "Begin cLevel::InitModels ()" << endl;
	#endif
	if (!fin.is_open())    return false;

	// Read list of model IDs
	string type, xfile;
	fin >> type;
	if(type != "[BeginModelList]")
	{
		ErrorMsg("Reading model list");
		return false;
	}

	// For each name, create a model object
	fin >> type;  
	while(type != "[EndModelList]")
	{
		fin >> xfile;
		g_eventMngr.SendEvent(
			&cEvent(GraphicLoadModel, 
			sVarient(type.c_str()), 
			sVarient(xfile.c_str())));

		fin >> type;

		if(!fin)  return false;
	}

	#ifdef MY_DEBUG
		debug << "End cLevel::InitModels ()" << endl;
	#endif
	return true;
}

/////////////////////////////////////////////////
bool cLevel::InitEntities(ifstream& fin)
{
	#ifdef MY_DEBUG
		debug << "Begin cLevel::InitEntities ()" << endl;
	#endif
	if(!fin.is_open())    return false;

	// Read list of entity IDs
	string type, id, model, collider, colliderType;
	int colliderCount = 0;
	fin >> type;
	if(type != "[BeginEntityList]")
	{
		ErrorMsg("Reading model list");
		return false;
	}

	// For each name, create a entity object
	fin >> type;

	while(type != "[EndEntityList]")
	{ 
		sVector3d pos, rot, scl, colStartDimensions, colEndDimensions;
		fin >> id >> model;

		fin	>> pos.m_x >> pos.m_y >> pos.m_z
			>> rot.m_x >> rot.m_y >> rot.m_z
			>> scl.m_x >> scl.m_y >> scl.m_z;

		EntityID entityId(id.c_str());
		GraphicID modelId(model.c_str());

		g_eventMngr.SendEvent(
			&cEvent(LoadEntity, entityId.GetID(),
			type.c_str()));

		g_eventMngr.SendEvent(
			&cEvent(MoveEntity, entityId.GetID(), 
			pos.m_x, pos.m_y, pos.m_z));

		g_eventMngr.SendEvent(
			&cEvent(RotateEntity, entityId.GetID(), 
			rot.m_x, rot.m_y, rot.m_z));

		g_eventMngr.SendEvent(
			&cEvent(ScaleEntity, entityId.GetID(), 
			scl.m_x, scl.m_y, scl.m_z));

		if(model != "NoModel")
			g_eventMngr.SendEvent(
			&cEvent(AttachModel, entityId.GetID(),
			modelId.GetID()));

		//get collider data, if exists
		fin >> colliderCount;
		for (int i = 0; i < colliderCount; i++)
		{
			fin >> colliderType;
			if (colliderType == "Heightmap")
			{
				fin >> collider;
				#ifdef MY_DEBUG
					debug << "Attach Heightmap event sent" << endl;
				#endif
				g_eventMngr.SendEvent(
					&cEvent(AttachHeightmap, entityId.GetID(), collider.c_str())
					);
			}
			else
			{				
					fin >> colStartDimensions.m_x 
						>> colStartDimensions.m_y 
						>> colStartDimensions.m_z
						>> colEndDimensions.m_x 
						>> colEndDimensions.m_y 
						>> colEndDimensions.m_z;

					//debug << "cLevel: sending LoadCollidable event" << endl;
					g_eventMngr.SendEvent( &cEvent(LoadCollidable, 
												   entityId.GetID(), 
												   colliderType.c_str(),
												   &colStartDimensions,
												   &colEndDimensions) );
			}
		}

		fin >> type;

		if(!fin)  
		{
			ErrorMsg("Level FileIn Failed");
			return false;
		}
	}

	#ifdef MY_DEBUG
		debug << "End cLevel::InitEntities ()" << endl;
	#endif
	return true;
}

/////////////////////////////////////////////////
bool cLevel::InitPhysics(ifstream& fin)
{
	#ifdef MY_DEBUG
		debug << "Begin cLevel::InitPhysics ()" << endl;
	#endif
	string type;
	fin >> type;
	if(type != "[BeginWorldPhysics]")
	{
		ErrorMsg("Reading level physics");
		return false;
	}

	float worldGravityVel;
	float worldMaxFallVel;
	float worldJumpVel;
	float worldFallDPS;
	fin >> worldGravityVel >> worldMaxFallVel 
		>> worldJumpVel >> worldFallDPS;
	g_eventMngr.SendEvent( &cEvent(InitPhysicsEvent, sVarient(worldGravityVel),
						   sVarient(worldMaxFallVel), sVarient(worldJumpVel),
						   sVarient(worldFallDPS)) );

	fin >> type;
	if(type != "[EndWorldPhysics]")
	{
		ErrorMsg("Ending level physics");
		return false;
	}

	#ifdef MY_DEBUG
		debug << "End cLevel::InitPhysics ()" << endl;
	#endif
	return true;
}

/////////////////////////////////////////////////
void cLevel::HandleEvent(cEvent* e)
{
	//debug << "Begin cLevel::HandleEvent(" 
	//      << e->GetEventID().GetStr() << ")\n";

	//debug << "End cLevel::HandleEvent() ==> " 
	//      << e->GetParam(3).m_asBool << endl;
}
