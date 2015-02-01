#ifndef __EVENTMNGR_H__
#define __EVENTMNGR_H__

/////////////////////////////////////////////////
#include <list>
#include <map>
#include <queue>
#include <fstream>
#include <iostream>

using namespace std;

#include "EventData.h"

/////////////////////////////////////////////////
class iListener
{
public:
	virtual void HandleEvent(cEvent*) = 0;
};

/////////////////////////////////////////////////
class cEventMngr
{
private:
	typedef list <iListener*> ListenerList;
	typedef ListenerList::iterator LLIter;

	typedef map <EventID, ListenerList> EventMap;
	typedef EventMap::iterator EMIter;

	typedef queue <cEvent*> EventQueue;

	EventMap m_registry;
	EventQueue m_eventQ;

public:
	cEventMngr() {}
	~cEventMngr();

	bool Init();
	bool RegisterListener(
		const EventID&, iListener*);
	bool RemoveListener(
		const EventID&, iListener*);
	bool RemoveListener(iListener*);

	void PostEvent(cEvent*);
	bool SendEvent(cEvent*);

	void Trigger();
};

/////////////////////////////////////////////////
extern cEventMngr g_eventMngr;

/////////////////////////////////////////////////
#endif // __EVENTMNGR_H__