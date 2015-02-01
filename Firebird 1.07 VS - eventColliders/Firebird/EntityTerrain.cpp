

#include "debug.h"
#include "EventMngr.h"
#include "EntityTerrain.h"

/////////////////////////////////////////////////
cTerrainEntity::cTerrainEntity(const EntityID& id)
							   : cEntity(id), m_heightmap(NULL),
							   RenderEvent("GRAPHICS_MODEL_RENDER_EVENT"),
							   AttachHeightmap("ENTITY_ATTACH_HEIGHTMAP_EVENT"),
							   HMWalkableRequest("TERRAIN_CHECK_WALKABLE_REQUEST"),
							   GetHeightRequest("TERRAIN_GET_HEIGHT_REQUEST")
{
	g_eventMngr.RegisterListener(AttachHeightmap, this);
	//g_eventMngr.Trigger();
}

/////////////////////////////////////////////////
cTerrainEntity::~cTerrainEntity()
{}

/////////////////////////////////////////////////
void cTerrainEntity::HandleEvent(cEvent* e) // ADD
{
	//debug << "Begin cTerrainEntity::HandleEvent(" 
	//      << e->GetEventID().GetStr() << ")" 
	//      << endl;
	EventID eid = e->GetEventID();
	
	if(eid == GetHeightRequest && !m_heightmap.empty())
	{
		float height = GetHeight(e->GetParam(0).m_asFloat, e->GetParam(1).m_asFloat);
		e->SetParam(1, (float)height); 
	}

	else if(eid == HMWalkableRequest && !m_heightmap.empty())
	{
		int walkable = CheckSlopeWalkable(e->GetParam(0).m_asFloat, e->GetParam(1).m_asFloat);
		e->SetParam(5, walkable);
	}

	//debug << "End cTerrainEntity::HandleEvent()" << endl;
}

/////////////////////////////////////////////////
void cTerrainEntity::SetPosition(const sVector3d& pos)
{
	//debug << "Begin cTerrainEntity::SetPosition("
	//  << pos.m_x << ", " << pos.m_y << ", " << pos.m_z << ")\n";

	m_phys.m_position = pos;

	D3DXMATRIX S, Rx, Ry, Rz, T;

	D3DXMatrixScaling (&S, m_phys.m_scale.m_x, m_phys.m_scale.m_y, m_phys.m_scale.m_z);

	D3DXMatrixRotationX (&Rx, m_phys.m_rotation.m_x);
	D3DXMatrixRotationY (&Ry, m_phys.m_rotation.m_y);
	D3DXMatrixRotationZ (&Rz, m_phys.m_rotation.m_z);

	D3DXMatrixTranslation (&T, m_phys.m_position.m_x, 
		m_phys.m_position.m_y, m_phys.m_position.m_z);

	m_world = S * Rx * Ry * Rz * T;
	//debug << "End cTerrainEntity::SetPosition()\n";
}

/////////////////////////////////////////////////
void cTerrainEntity::SetRotation(const sVector3d& rot)
{
	//debug << "Begin cTerrainEntity::SetRotation("
	//  << rot.m_x << ", " << rot.m_y << ", " << rot.m_z << ")\n";

	m_phys.m_rotation = rot;

	D3DXMATRIX S, Rx, Ry, Rz, T;

	D3DXMatrixScaling (&S, m_phys.m_scale.m_x, m_phys.m_scale.m_y, m_phys.m_scale.m_z);

	D3DXMatrixRotationX (&Rx, m_phys.m_rotation.m_x);
	D3DXMatrixRotationY (&Ry, m_phys.m_rotation.m_y);
	D3DXMatrixRotationZ (&Rz, m_phys.m_rotation.m_z);

	D3DXMatrixTranslation (&T, m_phys.m_position.m_x, 
		m_phys.m_position.m_y, m_phys.m_position.m_z);

	m_world = S * Rx * Ry * Rz * T;
	//debug << "End cTerrainEntity::SetRotation()\n";
}

/////////////////////////////////////////////////
void cTerrainEntity::SetScale(const sVector3d& scl)
{
	//debug << "Begin cTerrainEntity::SetScale("
	//  << scl.m_x << ", " << scl.m_y << ", " << scl.m_z << ")\n";

	m_phys.m_scale = scl;

	D3DXMATRIX S, Rx, Ry, Rz, T;

	D3DXMatrixScaling (&S, m_phys.m_scale.m_x, m_phys.m_scale.m_y, m_phys.m_scale.m_z);

	D3DXMatrixRotationX (&Rx, m_phys.m_rotation.m_x);
	D3DXMatrixRotationY (&Ry, m_phys.m_rotation.m_y);
	D3DXMatrixRotationZ (&Rz, m_phys.m_rotation.m_z);

	D3DXMatrixTranslation (&T, m_phys.m_position.m_x, 
		m_phys.m_position.m_y, m_phys.m_position.m_z);

	m_world = S * Rx * Ry * Rz * T;
	//debug << "End cTerrainEntity::SetScale()\n";
}

/////////////////////////////////////////////////
void cTerrainEntity::Draw()
{
	//debug << "Begin cTerrainEntity::Draw(" << m_modelId.GetStr() << ")" << endl;

	g_eventMngr.PostEvent(new cEvent(RenderEvent, 
		(void*)&m_modelId, (void*)&m_world));

	//debug << "End cTerrainEntity::Draw()" << endl;
}

/////////////////////////////////////////////////
bool cTerrainEntity::ReadCustomHMFile(string filename)
{
	#ifdef MY_DEBUG
		debug << "Terrain HM loading from: " << filename << endl;
	#endif
	ifstream inFile(filename.c_str());
//	inFile.open(filename.c_str);

	if( !inFile )
	{
		ErrorMsg("Terrain HM failed to load");
		return false;
	}

	//read in custom header data
	inFile >> m_vertsPerRow >> m_vertsPerColumn >> m_cellSpacing 
		>> m_heightmapStart.x
		>> m_heightmapStart.y
		>> m_heightmapStart.z;
	m_numVertices = m_vertsPerRow * m_vertsPerColumn;

	#ifdef MY_DEBUG
		debug << "HM Header read, values:"
			<< "\n\tVertsPerRow: " << m_vertsPerRow
			<< "\n\tVertsPerColumn: " << m_vertsPerColumn
			<< "\n\tCellSpacing: " << m_cellSpacing
			<< "\n\tHMStartX: " << m_heightmapStart.x
			<< "\n\tHMStartY: " << m_heightmapStart.y
			<< "\n\tHMStartZ: " << m_heightmapStart.z << endl;
	#endif
	m_heightmap.resize(m_numVertices);
	//read all verts (as floats)
	for (int i = 0; i < m_numVertices; i++)
	{
		inFile >>m_heightmap[i];
//		debug << "VERT# " << i << ": " << m_heightmap[i] << endl;
	}

	m_cellSpacing *= m_phys.m_scale.m_x;
	m_heightmapStart;
	
	for (int i = 0; i < m_numVertices; i++)
	{
		m_heightmap[i] *= m_phys.m_scale.m_y;
	}

	inFile.close();

	m_cellsPerRow = m_vertsPerRow - 1;
	m_cellsPerColumn = m_vertsPerColumn - 1;
	m_width = m_cellsPerRow * m_cellSpacing;
	m_depth = m_cellsPerColumn * m_cellSpacing;

	g_eventMngr.RegisterListener(GetHeightRequest, this);
	g_eventMngr.RegisterListener(HMWalkableRequest, this);

	#ifdef MY_DEBUG
		debug << "Terrain HM loaded!" << endl;
	#endif
	return true;
}

/////////////////////////////////////////////////
float cTerrainEntity::GetHeight(float x, float z)
{
	
	// Translate on xz-plane by the transformation that takes
	// the terrain START point to the origin.
	x = ((float)m_width / 2.0f) + x;
	z = ((float)m_depth / 2.0f) - z;
 
	// Scale down by the transformation that makes the 
	// cellspacing equal to one.  This is given by 
	// 1 / cellspacing since; cellspacing * 1 / cellspacing = 1.
	x /= (float)m_cellSpacing;
	z /= (float)m_cellSpacing;

	// From now on, we will interpret our positive z-axis as
	// going in the 'down' direction, rather than the 'up' direction.
	// This allows to extract the row and column simply by 'flooring'
	// x and z:

	int col = (int)::floorf(x);
	int row = (int)::floorf(z);

	// get the heights of the quad we're in:
	// 
    //  A   B
    //  *---*
    //  | / |
    //  *---*  
    //  C   D

	float A, B, C, D;
	if ( (row < 0 || row >= m_vertsPerRow - 1 || 
		  col < 0 || col >= m_vertsPerRow - 1) ) 
	{
		return -100.0f;
	}
	A = GetHeightmapEntry(row,   col);
	B = GetHeightmapEntry(row,   col+1);
	C = GetHeightmapEntry(row+1, col);
	D = GetHeightmapEntry(row+1, col+1);

	//
	// Find the triangle we are in:
	//

	// Translate by the transformation that takes the upper-left
	// corner of the cell we are in to the origin.  Recall that our 
	// cellspacing was nomalized to 1.  Thus we have a unit square
	// at the origin of our +x -> 'right' and +z -> 'down' system.
	float dx = x - col;
	float dz = z - row;

	// Note the below compuations of u and v are unneccessary, we really
	// only need the height, but we compute the entire vector to emphasis
	// the books discussion.
	float height = 0.0f;
	if(dz < 1.0f - dx)  // upper triangle ABC
	{
		float uy = B - A; // A->B
		float vy = C - A; // A->C

		// Linearly interpolate on each vector.  The height is the vertex
		// height the vectors u and v originate from {A}, plus the heights
		// found by interpolating on each vector u and v.
		height = A + (uy * dx) + (vy * dz);
	}
	else // lower triangle DCB
	{
		float uy = C - D; // D->C
		float vy = B - D; // D->B

		// Linearly interpolate on each vector.  The height is the vertex
		// height the vectors u and v originate from {D}, plus the heights
		// found by interpolating on each vector u and v.
		height = D + (uy * (1.0f - dx)) + (vy * (1.0f - dz));
	}

/*	#ifdef MY_DEBUG
		debug << "GET HEIGHT DATA:"
			<< "\n\tX: " << x
			<< "\n\tZ: " << z
			<< "\n\tCOL: " << col
			<< "\n\tROW: " << row
			<< "\n\tA: " << A
			<< "\n\tB: " << B
			<< "\n\tC: " << C
			<< "\n\tD: " << D
			<< "\n\tHEIGHT: " << height << endl;
	#endif*/

	return height;
}

/////////////////////////////////////////////////
int cTerrainEntity::CheckSlopeWalkable(float x, float z)
{ 
	// Translate on xz-plane by the transformation that takes
	// the terrain START point to the origin.
	x = ((float)m_width / 2.0f) + x;
	z = ((float)m_depth / 2.0f) - z;
 
	// Scale down by the transformation that makes the 
	// cellspacing equal to one.  This is given by 
	// 1 / cellspacing since; cellspacing * 1 / cellspacing = 1.
	x /= (float)m_cellSpacing;
	z /= (float)m_cellSpacing;

	// From now on, we will interpret our positive z-axis as
	// going in the 'down' direction, rather than the 'up' direction.
	// This allows to extract the row and column simply by 'flooring'
	// x and z:

	int col = (int)::floorf(x);
	int row = (int)::floorf(z);

	// get the heights of the quad we're in:
	// 
    //  A   B
    //  *---*
    //  | / |
    //  *---*  
    //  C   D

	float A, B, C;
	if ( (row < 0 || row >= m_vertsPerRow - 1 || 
		  col < 0 || col >= m_vertsPerRow - 1) ) 
	{
		return NO_COLLISION;
	}

	float dx = x - col;
	float dz = z - row;
	if(dz < 1.0f - dx)  // upper triangle ABC
	{
		A = GetHeightmapEntry(row,   col);
		B = GetHeightmapEntry(row,   col+1);
		C = GetHeightmapEntry(row+1, col);
	}
	else // lower triangle DCB
	{
		A = GetHeightmapEntry(row,   col+1);
		B = GetHeightmapEntry(row+1, col);
		C = GetHeightmapEntry(row+1, col+1);
	}

	// Check slopes, if too steep return NO_WALK, else return WALK_ON_TOP.
	return CheckSlopeWalkable(A, B, C);
}

/////////////////////////////////////////////////
int cTerrainEntity::CheckSlopeWalkable(float PointA, float PointB, float PointC)
{
	// Check slopes, if too steep return NO_WALK, else return WALK_ON_TOP.
	return WALK_ON_TOP;
}