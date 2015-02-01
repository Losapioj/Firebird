#include <string>
using std::string;

#include "Debug.h"
#include "EventMngr.h"

/////////////////////////////////////////////////
cEventMngr g_eventMngr;

/////////////////////////////////////////////////

cEventMngr::~cEventMngr() 
{
	#ifdef MY_DEBUG
		debug << "~cEventMngr start" << endl;
	#endif

	while(!m_eventQ.empty())
	{
		delete(m_eventQ.front());
		m_eventQ.pop();
	}

	#ifdef MY_DEBUG
		debug << "~cEventMngr end" << endl;
	#endif
}

/////////////////////////////////////////////////
bool cEventMngr::Init()
{
	#ifdef MY_DEBUG
		debug << "Begin cEventMngr::Init()" << endl;
	#endif

	ifstream fin("EventMngr.cfg");
	if (!fin)
	{
		FileNotFoundMsg("EventMngr.cfg");
		return false;
	}

	string str;
	fin >> str;
	if (str != "[BeginEventList]")
	{
		ErrorMsg("Reading event list");
		return false;
	}
	fin >> str;
	while (str != "[EndEventList]")
	{
		cHashString hs(str.c_str());
		m_registry[hs];
		fin >> str;
		if(!fin)
			return false;
	}

	#ifdef MY_DEBUG
		debug << "End cEventMngr::Init()" << endl;
	#endif
	return true;
}

/////////////////////////////////////////////////
bool cEventMngr::RegisterListener (const EventID& ev, iListener* lst)
{
	//debug << "Begin cEventMngr::RegisterListener(" 
	//      << ev.GetStr() << ")" << endl;
#ifdef MY_PROFILE
	//debug << "RELEASE NOT SET TEST" << endl;
	if(lst == NULL)  
	{
		ErrorMsg("cEventMngr::RegisterListener Listener NULL");
		return false;
	}
	EMIter it = m_registry.find(ev);

	
	LLIter lit = find(m_registry [ev].begin(),
		m_registry [ev].end(),
		lst);
	if(lit != m_registry[ev].end())  
	{
		ErrorMsg("cEventMngr::RegisterListener Already Listening");
		return true;
	}

	if(it == m_registry.end())
	{
		ErrorMsg("cEventMngr::RegisterListener non-event requested");
		debug << ev.GetStr() << endl;
		return false;
	}
#endif
	m_registry[ev].push_back(lst);
	return true;
}

/////////////////////////////////////////////////
bool cEventMngr::RemoveListener(const EventID& ev, iListener* lst)
{
	if(lst == NULL)  
	{
		ErrorMsg("cEventMngr::RemoveListener no listener sent.");
		return false;
	}
	EMIter eit = m_registry.find(ev);
	if(eit == m_registry.end())
	{
		ErrorMsg("cEventMngr::RemoveListener event not found.");
		return false;
	}
	LLIter lit = find(m_registry [ev].begin(),
		m_registry [ev].end(),
		lst);
	if(lit == m_registry[ev].end())  
	{
		ErrorMsg("cEventMngr::RemoveListener listener not found.");
		return false;
	}
	m_registry[ev].erase(lit);
	return true;
}

/////////////////////////////////////////////////
bool cEventMngr::RemoveListener(iListener* lst)
{
	bool flag = false;
	if(lst == NULL)  return false;
	for(EMIter eit = m_registry.begin();
		eit != m_registry.end(); ++eit)
	{
		LLIter lit = find(eit->second.begin(),
			eit->second.end(),
			lst);
		if(lit != eit->second.end())
		{
			eit->second.erase(lit);
			flag = true;
		}
	}
	return flag;
}

/////////////////////////////////////////////////
void cEventMngr::PostEvent(cEvent* ed)
{
	//debug << "Begin cEventMngr::PostEvent(" 
	//      << ed->GetEventID().GetStr() << ")" << endl;
	m_eventQ.push(ed);
	//debug << "End cEventMngr::PostEvent()" << endl;
}

/////////////////////////////////////////////////
bool cEventMngr::SendEvent(cEvent* ed)
{
	//debug << "Begin cEventMngr::SendEvent(" << ed->GetEventID().GetStr() << ")" << endl;

	EMIter eit = m_registry.find(ed->GetEventID());
	if(eit == m_registry.end())
	{
		ErrorMsg("Non-event received");
		debug << ed->GetEventID().GetStr() << endl;
		delete ed;
		return false;
	}
	//debug << "event type" << ed->GetParam(0).m_asString << " found, iterating..." << endl;

	LLIter lit = eit->second.begin();
	if(lit == eit->second.end())
	{
		/*debug << "No listeners registered for "
		<< ed->GetEventID().GetStr() << endl;*/
		return false;
	}
	while(lit != eit->second.end() && !ed->CheckIsComplete())
	{
		(*lit)->HandleEvent(ed);
		++lit;
	}

	//debug << "End cEventMngr::SendEvent()" << endl;

	return true;
}

/////////////////////////////////////////////////
void cEventMngr::Trigger()
{
	//debug << "Begin cEventMngr::Trigger()" << endl;
	while(!m_eventQ.empty())
	{
		SendEvent(m_eventQ.front());
		delete m_eventQ.front(); 
		m_eventQ.pop();
	}
	//debug << "End cEventMngr::Trigger()" << endl;
}

/////////////////////////////////////////////////