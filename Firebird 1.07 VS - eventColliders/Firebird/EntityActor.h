#ifndef __ENTITYACTOR_H__
#define __ENTITYACTOR_H__

#include "Entity.h"

/////////////////////////////////////////////////
class cActorEntity : public cEntity,
	public iMoveable, 
	public iRenderable
{
protected:
	sVector3d m_dest;
	float m_delay;
	enum EntityState{WAITING, ROTATING, MOVING};
	EntityState m_state;
	int m_dir;
	vector<sPoint> m_path;

	GraphicID m_modelId;

//	EventID TileWalkableRequest;
//	EventID FindPathRequest;
	EventID RenderEvent;
	EventID EntityCreated;
	EventID EntityUpdated;

	void Wait();
	void Rotate(float);
	void Move(float);

public:
	cActorEntity(const EntityID&);
	virtual ~cActorEntity()  {}

	virtual void Update(float);
	virtual void Draw();

	virtual const GraphicID& GetModel()
	{  return m_modelId;  }
	virtual void SetModel(const GraphicID& id)
	{  m_modelId = id;  }
};

/////////////////////////////////////////////////
#endif // __ENTITYACTOR_H__