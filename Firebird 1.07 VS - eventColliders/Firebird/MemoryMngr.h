#ifndef __MEMORY_MANAGER_H__
#define __MEMORY_MANAGER_H__
/*
const static int NAMELENGTH = 40;

struct AllocHeader;
class cHeap;

/////////////////////////////////////////////////
class cHeap
{
private:
	char m_name[NAMELENGTH];

public:
	cHeap(const char* name);

	const char* GetName() const;

	void* operator new (size_t size, cHeap* pHeap);
	void operator delete (void* pMem);


//
//
	void AddAllocation(size_t s){}
	void RemoveAllocation(size_t s){}
//
//
};

/////////////////////////////////////////////////
struct AllocHeader
{
public:
	cHeap* m_pHeap;
	int m_size;
};
/*
class cMemoryMngr
{
public:
	cMemoryMngr(void);
	~cMemoryMngr(void);
};
*/

#endif