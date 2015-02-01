#include <string>
using std::string;

#include "Debug.h"
#include "Camera.h"

/////////////////////////////////////////////////
//constants
const static sVector3d DEFAULT_POS(0, 0, 0);
const static sVector3d DEFAULT_LOOK(0, 0, 1);
const static sVector3d DEFAULT_UP(0, 1, 0);
const static sVector3d DEFAULT_RIGHT(1, 0, 0);

/////////////////////////////////////////////////
cCamera::cCamera ()
: m_pos (DEFAULT_POS), m_look (DEFAULT_LOOK), m_up (DEFAULT_UP), 
m_right (DEFAULT_RIGHT), m_valid(false), m_flying(false),
WalkEvent("CAMERA_WALK_EVENT"),
StrafeEvent("CAMERA_STRAFE_EVENT"),
FlyEvent("CAMERA_FLY_EVENT"),
PitchEvent("CAMERA_PITCH_EVENT"),
YawEvent("CAMERA_YAW_EVENT"),
RollEvent("CAMERA_ROLL_EVENT"),
MoveEvent("CAMERA_MOVE_EVENT"),
RotateEvent("CAMERA_ROTATE_EVENT")
{
	g_eventMngr.RegisterListener(WalkEvent, this);
	g_eventMngr.RegisterListener(StrafeEvent, this);
	g_eventMngr.RegisterListener(FlyEvent, this);
	g_eventMngr.RegisterListener(PitchEvent, this);
	g_eventMngr.RegisterListener(YawEvent, this);
	g_eventMngr.RegisterListener(RollEvent, this);
	g_eventMngr.RegisterListener(MoveEvent, this);
	g_eventMngr.RegisterListener(RotateEvent, this);
}

/////////////////////////////////////////////////
bool cCamera::Init()//LPDIRECT3DDEVICE9 dev)
{
	#ifdef MY_DEBUG
		debug << "Begin cCamera::Init()" << endl;
	#endif

	m_look = DEFAULT_LOOK;
	m_up = DEFAULT_UP; 
	m_right = DEFAULT_RIGHT;

	#ifdef MY_DEBUG
		debug << "End cCamera::Init()" << endl;
	#endif
	return true;
}

/////////////////////////////////////////////////
void cCamera::Reset()
{
	//debug << "Begin cCamera::Reset()" << endl;

	m_look = DEFAULT_LOOK;
	m_up = DEFAULT_UP; 
	m_right = DEFAULT_RIGHT;

	m_valid = false;
	//debug << "End cCamera::Reset()" << endl;
}

/////////////////////////////////////////////////
void cCamera::HandleEvent(cEvent* e)
{
	//debug << "Begin cCamera::HandleEvent()" << endl;

	EventID id = e->GetEventID();
	if(id == WalkEvent)
		Walk(e->GetParam(0).m_asFloat);

	else if(id == StrafeEvent)
		Strafe(e->GetParam(0).m_asFloat);

	else if(id == FlyEvent)
		Fly(e->GetParam(0).m_asFloat);

	else if(id == PitchEvent)
		Pitch(e->GetParam(0).m_asFloat);

	else if(id == YawEvent)
		Yaw(e->GetParam(0).m_asFloat);

	else if(id == RollEvent)
		Roll(e->GetParam(0).m_asFloat);

	else if(id == MoveEvent)
	{
		sVector3d pos(
			e->GetParam(0).m_asFloat,
			e->GetParam(1).m_asFloat,
			e->GetParam(2).m_asFloat);

		SetPosition(pos);
	}

	else if(id == RotateEvent)
	{
		sVector3d rot(
			e->GetParam(0).m_asFloat,
			e->GetParam(1).m_asFloat,
			e->GetParam(2).m_asFloat);

		SetRotation(rot);
	}

	//debug << "End cCamera::HandleEvent()" << endl;
}

/////////////////////////////////////////////////
void cCamera::SetPosition(const sVector3d& pos)
{
	//debug << "Begin cCamera::SetPosition()" << endl;
	m_pos = pos;
	m_valid = false;
	//debug << "End cCamera::SetPosition()" << endl;
}

/////////////////////////////////////////////////
void cCamera::SetRotation(const sVector3d& r)
{
	//debug << "Begin cCamera::SetRotation()" << endl;
	Reset();
	Pitch(r.m_x);
	Yaw(r.m_y);
	Roll(r.m_z);

	m_valid = false;
	//debug << "End cCamera::SetRotation()" << endl;
}

/////////////////////////////////////////////////
const D3DXMATRIX& cCamera::CalcViewMatrix()
{
	//debug << "Begin cCamera::CalcViewMatrix()" << endl;
	if(!m_valid)
	{
		// Keep camera's axes orthogonal to each other:
		D3DXVec3Normalize((D3DXVECTOR3*)&m_look, (const D3DXVECTOR3*)&m_look);
		D3DXVec3Cross((D3DXVECTOR3*)&m_up, (const D3DXVECTOR3*)&m_look, (const D3DXVECTOR3*)&m_right);
		D3DXVec3Normalize((D3DXVECTOR3*)&m_up, (const D3DXVECTOR3*)&m_up);
		D3DXVec3Cross((D3DXVECTOR3*)&m_right, (const D3DXVECTOR3*)&m_up, (const D3DXVECTOR3*)&m_look);
		D3DXVec3Normalize((D3DXVECTOR3*)&m_right, (const D3DXVECTOR3*)&m_right);

		// Build the view matrix:
		m_view(0, 0) = m_right.m_x;
		m_view(0, 1) = m_up.m_x;
		m_view(0, 2) = m_look.m_x;
		m_view(0, 3) = 0.0f;

		m_view(1, 0) = m_right.m_y;
		m_view(1, 1) = m_up.m_y;
		m_view(1, 2) = m_look.m_y;
		m_view(1, 3) = 0.0f;

		m_view(2, 0) = m_right.m_z;
		m_view(2, 1) = m_up.m_z;
		m_view(2, 2) = m_look.m_z;
		m_view(2, 3) = 0.0f;

		m_view(3, 0) = -D3DXVec3Dot((D3DXVECTOR3*)&m_right, (const D3DXVECTOR3*)&m_pos);
		m_view(3, 1) = -D3DXVec3Dot((D3DXVECTOR3*)&m_up, (const D3DXVECTOR3*)&m_pos);
		m_view(3, 2) = -D3DXVec3Dot((D3DXVECTOR3*)&m_look, (const D3DXVECTOR3*)&m_pos);
		m_view(3, 3) = 1.0f;

		m_valid = true;
	}

	//debug << "End cCamera::CalcViewMatrix()" << endl;
	return m_view;
}

/////////////////////////////////////////////////
void cCamera::Walk(float units)
{
	if (m_flying)
		m_pos += m_look * units;

	// move only on xz plane for land object
	else
		m_pos += sVector3d(m_look.m_x, 0.0f, m_look.m_z) * units;

	m_valid = false;
}

/////////////////////////////////////////////////
void cCamera::Strafe(float units)
{
	if (m_flying)
		m_pos += m_right * units;

	// move only on xz plane for land object
	else
		m_pos += sVector3d(m_right.m_x, 0.0f, m_right.m_z) * units;

	m_valid = false; 
}

/////////////////////////////////////////////////
void cCamera::Fly(float units)
{
	if (m_flying)
		m_pos += m_up * units;

	m_valid = false;
}

/////////////////////////////////////////////////
void cCamera::Pitch(float angle)
{
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, (const D3DXVECTOR3*)&m_right, angle);

	// rotate _up and _look around _right vector
	D3DXVec3TransformCoord((D3DXVECTOR3*)&m_up, (const D3DXVECTOR3*)&m_up, &T);
	D3DXVec3TransformCoord((D3DXVECTOR3*)&m_look, (const D3DXVECTOR3*)&m_look, &T);

	m_valid = false;
}

/////////////////////////////////////////////////
void cCamera::Yaw(float angle)
{
	D3DXMATRIX T;

	// rotate around own up vector for aircraft
	if (m_flying)
		D3DXMatrixRotationAxis(&T, (const D3DXVECTOR3*)&m_up, angle);

	// rotate around world y (0, 1, 0) always for land object
	else
		D3DXMatrixRotationY(&T, angle);

	// rotate _right and _look around _up or y-axis
	D3DXVec3TransformCoord((D3DXVECTOR3*)&m_right,(const D3DXVECTOR3*)&m_right, &T);
	D3DXVec3TransformCoord((D3DXVECTOR3*)&m_look,(const D3DXVECTOR3*)&m_look, &T);

	m_valid = false;
}

/////////////////////////////////////////////////
void cCamera::Roll(float angle)
{
	// only roll for aircraft type
	if (m_flying)
	{
		D3DXMATRIX T;
		D3DXMatrixRotationAxis(&T, (const D3DXVECTOR3*)&m_look, angle);

		// rotate _up and _right around _look vector
		D3DXVec3TransformCoord((D3DXVECTOR3*)&m_right, (const D3DXVECTOR3*)&m_right, &T);
		D3DXVec3TransformCoord((D3DXVECTOR3*)&m_up, (const D3DXVECTOR3*)&m_up, &T);

		m_valid = false;
	}
}