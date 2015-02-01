// Timer.h - class cTimer definition

#ifndef __TIMER_H__
#define __TIMER_H__

#include <windows.h>

class cTimer : public iListener
{
private:
	__int64 cntsPerSec;
	float secsPerCnt;
	__int64 currTimeStamp,
		prevTimeStamp;
	float dt;
	bool paused;

	EventID TimeUpdate;
	EventID TimePause;

public:
	cTimer()
		: cntsPerSec(0), prevTimeStamp(0), dt(0.0f),
		TimeUpdate("TIMER_UPDATE_EVENT"),
		TimePause("TIMER_PAUSE_EVENT")
	{
		QueryPerformanceFrequency ((LARGE_INTEGER*)&cntsPerSec);
		secsPerCnt = 1.0f / (float)cntsPerSec;
		QueryPerformanceCounter ((LARGE_INTEGER*)&prevTimeStamp);

		g_eventMngr.RegisterListener(TimePause, this);
		paused = false;
	}

	void Tick()
	{
		//debug << "Begin cTimer::Tick()" << endl;
		currTimeStamp = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
		dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;
		prevTimeStamp = currTimeStamp;

		if (paused)
		{
			cEvent e(TimeUpdate, 0);
			g_eventMngr.SendEvent(&e);
			return;
		}
		cEvent e(TimeUpdate, sVarient(dt));
		g_eventMngr.SendEvent(&e);
		//debug << "Time delta: " << dt << endl;
		//debug << "End cTimer::Tick()" << endl;
	}

	void TogglePause()
	{
		if (paused)
			paused = false;
		else
			paused = true;
	}

	void HandleEvent(cEvent* e)
	{
		EventID eid = e->GetEventID();
		if (eid == TimePause)
			TogglePause();
	}
}; 

/////////////////////////////////////////////////
#endif // __TIMER_H__