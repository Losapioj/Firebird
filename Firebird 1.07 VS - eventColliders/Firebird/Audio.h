#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <fmod.hpp>
using FMOD::System;
using FMOD::Sound;
using FMOD::Channel;

#include <map>
using std::map;

#include "EventMngr.h"

/////////////////////////////////////////////////
typedef struct
{
	Sound* sound;
	Channel* channel;
} Sample;

/////////////////////////////////////////////////
class cAudio : public iListener
{
private:
	typedef map <AudioID, Sample*> SampleMap;
	typedef SampleMap::iterator SampleIter;

	System* m_audio;
	SampleMap m_soundList;

	EventID LoadEvent;
	EventID DeleteEvent;
	EventID PlayEvent;
	EventID StopEvent;
	EventID StopAllEvent;
	EventID IsPlayingReq;

public:
	cAudio();

	bool Init();
	~cAudio();

	bool LoadSound(const char*);
	bool DeleteSound(const AudioID&);

	bool Play(const AudioID& id, int loopCount);
	bool Update();

	bool IsPlaying(const AudioID&);
	bool Stop(const AudioID&);
	void StopAll();

	void HandleEvent(cEvent*);

};

/////////////////////////////////////////////////
#endif // __AUDIO_H__