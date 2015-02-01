#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "BoundingVolume.h"

/////////////////////////////////////////////////
extern const int NO_COLLISION;
extern const int NO_WALK;
extern const int WALK_ON_TOP;

/////////////////////////////////////////////////
class iRenderable
{
public:
	virtual void Draw() = 0;

	virtual const GraphicID& GetModel() = 0;
	virtual void SetModel(const GraphicID&) = 0;
};

/////////////////////////////////////////////////
class iMoveable
{
public:
	virtual void Update(float dt) = 0;
};

/////////////////////////////////////////////////
class iEntity
{
public:
	virtual ~iEntity() {}

	virtual const EntityID& GetID() = 0;

	virtual void SetPosition(const sVector3d&) = 0;
	virtual const sVector3d& GetPosition() = 0;

	virtual void SetRotation(const sVector3d&) = 0;
	virtual const sVector3d& GetRotation() = 0;

	virtual void SetScale(const sVector3d&) = 0;
	virtual const sVector3d& GetScale() = 0;
};

/////////////////////////////////////////////////
class cEntity : public iEntity
{
protected:
	EntityID m_id;

	sPhysicalData m_phys;
	D3DXMATRIX m_world;

	EventID EntityCreated;

public:
	cEntity(const EntityID& id)
		: m_id(id)
	{  m_phys.m_scale = sVector3d(1,1,1);  }

	virtual ~cEntity() {}

	virtual const EntityID& GetID()
	{  return m_id;  }

	virtual void SetPosition(const sVector3d& pos)
	{  m_phys.m_position = pos;  }
	virtual const sVector3d& GetPosition()
	{  return m_phys.m_position;  }

	virtual void SetRotation(const sVector3d& rot)
	{  m_phys.m_rotation = rot;  }
	virtual const sVector3d& GetRotation()
	{  return m_phys.m_rotation;  }

	virtual void SetScale(const sVector3d& scl)
	{  m_phys.m_scale = scl;  }
	virtual const sVector3d& GetScale()
	{  return m_phys.m_scale;  }

	virtual sPhysicalData& GetPhysPointer()
	{  return m_phys;  }
};

/////////////////////////////////////////////////
#endif // __ENTITY_H__