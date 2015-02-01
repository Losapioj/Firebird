#ifndef __ENTITYAVATAR_H__
#define __ENTITYAVATAR_H__

#include "Entity.h"

/////////////////////////////////////////////////
class cAvatarEntity : public cEntity,
	public iMoveable, 
	public iListener
{
protected:
	int m_forwardKey, m_backwardKey,
		m_leftwardKey, m_rightwardKey,
		m_jumpKey, m_sprintKey,
		m_crouchKey;
	bool m_forward, m_backward,
		 m_leftward, m_rightward,
		 m_jump, m_sprint,
		 m_crouch;
	int m_pitch, m_pan;

	bool onHeightmap;

	int currentHP, maxHP;

	EventID KeyPress;
	EventID KeyDown;
	EventID KeyUp;
	EventID MouseMove;

	EventID CameraMove;
	EventID CameraRotate;
	EventID TileWalkableRequest;
	EventID AttachCollider;
	EventID EntityUpdated;
	EventID TerrainHMWalkableRequest;
	EventID TerrainHeightRequest;

	EventID PhysHitGround;
	EventID PhysFall;
	EventID PhysJump;

public:
	cAvatarEntity(const EntityID&);
	virtual ~cAvatarEntity();

	virtual void Update(float);
	void HandleEvent(cEvent*);

	virtual void SetPosition(const sVector3d&);

	virtual void SetRotation(const sVector3d&);

	virtual void SetScale(const sVector3d&);
};

/////////////////////////////////////////////////
#endif // __ENTITYAVATAR_H__