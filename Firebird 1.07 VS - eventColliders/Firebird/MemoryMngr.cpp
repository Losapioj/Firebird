#include <stdlib.h> 

#include "MemoryMngr.h"
/*
/////////////////////////////////////////////////
void* cHeap::operator new(size_t size, cHeap* pHeap)
{
	size_t requestedBytes = size + sizeof(AllocHeader);
	char* pMem = (char*) malloc(requestedBytes);
	AllocHeader* pHeader = (AllocHeader*) pMem;
	pHeader->m_pHeap = pHeap;
	pHeader->m_size = size;
	pHeap->AddAllocation(size);

	void* pStartMemBlock = pMem + sizeof(AllocHeader);
	return pStartMemBlock;
}

/////////////////////////////////////////////////
void cHeap::operator delete(void *pMem)
{
	AllocHeader* pHeader = (AllocHeader*)( (char*)pMem - sizeof(AllocHeader) );
	pHeader->m_pHeap->RemoveAllocation(pHeader->m_size);
	free(pHeader);
}
*/
/////////////////////////////////////////////////
/*
cMemoryMngr::cMemoryMngr(void)
{
}

cMemoryMngr::~cMemoryMngr(void)
{
}
*/