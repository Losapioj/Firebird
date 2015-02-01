#include "Debug.h"
#include "EventMngr.h"
#include "EntityMngr.h"

/////////////////////////////////////////////////
cEntityMngr::cEntityMngr()
: LoadEntity("ENTITY_LOAD_EVENT"),
MoveEntity("ENTITY_POSITION_EVENT"),
OrientEntity("ENTITY_ROTATE_EVENT"),
ScaleEntity("ENTITY_SCALE_EVENT"),
AttachModel("ENTITY_ATTACH_MODEL_EVENT"),
AttachHeightmap("ENTITY_ATTACH_HEIGHTMAP_EVENT"),
AttachCollider("ENTITY_ATTACH_COLLIDABLE_EVENT"),
LoadCollidable("ENTITY_LOAD_COLLIDABLE_EVENT")
{
	g_eventMngr.RegisterListener(LoadEntity, this);
	g_eventMngr.RegisterListener(MoveEntity, this);
	g_eventMngr.RegisterListener(OrientEntity, this);
	g_eventMngr.RegisterListener(ScaleEntity, this);
	g_eventMngr.RegisterListener(AttachModel, this);
	g_eventMngr.RegisterListener(AttachHeightmap, this);
	g_eventMngr.RegisterListener(LoadCollidable, this);
}

/////////////////////////////////////////////////
cEntityMngr::~cEntityMngr()
{
	#ifdef MY_DEBUG
		debug << "~cEntityMngr start" << endl;
	#endif

	// Delete all entity objects
	while(!m_entities.empty())
	{
		EntityIter it = m_entities.begin();
		delete it->second;
		it->second = NULL;
		m_entities.erase (it);
	}

	#ifdef MY_DEBUG
		debug << "~cEntityMngr end" << endl;
	#endif
}

/////////////////////////////////////////////////
void cEntityMngr::HandleEvent(cEvent* e)
{
	//debug << "Begin cEntityMngr::HandleEvent(" 
	//      << e->GetEventID().GetStr() << ")\n";

	EntityID id;
	id.SetID(e->GetParam(0).m_asPointer);

	EventID eid = e->GetEventID();

	if(eid == LoadEntity)
		CreateEntity(id, e->GetParam(1).m_asString);

	else if(eid == MoveEntity)
	{
		sVector3d pos(e->GetParam(1).m_asFloat,
			e->GetParam(2).m_asFloat,
			e->GetParam(3).m_asFloat);
		#ifdef MY_DEBUG
			debug << "pos = " << pos.m_x << ", " 
							  << pos.m_y << ", " 
							  << pos.m_z << endl;
		#endif
		SetEntityPosition(id, pos);
	}

	else if(eid== OrientEntity)
	{
		sVector3d rot( DegToRad(e->GetParam(1).m_asFloat),
			DegToRad(e->GetParam(2).m_asFloat),
			DegToRad(e->GetParam(3).m_asFloat) );
		SetEntityRotation(id, rot);
	}

	else if(eid == ScaleEntity)
	{
		sVector3d scl(e->GetParam(1).m_asFloat,
			e->GetParam(2).m_asFloat,
			e->GetParam(3).m_asFloat);
		SetEntityScale(id, scl);
	}

	else if(eid == AttachModel)
	{
		GraphicID model;
		model.SetID(e->GetParam(1).m_asPointer);
		SetEntityModel(id, model);
	}

	else if(eid == AttachHeightmap)
	{
		#ifdef MY_DEBUG
			debug << "Attach Heightmap event caught" << endl;
		#endif
		const char* filename = (const char*)e->GetParam(1).m_asPointer;
		SetEntityHeightmap(id, filename);
	}

	else if(eid == LoadCollidable)
	{
		#ifdef MY_DEBUG
			debug << "cEntityMngr: Attaching colider Call" << endl;
		#endif
		SetEntityCollider(id, e->GetParam(1).m_asString,
						  *(sVector3d*)(e->GetParam(2).m_asPointer),
						  *(sVector3d*)(e->GetParam(3).m_asPointer));
	}
	//debug << "End cEntityMngr::HandleEvent()\n";
}

/////////////////////////////////////////////////
bool cEntityMngr::CreateEntity(const EntityID& id, const char* type)
{
	//debug << "Begin cEntityMngr::CreateEntity (" 
	//      << id.GetStr() << ", " << type << ")" 
	//      << endl;

	EntityIter it = m_entities.find(id);
	if (it != m_entities.end())
		return false;

	if(strcmp(type, "Terrain") == 0)
	{
		cTerrainEntity* temp = new cTerrainEntity(id);
		m_entities[id] = temp;
	}

	else if (strcmp(type, "Avatar") == 0)
	{
		cAvatarEntity* temp = new cAvatarEntity(id);
		m_entities[id] = temp;
		//m_moveList[id] = temp;
	}

	else if (strcmp(type, "Actor") == 0)
	{
		cActorEntity* temp = new cActorEntity(id);
		m_entities[id] = temp;
		//m_moveList[id] = temp;
		//m_drawList[id] = temp;
	}

	else
		ErrorMsg("Unknown entity type");

	//debug << "End cEntityMngr::CreateEntity ()" 
	//      << endl;
	return true;
}

/////////////////////////////////////////////////
bool cEntityMngr::SetEntityPosition(const EntityID& id, const sVector3d& pos)
{
	//debug << "Begin cEntityMngr::SetEntityPosition (" 
	//      << id.GetStr() << " => " << pos.m_x << ", "
	//      << pos.m_y << ", " << pos.m_z << ")" << endl;
	EntityIter it = m_entities.find(id);
	if (it == m_entities.end())
		return false;

	m_entities[id]->SetPosition(pos);
	//debug << "End cEntityMngr::SetEntityPosition ()" << endl;
	return true;
}

/////////////////////////////////////////////////
bool cEntityMngr::SetEntityRotation(const EntityID& id, const sVector3d& rot)
{
	//debug << "Begin cEntityMngr::SetEntityRotation (" 
	//      << id.GetStr() << " => " << rot.m_x << ", "
	//      << rot.m_y << ", " << rot.m_z << ")" << endl;
	EntityIter it = m_entities.find(id);
	if (it == m_entities.end())
		return false;

	m_entities[id]->SetRotation(rot);
	//debug << "End cEntityMngr::SetEntityRotation ()" << endl;
	return true;
}

/////////////////////////////////////////////////
bool cEntityMngr::SetEntityScale(const EntityID& id, const sVector3d& scl)
{
	//debug << "Begin cEntityMngr::SetEntityScale(" 
	//      << id.GetStr() << " => " << scl.m_x << ", "
	//      << scl.m_y << ", " << scl.m_z << ")" << endl;
	EntityIter it = m_entities.find(id);
	if (it == m_entities.end())
		return false;

	m_entities[id]->SetScale(scl);
	//debug << "End cEntityMngr::SetEntityScale()" << endl;
	return true;
}

/////////////////////////////////////////////////
bool cEntityMngr::SetEntityModel(const EntityID& id, const GraphicID& model)
{
	//debug << "Begin cEntityMngr::SetEntityModel(" 
	//      << id.GetStr() << ", " << model.GetStr() << ")" << endl;
	EntityIter it = m_entities.find(id);
	if(it == m_entities.end())
		return false;

	iRenderable* entity = 
		dynamic_cast<iRenderable*>(it->second);
	entity->SetModel(model);
	//debug << "End cEntityMngr::SetEntityModel()" << endl;
	return true;
}

/////////////////////////////////////////////////
bool cEntityMngr::SetEntityHeightmap(const EntityID& id,
									 const char* filename)
{
	#ifdef MY_DEBUG
		debug << "cEntityMngr::SetEntityHeightmap started" << endl;
	#endif
	EntityIter it = m_entities.find(id);
	if (it == m_entities.end())
	{
		ErrorMsg("HM setting, entity not found");
		return false;
	}

	cTerrainEntity* entity = dynamic_cast<cTerrainEntity*>(m_entities[id]);
	entity->ReadCustomHMFile(filename);

	ifstream fin(filename);


	fin.close();

	return true;
}

/////////////////////////////////////////////////
bool cEntityMngr::SetEntityCollider(const EntityID& id,
									const char* colType,
									sVector3d& dataStart,
									sVector3d& dataEnd)
{
	string s = colType;
	//debug << "cEntityMngr::SetEntityCollider colType = " << colType << endl;
	EntityIter it = m_entities.find(id);
	if (it == m_entities.end())
	{
		ErrorMsg("Collider setting, entity not found");
		return false;
	}
	dataStart *= m_entities[id]->GetScale();
	dataEnd *= m_entities[id]->GetScale();

	if (s == "Cylinder")
	{
		//debug << "Attaching colider to: " << id.GetStr() << endl;
		cBoundingCylinder* cyl = new cBoundingCylinder(dataStart.m_x, 
												   dataStart.m_y, 
												   &(m_entities[id]->GetPhysPointer()));
		g_eventMngr.PostEvent(new cEvent(AttachCollider, id.GetID(), cyl));
	}
	else if (s == "AABox")
	{
		//debug << "Attaching colider to: " << id.GetStr() << endl;
		cBoundingBox* box = new cBoundingBox(dataStart, 
											 dataEnd, 
											 &(m_entities[id]->GetPhysPointer()));
		g_eventMngr.PostEvent(new cEvent(AttachCollider, id.GetID(), box));
	}
	else if (s == "Sphere")
	{
		//debug << "Attaching colider to: " << id.GetStr() << endl;
		cBoundingSphere* sphere = new cBoundingSphere(dataStart.m_x, 
											 &(m_entities[id]->GetPhysPointer()));
		g_eventMngr.PostEvent(new cEvent(AttachCollider, id.GetID(), sphere));
	}
	return true;
}

/////////////////////////////////////////////////
void cEntityMngr::UpdateEntities(float dt)
{  
	//debug << "Begin cEntityMngr::UpdateEntities(" << dt << ")" << endl;
	for(EntityIter it = m_entities.begin();
		it != m_entities.end(); ++it)
	{
		iMoveable* entity = 
			dynamic_cast<iMoveable*>(it->second);

		if(entity)
			entity->Update(dt);
	}

	//debug << "End cEntityMngr::UpdateEntities()" << endl;
}

/////////////////////////////////////////////////
void cEntityMngr::DrawEntities()
{
	//debug << "Begin cEntityMngr::DrawEntities()" << endl;

	for(EntityIter it = m_entities.begin();
		it != m_entities.end(); ++it)
	{
		iRenderable* entity = 
			dynamic_cast<iRenderable*>(it->second);

		if(entity)
			entity->Draw();
	}

	//debug << "End cEntityMngr::DrawEntities()" << endl;
}


