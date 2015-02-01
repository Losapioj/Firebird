#ifndef __TERRAINENTITY_H__
#define __TERRAINENTITY_H__

#include "Entity.h"

/////////////////////////////////////////////////
class cTerrainEntity : public cEntity,
	public iRenderable,
	public iListener
{
protected:
	GraphicID m_modelId;

	EventID RenderEvent;
	EventID AttachHeightmap;
	EventID HMWalkableRequest;
	EventID GetHeightRequest;

	//new data for HM use
	std::vector<float> m_heightmap;
	//USE HM OR COLISION MODEL

	int m_vertsPerRow;
	int m_vertsPerColumn;
	float m_cellSpacing;
	D3DXVECTOR3 m_heightmapStart;

	int m_cellsPerRow;
	int m_cellsPerColumn;
	float m_width;
	float m_depth;
	int m_numVertices;

public:
	cTerrainEntity(const EntityID&);
	virtual ~cTerrainEntity();
	void HandleEvent(cEvent*);

	void Draw();

	virtual const GraphicID& GetModel()
	{  return m_modelId;  }
	virtual void SetModel(const GraphicID& id)
	{  m_modelId = id;  }

	inline float GetHeightmapEntry(int row, int col)
	{	return m_heightmap[row * m_vertsPerRow + col];	}
	inline void SetHeightmapEntry(int row, int col, float value)
	{	m_heightmap[row * m_vertsPerRow + col] = value;	}

	virtual void SetPosition(const sVector3d&);

	virtual void SetRotation(const sVector3d&);

	virtual void SetScale(const sVector3d&);

	////////////
	//NEW METHODS FOR HM USE
	////////////

	//loads a custom HM file and saves it to the entity.
	bool ReadCustomHMFile(string filename);

	float GetHeight(float x, float z);
	
	//Check if HM is too steep
	//retun one of the walkable constants
	int CheckSlopeWalkable(float x, float z);
	int CheckSlopeWalkable(float PointA, float PointB, float PointC);

};

/////////////////////////////////////////////////
#endif // __TERRAINENTITY_H__