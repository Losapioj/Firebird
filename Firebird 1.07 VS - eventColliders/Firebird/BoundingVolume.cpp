#include "Debug.h"
#include "BoundingVolume.h"

/////////////////////////////////////////////////
bool cBoundingSphere::vsSphere(const iCollidable* target) const
{
	if (target->GetVolumeType() != SPHERE)
		return false;
	cBoundingSphere* sphere = (cBoundingSphere*) target;

	sVector3d diff = sphere->GetPosition() - m_object->m_position;
	return diff.NormSquared() < (m_radius * m_radius) + 
		(sphere->GetRadius() * sphere->GetRadius());
}

/////////////////////////////////////////////////
bool cBoundingSphere::vsAABox(const iCollidable* target) const
{
	if (target->GetVolumeType() != AABOX)
		return false;
	cBoundingBox* box = (cBoundingBox*) target;

	sVector3d boxMin = box->GetMinCorner() + box->GetPosition(),
		boxMax = box->GetMaxCorner() + box->GetPosition(),
		sphPos = GetPosition();
	float sphRad = GetRadius();

	sVector3d delta;

	if(sphPos.m_x < boxMin.m_x)
		delta.m_x = sphPos.m_x - boxMin.m_x;
	else if(sphPos.m_x > boxMax.m_x)
		delta.m_x = sphPos.m_x - boxMax.m_x;

	if(sphPos.m_y < boxMin.m_y)
		delta.m_y = sphPos.m_y - boxMin.m_y;
	else if(sphPos.m_y > boxMax.m_y)
		delta.m_y = sphPos.m_y - boxMax.m_y;

	if(sphPos.m_z < boxMin.m_z)
		delta.m_z = sphPos.m_z - boxMin.m_z;
	else if(sphPos.m_z > boxMax.m_z)
		delta.m_z = sphPos.m_z - boxMax.m_z;

	return (delta.NormSquared() <= sphRad * sphRad);
}

/////////////////////////////////////////////////
bool cBoundingSphere::vsCylinder(const iCollidable* target) const
{
	if (target->GetVolumeType() != CYLINDER)
		return false;
	cBoundingCylinder* cyl = (cBoundingCylinder*) target;

	sVector3d sphPos = GetPosition();
	float sphRad = GetRadius();

	sVector3d cylPos = cyl->GetPosition();
	float cylHgt = cyl->GetHeight(),
		cylRad = cyl->GetRadius();

	sVector2d p1(sphPos.m_x, sphPos.m_z),
		p2(cylPos.m_x, cylPos.m_z),
		diff = p2 - p1;

	if (diff.NormSquared() > (cylRad * cylRad) +
		(sphRad * sphRad))
	{
		return false;
	}
	else if (sphPos.m_y + sphRad < cylPos.m_y)
	{
		return false;
	}
	else if (sphPos.m_y - sphRad > cylPos.m_y + cylHgt)
	{
		return false;
	}

	return true;
}

/////////////////////////////////////////////////
bool cBoundingCylinder::vsSphere(const iCollidable* target) const
{
	if (target->GetVolumeType() != SPHERE)
		return false;
	cBoundingSphere* sphere = (cBoundingSphere*) target;

	return sphere->vsCylinder(this);
}

/////////////////////////////////////////////////
bool cBoundingCylinder::vsAABox(const iCollidable* target) const
{
	if (target->GetVolumeType() != AABOX)
		return false;
	cBoundingBox* box = (cBoundingBox*) target;

	sVector3d boxMin = box->GetMinCorner() + box->GetPosition(),
		boxMax = box->GetMaxCorner() + box->GetPosition(),
		cylPos = GetPosition();
	float cylHgt = GetHeight(),
		cylRad = GetRadius();

	sVector3d delta;

	if (boxMax.m_y < cylPos.m_y)
		return false;
	if (boxMin.m_y > cylPos.m_y + cylHgt)
		return false;

	if(cylPos.m_x < boxMin.m_x)
		delta.m_x = cylPos.m_x - boxMin.m_x;
	else if(cylPos.m_x > boxMax.m_x)
		delta.m_x = cylPos.m_x - boxMax.m_x;

	if(cylPos.m_y < boxMin.m_y)
		delta.m_y = cylPos.m_y - boxMin.m_y;
	else if(cylPos.m_y > boxMax.m_y)
		delta.m_y = cylPos.m_y - boxMax.m_y;

	if(cylPos.m_z < boxMin.m_z)
		delta.m_z = cylPos.m_z - boxMin.m_z;
	else if(cylPos.m_z > boxMax.m_z)
		delta.m_z = cylPos.m_z - boxMax.m_z;

	if (delta.NormSquared() > cylRad * cylRad)
		return false;

	return true;
}

/////////////////////////////////////////////////
bool cBoundingCylinder::vsCylinder(const iCollidable* target) const
{
	if (target->GetVolumeType() != CYLINDER)
		return false;
	cBoundingCylinder* cyl = (cBoundingCylinder*) target;

	sVector3d pos1 = GetPosition(),
		pos2 = cyl->GetPosition();
	float rad1 = GetRadius(),
		hgt1 = GetHeight(),
		rad2 = cyl->GetRadius(),
		hgt2 = cyl->GetHeight();

	if (pos1.m_y + hgt1 < pos2.m_y)
		return false;
	if (pos2.m_y + hgt2 < pos1.m_y)
		return false;

	sVector2d pos1_2d(pos1.m_x, pos1.m_z),
		pos2_2d(pos2.m_x, pos2.m_z);

	sVector2d diff = pos2_2d - pos1_2d;
	if (diff.NormSquared() > (rad1 * rad1) +
		(rad2 * rad2))
		return false;

	return true;
}

/////////////////////////////////////////////////
bool cBoundingBox::vsSphere(const iCollidable* target) const
{
	if (target->GetVolumeType() != SPHERE)
		return false;
	cBoundingSphere* sphere = (cBoundingSphere*) target;

	return sphere->vsAABox(this);
}

/////////////////////////////////////////////////
bool cBoundingBox::vsAABox(const iCollidable* target) const
{
	if (target->GetVolumeType() != AABOX)
		return false;
	cBoundingBox* box = (cBoundingBox*) target;

	sVector3d min1 = GetMinCorner() + GetPosition(),
		max1 = GetMaxCorner() + GetPosition(),
		min2 = box->GetMinCorner() + box->GetPosition(),
		max2 = box->GetMaxCorner() + box->GetPosition();

	if(min1.m_x > max2.m_x)
		return false;
	if(min2.m_x > max1.m_x)
		return false;

	if(min1.m_y > max2.m_y)
		return false;
	if(min2.m_y > max1.m_y)
		return false;

	if(min1.m_z > max2.m_z)
		return false;
	if(min2.m_z > max1.m_z)
		return false;

	return true;
}

/////////////////////////////////////////////////
bool cBoundingBox::vsCylinder(const iCollidable* target) const
{
	if (target->GetVolumeType() != CYLINDER)
		return false;
	cBoundingCylinder* cyl = (cBoundingCylinder*) target;

	return cyl->vsAABox(this);
}
