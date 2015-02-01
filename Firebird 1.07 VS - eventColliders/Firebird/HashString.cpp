#include <string>
using std::string;
#include <map>
using std::map;

#include "HashString.h"

/////////////////////////////////////////////////
cHashString::stringMap cHashString::s_str;

/////////////////////////////////////////////////
cHashString::cHashString()
: m_id(0)
{
	s_str[0] = NULL; 
}

/////////////////////////////////////////////////
cHashString::cHashString(char const * const s)
: m_id(0)
{
	SetStr(s);
}

/////////////////////////////////////////////////
cHashString::cHashString(const cHashString& hs)
: m_id(hs.m_id)
{}

/////////////////////////////////////////////////
cHashString::~cHashString()
{
	//m_str[0] = '\0';
	m_id = 0;
}

/////////////////////////////////////////////////
void cHashString::SetStr(const char* str)
{
	m_id = HashName(str);

	strIter it = s_str.find(m_id);
	if(it == s_str.end())
	{
		int len = strlen(str) + 1;
		char* temp = new char[len];
		strcpy_s(temp, len, str);
		s_str[m_id] = temp;
	}
}

/////////////////////////////////////////////////
bool cHashString::SetID(void* id)
{
	strIter it = s_str.find(id);
	if(it == s_str.end())
		return false;

	m_id = id;
	return true;
}

/////////////////////////////////////////////////
const cHashString& cHashString::operator=(
	const cHashString& hs)
{
	m_id = hs.m_id;

	return *this;
}

/////////////////////////////////////////////////
void* cHashString::HashName(char const* s)
{
	unsigned long base = 65521L;
	unsigned long nMax = 5552L;

	if(s == NULL)
		return NULL;

	unsigned long s1 = 0;
	unsigned long s2 = 0;

	for(size_t len = strlen(s); len > 0; )
	{
		unsigned long k = (len < nMax ? len : nMax);
		len -= k;
		while(k >= 16)
		{
			for(int i = 0; i < 16; ++i)
			{
				s1 += tolower(s[i]); 
				s2 += s1;
			} 
			s += 16;
			k -= 16;
		}

		if(k != 0)
			do
			{
				s1 += *s++;
				s2 += s1;
			}while(--k);

			s1 %= base;
			s2 %= base;
	}

	return (void*)((s2 << 16) | s1);
}

/////////////////////////////////////////////////
void cHashString::Shutdown()
{
	while(!s_str.empty())
	{
		strIter it = s_str.begin();
		if(it->second != NULL)
			delete it->second;

		s_str.erase(it);
	}
}