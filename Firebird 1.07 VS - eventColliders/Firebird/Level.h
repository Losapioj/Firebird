#ifndef __LEVEL_H__
#define __LEVEL_H__

// needed header files 
#include <windows.h>
#include <string>
#include <map>
#include <vector>
using std::string;
using std::map;
using std::vector;

#include "Util.h"

/*/////////////////////////////////////////////////
struct sMapTile
{
	sPoint m_loc;
	sMapTile* m_neighbors [8];
	float m_f, m_g;
	bool m_open, m_closed;
	sMapTile* m_parent;

	sMapTile ();
	sMapTile (DWORD, DWORD);
};
*/
/////////////////////////////////////////////////
inline float H (sPoint A, sPoint B)
{
	return A.Distance (B);
}

/////////////////////////////////////////////////
class cLevel : public iListener
{
protected:
//	typedef map <DWORD, sMapTile*> TILE_LIST;
//	typedef TILE_LIST::iterator TILE_ITER;

	LevelID m_id;
//	TILE_LIST m_tiles;

//	int horzStart, vertStart, width, height;

//	EventID TileWalkableRequest;
//	EventID FindPathRequest;

	EventID GraphicLoadModel;
	EventID LoadEntity;
	EventID MoveEntity;
	EventID RotateEntity;
	EventID ScaleEntity;
	EventID AttachModel;
	EventID AttachHeightmap;
	EventID LoadCollidable;
	EventID InitPhysicsEvent;

	bool InitModels (ifstream&);
	bool InitEntities (ifstream&);
	bool InitPhysics (ifstream&);

public:
	cLevel();
	~cLevel();
	virtual bool Init(const string&);
/*
	sMapTile* GetTile(DWORD, DWORD);
	sMapTile* GetTile(const sPoint& pt)
	{  return GetTile(pt.m_x, pt.m_y);  }

	bool FindPath(sPoint, sPoint, 
		vector <sPoint>&);
*/
	virtual void HandleEvent(cEvent*);
};

/////////////////////////////////////////////////
#endif // __LEVEL_H__