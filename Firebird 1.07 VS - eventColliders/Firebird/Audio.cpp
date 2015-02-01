
#include "Debug.h"
#include "Audio.h"
#pragma comment(lib, "fmodex_vc.lib")

/////////////////////////////////////////////////
//constants
const static int MAX_CHANNELS = 100;

//ERROR MESSAGES USED
const static string AUDIO_LOAD_COPY = "Audio file already loaded";
const static string SAMPLE_CREATION = "Audio sample creation";

/////////////////////////////////////////////////
cAudio::cAudio()
: m_audio(NULL), LoadEvent("AUDIO_LOAD_EVENT"),
DeleteEvent("AUDIO_DELETE_EVENT"),
PlayEvent("AUDIO_PLAY_EVENT"),
StopEvent("AUDIO_STOP_EVENT"),
StopAllEvent("AUDIO_STOPALL_EVENT"),
IsPlayingReq("AUDIO_ISPLAYING_REQUEST")
{
	g_eventMngr.RegisterListener(LoadEvent, this);
	g_eventMngr.RegisterListener(DeleteEvent, this);
	g_eventMngr.RegisterListener(PlayEvent, this);
	g_eventMngr.RegisterListener(StopEvent, this);
	g_eventMngr.RegisterListener(StopAllEvent, this);
	g_eventMngr.RegisterListener(IsPlayingReq, this);
}

/////////////////////////////////////////////////
bool cAudio::Init()
{
	#ifdef MY_DEBUG
		debug << "Begin cAudio::Init()" << endl;
	#endif
	if(System_Create(&m_audio) != FMOD_OK)
		return false;

	FMOD_RESULT fr = m_audio->init(
		MAX_CHANNELS, FMOD_INIT_NORMAL, NULL);

	if (fr != FMOD_OK)
	{
		ErrorMsg("Audio initialization failed.");
		return false;
	}

	#ifdef MY_DEBUG
		debug << "End cAudio::Init()" << endl;
	#endif
	return true;
}

/////////////////////////////////////////////////
cAudio::~cAudio()
{
	#ifdef MY_DEBUG
		debug << "~cAudio start" << endl;
	#endif

	StopAll();
	while(!m_soundList.empty())
	{
		SampleIter it = m_soundList.begin();
		if(it->second != NULL)
			delete it->second;

		m_soundList.erase(it);
	}

	if(m_audio != NULL)
		m_audio->release();
	m_audio = NULL;

	#ifdef MY_DEBUG
		debug << "~cAudio end" << endl;
	#endif
}

/////////////////////////////////////////////////
bool cAudio::LoadSound(const char* file)
{
	//debug << "Begin cAudio::LoadSound(" << file << ")" << endl;
	AudioID id(file);
	SampleIter it = m_soundList.find(id);
	if (it != m_soundList.end ())
	{
		ErrorMsg(AUDIO_LOAD_COPY);
		//debug << "End cAudio::LoadSound() ==> true" << endl;
		return true;
	}

	Sample* s = new Sample;
	if(s == NULL)
	{
		ErrorMsg(SAMPLE_CREATION);
		//debug << "End cAudio::LoadSound() ==> false" << endl;
		return false;
	}

	FMOD_RESULT fr = m_audio->createSound (
		file, FMOD_DEFAULT, NULL, &(s->sound));
	if (fr != FMOD_OK)
	{
		FileNotFoundMsg(file);
		delete s;
		//debug << "End cAudio::LoadSound() ==> false" << endl;
		return false;
	}

	s->sound->setMode(FMOD_LOOP_NORMAL);

	m_soundList[id] = s;

	//debug << "End cAudio::LoadSound() ==> true" << endl;
	return true;
}

/////////////////////////////////////////////////
bool cAudio::DeleteSound(const AudioID& id)
{
	SampleIter it = m_soundList.find(id);
	if(it == m_soundList.end())
		return false;

	Stop(id);
	m_soundList[id]->sound->release();
	m_soundList.erase(it);

	return true;
}

/////////////////////////////////////////////////
bool cAudio::Play(const AudioID& id, int loopCount)
{
	SampleIter it = m_soundList.find(id);
	if (it == m_soundList.end())
		return false;
	
	m_soundList[id]->channel->setLoopCount(loopCount);

	FMOD_RESULT fr = m_audio->playSound(
		FMOD_CHANNEL_FREE, m_soundList[id]->sound, 
		true, &(m_soundList[id]->channel));

	if(fr != FMOD_OK)
		return false;

	m_soundList[id]->channel->getLoopCount(&loopCount);
	//debug << "\t\t LOOP COUNT: " << loopCount << endl << endl;
	m_soundList[id]->channel->setPaused(false);

	return true;
}

/////////////////////////////////////////////////
bool cAudio::Update()
{
	if (m_audio->update() == FMOD_OK)
		return true;

	return false;
}

/////////////////////////////////////////////////
bool cAudio::IsPlaying(const AudioID& id)
{
	SampleIter it = m_soundList.find(id);
	if (it == m_soundList.end())
		return false;

	bool flag = false;
	FMOD_RESULT fr = 
		m_soundList[id]->channel->isPlaying(&flag);

	return (flag && fr == FMOD_OK);
}

/////////////////////////////////////////////////
bool cAudio::Stop(const AudioID& id)
{
	SampleIter it = m_soundList.find(id);
	if(it == m_soundList.end())
		return false;

	if(!IsPlaying(id))
		return false;

	m_soundList[id]->channel->stop();

	return true;
}

/////////////////////////////////////////////////
void cAudio::StopAll ()
{
	for(SampleIter it = m_soundList.begin(); 
		it != m_soundList.end (); ++it)
		it->second->channel->stop ();
}

/////////////////////////////////////////////////
void cAudio::HandleEvent(cEvent* e)
{
	EventID id = e->GetEventID();
	if(id == LoadEvent)
		LoadSound(e->GetParam(0).m_asString);

	else if(id == DeleteEvent)
	{
		AudioID id;
		if(!id.SetID(e->GetParam(0).m_asPointer))
			return;
		DeleteSound(id);
	}

	else if(id == PlayEvent)
	{
		AudioID id;
		if(!id.SetID(e->GetParam(0).m_asPointer))
			return;
		Play(id, e->GetParam(1).m_asInt);
	}

	else if(id == StopEvent)
	{
		AudioID id;
		if(!id.SetID(e->GetParam(0).m_asPointer))
			return;
		Stop(id);
	}

	else if(id == StopAllEvent)
		StopAll();

	else if(id == IsPlayingReq)
	{
		AudioID id;
		if(!id.SetID(e->GetParam(0).m_asPointer))
			return;
		bool b = IsPlaying(id);
		e->SetParam(1,sVarient(b));
	}
}