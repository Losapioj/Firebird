#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "BoundingVolume.h"
#include "EventMngr.h"



/////////////////////////////////////////////////
class cPhysics : public iListener
{
private:
	typedef multimap <EntityID, iCollidable*> PD_LIST;
	typedef PD_LIST::iterator PD_ITER;

	typedef multimap <EntityID, iCollidable*> EVENT_LIST;
	typedef PD_LIST::iterator EVENT_ITER;

	PD_LIST m_entities;
	EventID LevelInitPhys;
	EventID EntityAttachCollidable;
	EventID EntityUpdated;
	EventID FallEvent;
	EventID JumpEvent;

	float m_dt;

	float m_worldGravityVel;
	float m_worldMaxFallVel;
	float m_worldJumpVel;
	int m_worldFallDPS;
	
public:
	cPhysics();
	~cPhysics();

	void HandleEvent(cEvent*);
	bool RegisterEntity(EntityID id, iCollidable* coll);
	//tests for any collisions
	int VerifyEntity(EntityID id, const EntityID* colWithId);
	//tests for collision with a specific entity
//	int VerifyEntityAgainst(EntityID id, EntityID testVsId);
	//tests for collision with event colliders
	int CheckEventColliders(EntityID id);

	inline void setDT(float dt)
	{	m_dt = dt;	}

	//returns distance falling for this frame
	//float EntityFall(float& curVel, float gravityModPct = 1);
	//checks fall time, calculates damage to apply
	int FallToDmg(float fallTime, int timeIgnored);
};

/////////////////////////////////////////////////
#endif // __PHYSICALDATA_H__