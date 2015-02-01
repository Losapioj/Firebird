#ifndef __BOUNDINGVOLUME_H__
#define __BOUNDINGVOLUME_H__

#include "PhysicalData.h"

/////////////////////////////////////////////////
//results for collision detection
const int NO_COLLISION = 0;
const int NO_WALK = 1;
const int WALK_ON_TOP = 2;

/////////////////////////////////////////////////
enum VolumeType {SPHERE, CYLINDER, AABOX,
COMPOSITE, NONE = 0xffffffff};

/////////////////////////////////////////////////
class iCollidable
{
public:
	virtual VolumeType GetVolumeType() const = 0;

	virtual bool vsSphere(const iCollidable*) const = 0;
	virtual bool vsAABox(const iCollidable*) const = 0;
	virtual bool vsCylinder(const iCollidable*) const = 0;
};

/////////////////////////////////////////////////
class cBoundingVolume : public iCollidable
{
protected:
	VolumeType m_type;
	sPhysicalData* m_object;

public:
	cBoundingVolume(VolumeType v, sPhysicalData* p)
		: m_type(v), m_object(p) {}
	virtual VolumeType GetVolumeType() const
	{  return m_type;  }

	virtual const sVector3d& GetPosition() const
	{  return (m_object->m_position);  }
	virtual const sVector3d& GetRotation() const
	{  return (m_object->m_rotation);  }
	virtual const sVector3d& GetScale() const
	{  return (m_object->m_scale);  }
};

/////////////////////////////////////////////////
class cBoundingSphere : public cBoundingVolume
{
protected:
	float m_radius;

public:
	cBoundingSphere(float r, sPhysicalData* p)
		: cBoundingVolume(SPHERE, p), m_radius(r) {}

	float GetRadius() const
	{  return m_radius;  }

	virtual bool vsSphere(const iCollidable*) const;
	virtual bool vsAABox(const iCollidable*) const;
	virtual bool vsCylinder(const iCollidable*) const;
};

/////////////////////////////////////////////////
class cBoundingCylinder : public cBoundingVolume
{
protected:
	float m_radius, m_height;

public:
	cBoundingCylinder(float r, float h, sPhysicalData* p)
		: cBoundingVolume(CYLINDER, p), m_radius(r),
		m_height(h)
	{}

	float GetRadius() const
	{  return m_radius;  }
	float GetHeight() const
	{  return m_height;  }

	virtual bool vsSphere(const iCollidable*) const;
	virtual bool vsAABox(const iCollidable*) const;
	virtual bool vsCylinder(const iCollidable*) const;
};

/////////////////////////////////////////////////
class cBoundingBox : public cBoundingVolume
{
protected:
	sVector3d m_min, m_max;

public:
	cBoundingBox(const sVector3d& min,
		const sVector3d& max, 
		sPhysicalData* p)
		: cBoundingVolume(AABOX, p), m_min(min), m_max(max)
	{}

	const sVector3d& GetMinCorner() const
	{  return m_min;  }
	const sVector3d& GetMaxCorner() const
	{  return m_max;  }

	virtual bool vsSphere(const iCollidable*) const;
	virtual bool vsAABox(const iCollidable*) const;
	virtual bool vsCylinder(const iCollidable*) const;
};

/////////////////////////////////////////////////
#endif