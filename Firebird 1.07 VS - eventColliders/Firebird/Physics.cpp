#include "Debug.h"
#include "Physics.h"

/////////////////////////////////////////////////
extern const int NO_COLLISION;
extern const int NO_WALK;
extern const int WALK_ON_TOP;

/////////////////////////////////////////////////
cPhysics::cPhysics()
: m_worldGravityVel(1), m_worldMaxFallVel(1),
m_worldJumpVel(1), m_worldFallDPS(1),
EntityAttachCollidable("ENTITY_ATTACH_COLLIDABLE_EVENT"),
EntityUpdated("ENTITY_UPDATED_EVENT"),
FallEvent("PHYSICS_FALL_EVENT"),
JumpEvent("PHYSICS_JUMP_EVENT"),
LevelInitPhys("LEVEL_INIT_PHYSICS_EVENT")
{
	g_eventMngr.RegisterListener(LevelInitPhys, this);
	g_eventMngr.RegisterListener(EntityAttachCollidable, this);
	g_eventMngr.RegisterListener(EntityUpdated, this);
	g_eventMngr.RegisterListener(FallEvent, this);
	g_eventMngr.RegisterListener(JumpEvent, this);
}

/////////////////////////////////////////////////
cPhysics::~cPhysics()
{
	#ifdef MY_DEBUG
		debug << "~cPhysics start" << endl;
	#endif

	while (!m_entities.empty())
	{
		PD_ITER it = m_entities.begin();
		//#ifdef MY_DEBUG
		//	debug << "iter begin" << &it << " iter end" << &m_entities.end() << endl;
		//#endif
		if(it->second != NULL)
		{
			//debug << "deleting..." << it->second->GetVolumeType()<< endl;
			delete it->second;
			//debug << "deleted" << endl;
		}
		//debug << "erasing..." << endl;
		m_entities.erase(it);
		//debug << "erased" << endl;
	}

	#ifdef MY_DEBUG
		debug << "~cPhysics end" << endl;
	#endif
}

/////////////////////////////////////////////////
void cPhysics::HandleEvent(cEvent* e)
{
	EventID eid = e->GetEventID();
	if(eid == EntityAttachCollidable)
	{
		EntityID entity;
		entity.SetID(e->GetParam(0).m_asInt);
		iCollidable* coll = (iCollidable*)e->GetParam(1).m_asPointer;
		RegisterEntity(entity, coll);
		e->SetIsComplete(true);
	}
	else if(eid == EntityUpdated)
	{
		EntityID entity, colWith;
		entity.SetID(e->GetParam(0).m_asInt);
		e->SetParam(5, VerifyEntity(entity, &colWith));
		e->SetParam(4, &colWith);
	}
	else if(eid == LevelInitPhys)
	{
		m_worldGravityVel = e->GetParam(0).m_asFloat;
		m_worldMaxFallVel= e->GetParam(1).m_asFloat;
		m_worldJumpVel= e->GetParam(2).m_asFloat;
		m_worldFallDPS= e->GetParam(3).m_asInt;
	}
	else // a model is calling me, get their physics data struct
	{
		sPhysicalData* physData = (sPhysicalData*)( e->GetParam(0).m_asPointer );

		if(eid == FallEvent)
		{
			physData->m_verticalSpeed -= m_worldGravityVel * m_dt;
//			#ifdef MY_DEBUG
//				debug << "Physics vert speed: " << physData->m_verticalSpeed << endl;
//			#endif
			if (physData->m_verticalSpeed <= m_worldMaxFallVel)
			{
				physData->m_verticalSpeed = m_worldMaxFallVel;
			}
//			#ifdef MY_DEBUG
//				debug << "Physics vert speed: " << physData->m_verticalSpeed << endl;
//			#endif
			float fallDist = physData->m_verticalSpeed * m_dt * physData->m_gravityMod;
//			float dist = EntityFall(physData->m_verticalSpeed, 
//									physData->m_gravityMod);

			physData->m_position.m_y += fallDist;

			//get data to check for collision
			EntityID eid;
			eid.SetID(e->GetParam(1).m_asInt);

			if (physData->m_position.m_y <= physData->m_groundHeight ) //HIT GROUND
			{
				physData->m_position.m_y = physData->m_groundHeight;
				if (physData->m_fallTimeIgnored != -1 && 
					physData->m_fallTime > physData->m_fallTimeIgnored) //calc fall damage
				{
					int dmg = FallToDmg(physData->m_fallTime, 
										physData->m_fallTimeIgnored);
					e->SetParam(0, dmg);
				}
				else //return 0 for damage
					e->SetParam(0, 0);
				physData->m_verticalSpeed = 0;
				physData->m_onGround = true;
			}
			else //still falling to heightmap, check for model collision
			{
				EntityID entity;
				entity.SetID(e->GetParam(1).m_asInt);
				if (VerifyEntity(entity, NULL) == NO_WALK) //fell into model
				{
					physData->m_position.m_y -= fallDist;
					physData->m_verticalSpeed = 0;
					physData->m_onGround = true;

					if (physData->m_fallTimeIgnored != -1 && 
						physData->m_fallTime > physData->m_fallTimeIgnored) //calc fall damage
					{
						int dmg = FallToDmg(physData->m_fallTime, 
											physData->m_fallTimeIgnored);
						e->SetParam(0, dmg);
					}
				}
				else 
					e->SetParam(0, 0);
			}
		}
		else if(eid == JumpEvent && physData->m_onGround)
		{
			physData->m_verticalSpeed = m_worldJumpVel;
			physData->m_position.m_y += m_worldJumpVel * m_dt;
			physData->m_onGround = false;
		}
	}
}

/////////////////////////////////////////////////
bool cPhysics::RegisterEntity(EntityID id, iCollidable* coll)
{

/*	std::pair <PD_ITER, PD_ITER> iterReturn = m_entities.equal_range(id);
	for( PD_ITER it = iterReturn.first; it != iterReturn.second; it++)
	{
		it->second = coll;
	}*/
	//if(it != m_entities.end())
	//	return false;

	std::pair <EntityID, iCollidable*> addMe(id, coll);
	m_entities.insert(addMe);
//	m_entities[id] = coll;

	return true;
}

/////////////////////////////////////////////////
int cPhysics::VerifyEntity(EntityID id, const EntityID* colWithId)
{
	PD_ITER it = m_entities.find(id);
	if(it == m_entities.end())
		return NO_COLLISION;

	
	for(PD_ITER it2 = m_entities.begin(); it2 != m_entities.end(); it2++)
	{
		bool collision = false;
		if (it->first != it2->first)
		{
			if(it2->second->GetVolumeType() == AABOX)
			{
				collision = it->second->vsAABox(it2->second);
			}
			else if(it2->second->GetVolumeType() == CYLINDER)
			{
				collision = it->second->vsCylinder(it2->second);
			}
			else if(it2->second->GetVolumeType() == SPHERE)
			{
				collision = it->second->vsSphere(it2->second);
			}

			if(collision)
			{
				//debug << "Collision" << endl;
				return NO_WALK;
				colWithId = &it->first;
			}
		}
	}

	colWithId = NULL;
	return NO_COLLISION;
}

/////////////////////////////////////////////////
/*int cPhysics::VerifyEntityAgainst(EntityID id, EntityID testVsId)
{
	//find all colliders associated with first object
	pair<PD_ITER, PD_ITER> rangePair1;
	rangePair = m_entities.equal_range(id);

	//find all colliders associated with second object
	pair<PD_ITER, PD_ITER> rangePair2;
	rangePair = m_entities.equal_range(testVsId);

	

	//test these colliders only for collision
		for(PD_ITER it2 = rangePair2.first; it2 != rangePair2.second; it2++)
		{
			bool collision = false;
			if (startIter->first != endIter->first)
			{
				if(it->second->GetVolumeType() == AABOX)
				{
					collision = ->second->vsAABox(it->second);
				}
				else if(it->second->GetVolumeType() == CYLINDER)
				{
					collision = ->second->vsCylinder(it->second);
				}
				else if(it->second->GetVolumeType() == SPHERE)
				{
					collision = ->second->vsSphere(it->second);
				}

				if(collision)
				{
					//debug << "Collision" << endl;
					return NO_WALK;
				}
			}
		}

	return NO_COLLISION;
}*/

/////////////////////////////////////////////////
int cPhysics::FallToDmg(float fallTime, int timeIgnored)
{
	int damage = int( (fallTime - timeIgnored) * m_worldFallDPS );
	return damage;
}