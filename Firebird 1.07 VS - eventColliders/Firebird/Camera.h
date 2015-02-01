#ifndef __CAMERA_H__
#define __CAMERA_H__

/*#include <d3dx9.h>
#include <fstream>

using std::ifstream;*/

#include "Util.h"
#include "EventMngr.h"

/////////////////////////////////////////////////
class cCamera : public iListener
{
private: 
	sVector3d m_right;
	sVector3d m_up;
	sVector3d m_look;
	sVector3d m_pos;

	D3DXMATRIX m_view;

	EventID WalkEvent;
	EventID StrafeEvent;
	EventID FlyEvent;
	EventID PitchEvent;
	EventID YawEvent;
	EventID RollEvent;
	EventID MoveEvent;
	EventID RotateEvent;
	//EventID UpdateEvent;

	bool m_flying, m_valid;

	// Movement methods
	void Walk(float);     // forward or back
	void Strafe(float);   // left or right
	void Fly(float);      // up or down

	// Rotation methods
	void Pitch(float);    // Right vector
	void Yaw(float);      // Up vector  
	void Roll(float);     // Look vector 

public:
	cCamera();
	~cCamera() {}

	bool Init();
	void Reset();
	const D3DXMATRIX& CalcViewMatrix();
	virtual void HandleEvent(cEvent*);

	void SetRotation(const sVector3d&);

	void GetPosition(sVector3d& pos)
	{  pos = m_pos;  }
	void SetPosition(const sVector3d&);

	void AllowFlying(bool fly)
	{  m_flying = fly;  }

	void GetUp(sVector3d* up)
	{  *up = m_up;  }
	void GetLook(sVector3d* look)
	{  *look = m_look;  }
	void GetRight(sVector3d* right)
	{  *right = m_right;  }
};

/////////////////////////////////////////////////
#endif // __CAMERA_H__