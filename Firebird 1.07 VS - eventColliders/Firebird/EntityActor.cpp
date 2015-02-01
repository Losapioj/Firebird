//#include "EntityActor.h"
#define _CRT_RAND_S
#include <stdlib.h>

#include "debug.h"
#include "EventMngr.h"
#include "EntityActor.h"

/////////////////////////////////////////////////
const static float DEFAULT_BOUND_CYL_RAD = 2.0f;
const static float DEFAULT_BOUND_CYL_HEIGHT = 6.0f;

/////////////////////////////////////////////////
cActorEntity::cActorEntity (const EntityID& id)
: cEntity(id), m_state (WAITING), 
//TileWalkableRequest("LEVEL_TILE_WALKABLE_REQUEST"),
//FindPathRequest("LEVEL_FIND_PATH_REQUEST"),
RenderEvent("GRAPHICS_MODEL_RENDER_EVENT"),
EntityCreated("ENTITY_ATTACH_COLLIDABLE_EVENT"),
EntityUpdated("ENTITY_UPDATED_EVENT")
{
	cBoundingCylinder* cyl = new cBoundingCylinder(DEFAULT_BOUND_CYL_RAD, 
												   DEFAULT_BOUND_CYL_HEIGHT, 
												   &m_phys);
	g_eventMngr.PostEvent(new cEvent(EntityCreated, m_id.GetID(), cyl));
}

/////////////////////////////////////////////////
void cActorEntity::Wait()
{/*
	//debug << "Begin cActorEntity::Wait()" << endl;
	if(m_path.empty())
	{
		sPoint curTile((int)m_phys.m_position.m_x, 
			(int)m_phys.m_position.m_z);
		sPoint newTile;
		rand_s((UINT*)&newTile.m_x);
		rand_s((UINT*)&newTile.m_y);
		if(newTile.m_x < 0)  newTile.m_x *= -1;
		if(newTile.m_y < 0)  newTile.m_y *= -1;
		newTile.m_x %= 30;
		newTile.m_y %= 30;
		int room;
		rand_s((UINT*)&room);
		room %= 2;
		if(room == 0)
			newTile += sPoint(30,0);
		else if(room == 1)
			newTile += sPoint(0,80);

//		cEvent e(FindPathRequest, 0,
//			&curTile, &newTile, &m_path);
//		g_eventMngr.SendEvent(&e);
	}
	else // START ROTATING TOWARDS NEW DIRECTION
	{
		// Find next tile on current tile's
		// neighbors list
		sPoint curTile((int)m_phys.m_position.m_x, 
			(int)m_phys.m_position.m_z);
		sPoint neighbors[8] = {
			sPoint(curTile.m_x,   curTile.m_y+1), 
			sPoint(curTile.m_x+1, curTile.m_y+1), 
			sPoint(curTile.m_x+1, curTile.m_y), 
			sPoint(curTile.m_x+1, curTile.m_y-1), 
			sPoint(curTile.m_x,   curTile.m_y-1), 
			sPoint(curTile.m_x-1, curTile.m_y-1), 
			sPoint(curTile.m_x-1, curTile.m_y), 
			sPoint(curTile.m_x-1, curTile.m_y+1)};
			sPoint nextTile = m_path[0];

			for (int i = 0; i < 8; ++i)
			{
				if (nextTile == neighbors[i])
				{
					// Verify tile is walkable
/*					cEvent e(TileWalkableRequest, 0,
						nextTile.m_x, nextTile.m_y);
					g_eventMngr.SendEvent(&e);
					if(e.GetParam(3).m_asBool)
					{
						m_dir = i;
						m_dest.m_x = (float)(nextTile.m_x);
						m_dest.m_y = i * QTR_PI;
						m_dest.m_z = (float)(nextTile.m_y);
						m_state = ROTATING;
						break;
					}
*
					else 
					{
						m_path.clear();
						m_state = WAITING;
						return;
					}
				}
			}
			//debug << "Destination Heading: " << m_dest.m_y << endl;
	}
	//debug << "End cActorEntity::Wait()" << endl;
*/} 

/////////////////////////////////////////////////
void cActorEntity::Rotate (float dt)
{
	//debug << "state: ROTATING" << endl;
	//debug << "Current rotation: " << m_rotation.y << endl;
	if (m_dest.m_y > m_phys.m_rotation.m_y)
	{
		if (m_dest.m_y - m_phys.m_rotation.m_y < PI)
			m_phys.m_rotation.m_y += PI * dt;
		else 
			m_phys.m_rotation.m_y -= PI * dt;
	}
	else if (m_phys.m_rotation.m_y > m_dest.m_y)
	{
		if (m_phys.m_rotation.m_y - m_dest.m_y < PI)
			m_phys.m_rotation.m_y -= PI * dt;
		else 
			m_phys.m_rotation.m_y += PI * dt;
	}

	if (m_phys.m_rotation.m_y < 0.0f)
		m_phys.m_rotation.m_y += TWO_PI;
	else if (m_phys.m_rotation.m_y >= TWO_PI)
		m_phys.m_rotation.m_y -= TWO_PI;

	if (fabs (m_dest.m_y - m_phys.m_rotation.m_y) < 0.01)
	{
		m_phys.m_rotation.m_y = m_dest.m_y;
		m_state = MOVING;
	}
	//debug << "Current rotation: " << m_phys.m_rotation.m_y << endl;
}

/////////////////////////////////////////////////
void cActorEntity::Move (float dt)
{
	//debug << "state: MOVING" << endl;
	float dx = 10 * dt * sin(m_phys.m_rotation.m_y);
	float dz = 10 * dt * cos(m_phys.m_rotation.m_y);
	float newX = m_phys.m_position.m_x + dx;
	float newZ = m_phys.m_position.m_z + dz;
	if (((newX <= m_dest.m_x && m_dest.m_x < m_phys.m_position.m_x) ||
		(newX >= m_dest.m_x && m_dest.m_x > m_phys.m_position.m_x)) ||
		((newZ <= m_dest.m_z && m_dest.m_z < m_phys.m_position.m_z) ||
		(newZ >= m_dest.m_z && m_dest.m_z > m_phys.m_position.m_z)))
	{
		m_phys.m_position.m_x = m_dest.m_x;
		m_phys.m_position.m_z = m_dest.m_z;
		m_state = WAITING;
		m_path.erase(m_path.begin());
	}
	else 
	{
		m_phys.m_position.m_x = newX;
		m_phys.m_position.m_z = newZ;
	}

	g_eventMngr.SendEvent( &cEvent(EntityUpdated, m_id.GetID()) );
	//debug << "New Position: " << m_phys.m_position.m_x 
	//      << ", " << m_phys.m_position.m_z << endl;
}

/////////////////////////////////////////////////
void cActorEntity::Update (float dt)
{
	//debug << "\nBegin cActorEntity::Update ()" << endl;
	//debug << "Current state: " << m_state << endl;
	if (m_state == WAITING)
		Wait ();

	else if (m_state == ROTATING)
		Rotate (dt);

	else if (m_state == MOVING)
		Move (dt);

	D3DXMATRIX S, Rx, Ry, Rz, T;

	D3DXMatrixScaling (&S, m_phys.m_scale.m_x, m_phys.m_scale.m_y, m_phys.m_scale.m_z);

	D3DXMatrixRotationX (&Rx, m_phys.m_rotation.m_x);
	D3DXMatrixRotationY (&Ry, m_phys.m_rotation.m_y);
	D3DXMatrixRotationZ (&Rz, m_phys.m_rotation.m_z);

	D3DXMatrixTranslation (&T, m_phys.m_position.m_x, 
		m_phys.m_position.m_y, m_phys.m_position.m_z);

	m_world = S * Rx * Ry * Rz * T;

	//debug << "End cActorEntity::Update ()" << endl;
}

/////////////////////////////////////////////////
void cActorEntity::Draw()
{ 
	//debug << "Begin cActorEntity::Draw()" << endl;

	g_eventMngr.PostEvent(new cEvent(RenderEvent, 
		(void*)&m_modelId, (void*)&m_world));

	//debug << "End cActorEntity::Draw()" << endl;
}