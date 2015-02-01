#ifndef __ENTITYMNGR_H__
#define __ENTITYMNGR_H__

#include <map>
using std::map;

#include "EntityActor.h"
#include "EntityAvatar.h"
#include "EntityTerrain.h"

/////////////////////////////////////////////////
class cEntityMngr : public iListener
{
	typedef map<EntityID, cEntity*> EntityList;
	typedef EntityList::iterator EntityIter;

	EntityList m_entities;

	EventID LoadEntity;
	EventID MoveEntity;
	EventID OrientEntity;
	EventID ScaleEntity;
	EventID AttachModel;
	EventID AttachHeightmap;
	EventID AttachCollider;
	EventID LoadCollidable;

public: 
	cEntityMngr();
	~cEntityMngr();

	virtual void HandleEvent(cEvent*);
	bool CreateEntity(const EntityID&,
		const char*);
	bool CreateEntity(const char*,
		const char*,
		const sVector3d&,
		const sVector3d&, 
		const sVector3d&,
		const char*);
	void UpdateEntities(float);
	void DrawEntities();

	bool SetEntityPosition(const EntityID&,
		const sVector3d&);
	bool SetEntityRotation(const EntityID&,
		const sVector3d&);
	bool SetEntityScale(const EntityID&,
		const sVector3d&);
	bool SetEntityModel(const EntityID&,
		const GraphicID&);
	bool SetEntityHeightmap(const EntityID&,
		const char*);
	bool SetEntityCollider(const EntityID& id,
						   const char* colType,
						   sVector3d& dataStart,
						   sVector3d& dataEnd);
};

/////////////////////////////////////////////////
#endif // __ENTITYMNGR_H__