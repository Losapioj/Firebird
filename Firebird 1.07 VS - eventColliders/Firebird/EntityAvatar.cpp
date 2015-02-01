
#include "debug.h"
#include "EventMngr.h"
#include "EntityAvatar.h"

/////////////////////////////////////////////////
extern const int NO_COLLISION;
extern const int NO_WALK;
extern const int WALK_ON_TOP;

/////////////////////////////////////////////////
const static int AVATAR_HEIGHT = 5;
const static float CROUCH_HEIGHT_MUL = 0.75f;
const static float DEFAULT_ROT_SPD = 0.7f;
const static float DEFAULT_FORWARD_SPD = 10.0f;
const static float DEFAULT_BACKWARD_SPD = 5.0f;
const static float DEFAULT_STRAFE_SPD = 7.0f;
const static float DEFAULT_SPRINT_MUL = 2.0f;

const static float MAX_WALK_ONTO_HEIGHT = 0.5f;

//default keys
const static int DEFAULT_SPRINT_KEY = 0x2A; //LeftShift key
const static int DEFAULT_FORWARD_KEY = 0x11; //W key
const static int DEFAULT_LEFTWARD_KEY = 0x1E; //A key
const static int DEFAULT_BACKWARD_KEY = 0x1F; //S key
const static int DEFAULT_RIGHTWARD_KEY = 0x20; //D key
const static int DEFAULT_JUMP_KEY = 0x39; //space key
const static int DEFAULT_CROUCH_KEY = 0x1D; // LeftControl key

const static float DEFAULT_BOUND_CYL_RAD = 2.0f;
const static float DEFAULT_BOUND_CYL_HEIGHT = 6.0f;

/////////////////////////////////////////////////
cAvatarEntity::cAvatarEntity(const EntityID& id)
							 : cEntity(id), 
							 m_forwardKey(DEFAULT_FORWARD_KEY), m_backwardKey(DEFAULT_BACKWARD_KEY),
							 m_leftwardKey(DEFAULT_LEFTWARD_KEY), m_rightwardKey(DEFAULT_RIGHTWARD_KEY),
							 m_jumpKey(DEFAULT_JUMP_KEY), m_sprintKey(DEFAULT_SPRINT_KEY),
							 m_crouchKey(DEFAULT_CROUCH_KEY),
							 m_forward(false), m_backward(false), 
							 m_leftward(false),m_rightward(false), 
							 m_jump(false), m_sprint(false),
							 m_crouch(false),
							 m_pitch(0), m_pan(0),
							 KeyPress("KEY_PRESS_EVENT"),
							 KeyDown("KEY_DOWN_EVENT"),
							 KeyUp("KEY_UP_EVENT"),
							 MouseMove("MOUSE_MOVE_EVENT"),
							 CameraMove("CAMERA_MOVE_EVENT"),
							 CameraRotate("CAMERA_ROTATE_EVENT"),
							 TerrainHMWalkableRequest("TERRAIN_CHECK_WALKABLE_REQUEST"),
							 TerrainHeightRequest("TERRAIN_GET_HEIGHT_REQUEST"),
							// AttachCollider("ENTITY_ATTACH_COLLIDABLE_EVENT"),
							 EntityUpdated("ENTITY_UPDATED_EVENT"),
							 PhysFall("PHYSICS_FALL_EVENT"),
							 PhysJump("PHYSICS_JUMP_EVENT")
{
	#ifdef MY_DEBUG
		debug << "Begin cAvatarEntity::Constructor()" << endl;
	#endif
	g_eventMngr.RegisterListener(KeyPress, this);
	g_eventMngr.RegisterListener(KeyDown, this);
	g_eventMngr.RegisterListener(KeyUp, this);
	g_eventMngr.RegisterListener(MouseMove, this);

	onHeightmap = true;
	/*cBoundingCylinder* cyl = new cBoundingCylinder(DEFAULT_BOUND_CYL_RAD, 
												   DEFAULT_BOUND_CYL_HEIGHT, 
												   &m_phys);
	g_eventMngr.PostEvent(new cEvent(AttachCollider, m_id.GetID(), cyl));*/

	m_phys.m_gravityMod = 1;
	m_phys.m_fallTime = 0;
	m_phys.m_fallTimeIgnored = 1;
	m_phys.m_verticalSpeed = 0;

	#ifdef MY_DEBUG
		debug << "End cAvatarEntity::Constructor()" << endl;
	#endif
}

/////////////////////////////////////////////////
cAvatarEntity::~cAvatarEntity()
{}

/////////////////////////////////////////////////
void cAvatarEntity::Update(float dt)
{
	//debug << "Begin cAvatarEntity::Update()" << endl;

	if(m_pitch || m_pan)
	{
		if (m_pitch != 0)
		{
			m_phys.m_rotation.m_x += (DEFAULT_ROT_SPD * m_pitch * dt);
			if (m_phys.m_rotation.m_x < -HALF_PI)
				m_phys.m_rotation.m_x = -HALF_PI;
			else if (m_phys.m_rotation.m_x > HALF_PI)
				m_phys.m_rotation.m_x = HALF_PI;
			m_pitch = 0;
		}
		if (m_pan != 0)
		{
			m_phys.m_rotation.m_y += (DEFAULT_ROT_SPD * m_pan * dt);
			while (m_phys.m_rotation.m_y < 0)
				m_phys.m_rotation.m_y += TWO_PI;
			while (m_phys.m_rotation.m_y > TWO_PI)
				m_phys.m_rotation.m_y -= (TWO_PI);
			m_pan = 0;
		}

		g_eventMngr.PostEvent(
			new cEvent(CameraRotate, 
			sVarient(m_phys.m_rotation.m_x), 
			sVarient(m_phys.m_rotation.m_y), 
			sVarient(m_phys.m_rotation.m_z)));
	}

	// Amount entity is moving
	float dx = 0.0f;
	float dz = 0.0f;

	if(m_forward)
	{
		dx += (DEFAULT_FORWARD_SPD * dt * sin(m_phys.m_rotation.m_y));
		dz += (DEFAULT_FORWARD_SPD * dt * cos(m_phys.m_rotation.m_y));
		m_forward = false;
	}
	else if(m_backward)  
	{
		dx -= (DEFAULT_BACKWARD_SPD * dt * sin(m_phys.m_rotation.m_y));
		dz -= (DEFAULT_BACKWARD_SPD * dt * cos(m_phys.m_rotation.m_y));
		m_backward = false;
	}

	if(m_leftward)  
	{
		dx -= (DEFAULT_STRAFE_SPD * dt * cos (m_phys.m_rotation.m_y));
		dz += (DEFAULT_STRAFE_SPD * dt * sin (m_phys.m_rotation.m_y));
		m_leftward = false;
	}
	else if(m_rightward) 
	{
		dx += (DEFAULT_STRAFE_SPD * dt * cos (m_phys.m_rotation.m_y));
		dz -= (DEFAULT_STRAFE_SPD * dt * sin (m_phys.m_rotation.m_y));
		m_rightward = false;
	}

	//if sprinting mul by modifier
	if(m_sprint)
	{
		dx *= DEFAULT_SPRINT_MUL;
		dz *= DEFAULT_SPRINT_MUL;
	}

	if (m_jump && m_phys.m_onGround)
	{
#ifdef MY_PROFILE
			debug << "\t\tcAvatarEntity::Update-> JUMPING NOW" << endl;
			debug << "POSITION: " << m_phys.m_position.m_x << " " 
								  << m_phys.m_position.m_y << " " 
								  << m_phys.m_position.m_z << endl;
#endif
		g_eventMngr.SendEvent( &cEvent(PhysJump, &m_phys, m_id.GetID()) );
		m_jump = false;
	}

	// New position, used to check if valid.
	float newX = m_phys.m_position.m_x + dx;
	float newZ = m_phys.m_position.m_z + dz;

//	cEvent checkHMWalkable(TerrainHMWalkableRequest, newX, newZ);
//	g_eventMngr.SendEvent(&checkHMWalkable);
	
//	if(checkHMWalkable.GetParam(5).m_asInt != NO_WALK)
//	{
		cEvent TerHeiReq(TerrainHeightRequest, newX, newZ);
		g_eventMngr.SendEvent(&TerHeiReq);
		m_phys.m_groundHeight = TerHeiReq.GetParam(1).m_asFloat;
//	}


	//add change to position after checks
	m_phys.m_position.m_x = newX;
	m_phys.m_position.m_z = newZ;


	if (m_phys.m_position.m_y <= m_phys.m_groundHeight) //on ground
	{
#ifdef MY_DEBUG
			debug << "On Ground" << endl;
#endif
		m_phys.m_position.m_y = m_phys.m_groundHeight;
	}
	else //off ground
	{
/*#ifdef MY_DEBUG
		debug << "Off Ground" << endl
			<< "\tcurrent posY: " << m_phys.m_position.m_y << endl
			<< "\tcurrent FallSpd: " << m_phys.m_verticalSpeed << endl;
#endif*/
		cEvent fallEvent(PhysFall, &m_phys, m_id.GetID());
		g_eventMngr.SendEvent( &fallEvent );
		currentHP -= fallEvent.GetParam(0).m_asInt;

		m_jump = false;
	}

	//test for collision, handle as needed
	cEvent e(EntityUpdated, m_id.GetID());
	g_eventMngr.SendEvent( &e );
	if (e.GetParam(5).m_asInt == NO_WALK) //collided with object, investigate further
	{
		m_phys.m_position.m_x -= dx;
		m_phys.m_position.m_z -= dz;
		g_eventMngr.SendEvent( &e );
/*		if (e.GetParam(5).m_asInt == NO_WALK) //standing on object, handle it
		{
			m_phys.m_fallTime = 0;
			m_phys.m_onGround = true;
			m_phys.m_position.m_y += m_phys.m_verticalSpeed * dt;
			m_phys.m_verticalSpeed = 0;
		}*/

		if (e.GetParam(5).m_asInt == NO_COLLISION && 
				 m_phys.m_onGround == true) //check if walking into short enough object
		{
			m_phys.m_position.m_x += dx;
			m_phys.m_position.m_z += dz;
			m_phys.m_position.m_y += MAX_WALK_ONTO_HEIGHT;
			g_eventMngr.SendEvent( &e );
			if (e.GetParam(5).m_asInt == NO_WALK) //object too tall, check partial move
			{
				//check movement in Z only
				m_phys.m_position.m_x -= dx;
				m_phys.m_position.m_y -= MAX_WALK_ONTO_HEIGHT;
				g_eventMngr.SendEvent( &e );
				if (e.GetParam(5).m_asInt == NO_WALK) //check X only now
				{
					m_phys.m_position.m_x += dx;
					m_phys.m_position.m_z -= dz;
					g_eventMngr.SendEvent( &e );
					if (e.GetParam(5).m_asInt == NO_WALK) //can't move anywhere
					{
						m_phys.m_position.m_x -= dx;
					}
				}
			}
			else //object short enough, get on top of object
			{
				m_phys.m_position.m_y += MAX_WALK_ONTO_HEIGHT;
			}
		}
	}

	if(!m_crouch)
	{
		g_eventMngr.PostEvent(
			new cEvent(CameraMove, m_phys.m_position.m_x, 
			m_phys.m_position.m_y + AVATAR_HEIGHT, m_phys.m_position.m_z));
	}
	else
	{
		g_eventMngr.PostEvent(
			new cEvent(CameraMove, m_phys.m_position.m_x, 
			m_phys.m_position.m_y + (AVATAR_HEIGHT * CROUCH_HEIGHT_MUL), m_phys.m_position.m_z));
	}

	//debug << "End cAvatarEntity::Update()" 
	//      << endl;
}

/////////////////////////////////////////////////
void cAvatarEntity::HandleEvent(cEvent* e) // ADD
{
	//debug << "Begin cAvatarEntity::HandleEvent(" 
	//      << e->GetEventID().GetStr() << ")" 
	//      << endl;
	EventID eid = e->GetEventID();

	if(eid == KeyPress)
	{
		int keyPressed = e->GetParam(0).m_asInt;
		if(keyPressed == m_forwardKey)
			m_forward = true;
		else if(keyPressed == m_leftwardKey)
			m_leftward = true;
		else if(keyPressed == m_backwardKey)
			m_backward = true;
		else if(keyPressed == m_rightwardKey)
			m_rightward = true;
		else if(keyPressed == m_sprintKey)
			m_sprint = true;
		else if(keyPressed == m_crouchKey)
			m_crouch = true;
		else if(e->GetParam(0).m_asInt == m_jumpKey)
			m_jump = true;

		//cancel out exclusive commands
		if(m_forward && m_backward)  
			m_forward = m_backward = false;
		if(m_leftward && m_rightward)  
			m_leftward = m_rightward = false;
		if(m_sprint && m_backward)
			m_sprint = false;
	}
	else if(eid == KeyDown)
	{
		int keyPressed = e->GetParam(0).m_asInt;
		if(keyPressed == m_forwardKey)
			m_forward = true;
		else if(keyPressed == m_leftwardKey)
			m_leftward = true;
		else if(keyPressed == m_backwardKey)
			m_backward = true;
		else if(keyPressed == m_rightwardKey)
			m_rightward = true;
		else if(keyPressed == m_sprintKey)
			m_sprint = true;
		else if(keyPressed == m_crouchKey)
			m_crouch = true;
		else if(e->GetParam(0).m_asInt == m_jumpKey)
			m_jump = true;

		//cancel out exclusive commands
		if(m_forward && m_backward)  
			m_forward = m_backward = false;
		if(m_leftward && m_rightward)  
			m_leftward = m_rightward = false;
		if(m_sprint && m_backward)
			m_sprint = false;
	}
	else if(eid == MouseMove)
	{
		m_pan = e->GetParam(0).m_asInt;
		m_pitch = e->GetParam(1).m_asInt;
	}
	else if(eid == KeyUp)
	{
		int keyPressed = e->GetParam(0).m_asInt;
		if(keyPressed == m_sprintKey)
			m_sprint = false;
		else if(keyPressed == m_crouchKey)
			m_crouch = false;
	}
	//debug << "End cAvatarEntity::HandleEvent()" << endl;
}

/////////////////////////////////////////////////
void cAvatarEntity::SetPosition(const sVector3d& pos)
{
	m_phys.m_position = pos;

	g_eventMngr.PostEvent(
		new cEvent(CameraMove, 
		m_phys.m_position.m_x, 
		m_phys.m_position.m_y + (AVATAR_HEIGHT * m_phys.m_scale.m_y), 
		m_phys.m_position.m_z));
}

/////////////////////////////////////////////////
void cAvatarEntity::SetRotation(const sVector3d& rot)
{
	m_phys.m_rotation = rot;

	g_eventMngr.PostEvent(
		new cEvent(CameraRotate, 
		m_phys.m_rotation.m_x, 
		m_phys.m_rotation.m_y, 
		m_phys.m_rotation.m_z));
}

/////////////////////////////////////////////////
void cAvatarEntity::SetScale(const sVector3d& scl)
{
	m_phys.m_scale = scl;

	g_eventMngr.PostEvent(
		new cEvent(CameraMove, 
		m_phys.m_position.m_x, 
		m_phys.m_position.m_y + (AVATAR_HEIGHT * m_phys.m_scale.m_y), 
		m_phys.m_position.m_z));
}