#ifndef __EVENTDATA_H__
#define __EVENTDATA_H__

/////////////////////////////////////////////////
#include <d3dx9.h>
#include "HashString.h"

/////////////////////////////////////////////////
struct sVarient
{
	enum Type
	{
		TYPE_INTEGER, // = 0
		TYPE_FLOAT,   // = 1
		TYPE_BOOL,    // = 2
		TYPE_STRING,  // = 3
		TYPE_POINTER, // = 4
		//TYPE_ID,      // = 5
		TYPE_COUNT = 0xffffffff
	}; 

	Type m_type;

	union
	{
		int m_asInt;
		float m_asFloat;
		bool m_asBool;
		const char* m_asString;
		void* m_asPointer;
		//cHashString m_asId;
	};

	sVarient()
		: m_type(TYPE_POINTER), m_asPointer(NULL)
	{}

	sVarient(int i)
		: m_type(TYPE_INTEGER)
	{  this->m_asInt = i;  }

	sVarient(float f)
		: m_type(TYPE_FLOAT)
	{  m_asFloat = f;  }

	sVarient(bool b)
		: m_type(TYPE_BOOL)
	{  m_asBool = b;  }

	sVarient(const char* s)
		: m_type(TYPE_STRING)
	{  m_asString = s;  }

	sVarient(void* p)
		: m_type(TYPE_POINTER)
	{  m_asPointer = p;  }

	/*sVarient(const cHashString& id)
	: m_type(TYPE_ID)
	{  m_asId = id;  }*/
};

/////////////////////////////////////////////////
class cEvent
{
protected:
	static const int s_paramMax = 6;

	EventID m_id;
	sVarient m_params[s_paramMax];
	bool isComplete;

public:
	cEvent(const EventID& id)
		: m_id(id), isComplete(false)
	{}

	cEvent(const EventID& id, const sVarient& var0)
		: m_id(id), isComplete(false)
	{  m_params[0] = var0;  }

	cEvent(const EventID& id, const sVarient& var0,
		const sVarient& var1)
		: m_id(id), isComplete(false)
	{  
		m_params[0] = var0; 
		m_params[1] = var1;
	}

	cEvent(const EventID& id, const sVarient& var0,
		const sVarient& var1,
		const sVarient& var2)
		: m_id(id), isComplete(false)
	{  
		m_params[0] = var0; 
		m_params[1] = var1;
		m_params[2] = var2;
	}

	cEvent(const EventID& id, const sVarient& var0,
		const sVarient& var1,
		const sVarient& var2,
		const sVarient& var3)
		: m_id(id), isComplete(false)
	{  
		m_params[0] = var0; 
		m_params[1] = var1;
		m_params[2] = var2;
		m_params[3] = var3;
	}

	cEvent(const EventID& id, const sVarient& var0,
		const sVarient& var1,
		const sVarient& var2,
		const sVarient& var3,
		const sVarient& var4)
		: m_id(id), isComplete(false)
	{  
		m_params[0] = var0; 
		m_params[1] = var1;
		m_params[2] = var2;
		m_params[3] = var3;
		m_params[4] = var4;
	}

	cEvent(const EventID& id, const sVarient& var0,
		const sVarient& var1,
		const sVarient& var2,
		const sVarient& var3,
		const sVarient& var4,
		const sVarient& var5)
		: m_id(id), isComplete(false)
	{  
		m_params[0] = var0; 
		m_params[1] = var1;
		m_params[2] = var2;
		m_params[3] = var3;
		m_params[4] = var4;
		m_params[5] = var5;
	}

	inline virtual EventID GetEventID() const
	{  return m_id;  }

	inline sVarient GetParam(int i)
	{  
		if(0 <= i && i < s_paramMax)
			return m_params[i];  
		return sVarient();
	}

	inline void SetParam(int i, const sVarient& var)
	{
		if(0 <= i && i < s_paramMax)
			m_params[i] = var;
	}

	inline bool CheckIsComplete()
	{
		return isComplete;
	}

	inline void SetIsComplete(bool set)
	{
		isComplete = set;
	}

};

/////////////////////////////////////////////////
#endif // __EVENTDATA_H__