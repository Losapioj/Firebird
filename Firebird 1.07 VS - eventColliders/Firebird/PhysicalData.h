#ifndef __PHYSICALDATA_H__
#define __PHYSICALDATA_H__

#include "Util.h"

/////////////////////////////////////////////////
struct sPhysicalData
{
public:
	sVector3d m_position,
		m_rotation,
		m_scale;

	//bool to check if you can jump right now
	bool m_onGround;
	//ground height saved here for cPhysics access
	float m_groundHeight;
	float m_fallTime;
	float m_verticalSpeed;

	//gravity modifier %, from 0-1
	//allows for featherfall magic
	float m_gravityMod;
	//ignore time for fall damage
	//if no fall damage, set to -1
	int m_fallTimeIgnored;
};

/////////////////////////////////////////////////

#endif 