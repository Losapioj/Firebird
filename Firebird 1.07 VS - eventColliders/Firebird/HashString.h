#ifndef __HASHSTRING_H__
#define __HASHSTRING_H__

#include <map>
using std::map;

/////////////////////////////////////////////////
class cHashString
{
private:
	typedef map<void*, char*> stringMap;
	typedef stringMap::iterator strIter;

	void* m_id;
	static stringMap s_str;

public:
	cHashString();
	cHashString(char const * const);
	cHashString(const cHashString&);
	~cHashString();

	char const * const GetStr() const
	{  return s_str[m_id];  }
	/*void* GetID() const
	{  return m_id;  }*/
	int GetID() const
	{  return (int)m_id;  }
	void SetStr(char const * const);
	bool SetID(void*);
	bool SetID(int id)
	{  return SetID((void*)id);  }

	bool operator<(const cHashString& hs) const
	{  return GetID() < hs.GetID();  }
	bool operator==(const cHashString& hs) const
	{  return GetID() == hs.GetID();  }
	bool operator!=(const cHashString& hs) const
	{  return !(GetID() == hs.GetID());  }

	const cHashString& operator=(const cHashString&);
	//const cHashString& operator=(char const * const);

	static void* HashName(char const*);
	static void Shutdown();
};

/////////////////////////////////////////////////
typedef cHashString EventID;
typedef cHashString GraphicID;
typedef cHashString EntityID;
typedef cHashString AudioID;
typedef cHashString LevelID;
//typedef cHashString TextureID;

/////////////////////////////////////////////////
#endif // __HASHSTRING_H__