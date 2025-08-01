//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : General declarations
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// _DEBUG
// - Generate debug version
//
// _MSC_VER
// - Use Microsoft C++ extensions when set.
//
// USE_SDL
// - Compile with sdl support instead of mss (define: civsound.h)
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragmas commented out
// - Includes fixed for case sensitive filesystems
// - Added sdl sound and cdrom support
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "soundmanager.h"
#include "pointerlist.h"
#include "profileDB.h"
#include "SoundRecord.h"
#include "CivPaths.h"
#include "c3files.h"
#include "PlayListDB.h"
#include "gamesounds.h"
#include <iostream>
#include <cctype>

extern HWND			gHwnd;
extern ProfileDB	*g_theProfileDB;
extern CivPaths		*g_civPaths;
extern PlayListDB	*g_thePlayListDB;

SoundManager		*g_soundManager     = NULL;
static SDL_PropertiesID g_noLoopProps = SDL_CreateProperties();

#ifndef __linux__
#define CI_FixName(a) a
#endif

namespace
{
    uint32 const    k_CHECK_MUSIC_PERIOD = 4000;     // [ms]
    uint32 const    SLIDER_FULL         = 10;		// used in SoundManager::setVolume
    sint32          s_startTrack        = 1;        // skip CD data track
}

void SoundManager::Initialize()
{
    delete g_soundManager;
    g_soundManager = new SoundManager();
}

void SoundManager::Cleanup()
{
    delete g_soundManager;
    g_soundManager = NULL;
}

SoundManager::SoundManager()
:
		m_sfxSounds                 (NULL),
		m_voiceSounds               (NULL),
		m_soundWalker               (NULL),
		m_sfxVolume                 (SLIDER_FULL),
		m_musicVolume               (SLIDER_FULL),
		m_voiceVolume               (SLIDER_FULL),
		m_noSound                   (true),
		m_oggTrack					(0),
		m_musicTrack				(nullptr),
		m_mixer						(nullptr),
		m_timeToCheckMusic          (0),
		m_numTracks                 (0),
		m_curTrack                  (0),
		m_lastTrack                 (0),
		m_musicEnabled              (false),
		m_style                     (MUSICSTYLE_PLAYLIST),
		m_playListPosition          (0),
		m_userTrack                 (0),
		m_autoRepeat                (true),
		m_musicAutoRestartDisabled  (false)
{
    if (g_theProfileDB)
    {
		m_sfxVolume     = static_cast<uint32>(g_theProfileDB->GetSFXVolume());
		m_voiceVolume   = static_cast<uint32>(g_theProfileDB->GetVoiceVolume());
		m_musicVolume   = static_cast<uint32>(g_theProfileDB->GetMusicVolume());
    }

	m_sfxSounds     = new PointerList<CivSound>;
	m_voiceSounds   = new PointerList<CivSound>;
	m_soundWalker   = new PointerList<CivSound>::Walker;

	InitSoundDriver();
}

SoundManager::~SoundManager()
{
    DumpAllSounds();
    CleanupSoundDriver();

    delete m_sfxSounds;
    delete m_voiceSounds;
    delete m_soundWalker;
}

void SoundManager::DumpAllSounds()
{
	if (m_sfxSounds) {
		m_sfxSounds->DeleteAll();
	}
	if (m_voiceSounds) {
		m_voiceSounds->DeleteAll();
	}
}

void SoundManager::InitSoundDriver() //DONE//
{
	SDL_AudioFormat  output_format   = SDL_AUDIO_S16;
	int     		 output_channels = 2;
	int     		 output_rate     = 22050;	// 22khz @ 16 Bit stereo

	SDL_SetPointerProperty(g_noLoopProps, MIX_PROP_PLAY_LOOPS_NUMBER, 0);

	if (!MIX_Init()) {
		SDL_Log("MIX_Init() failed: %s", SDL_GetError());
        return;
	}

	if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        SDL_Log("SDL_InitSubSystem(SDL_INIT_AUDIO) failed: %s", SDL_GetError());
        return;
    }

	SDL_AudioSpec spec;
    SDL_zero(spec);
	spec.format=output_format;
	spec.channels=output_channels;
	spec.freq=output_rate;

	m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);

	if (m_mixer == nullptr) {
        SDL_Log("MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec) failed: %s", SDL_GetError());
        return;
    }

	m_noSound = false;

	m_musicTrack = MIX_CreateTrack(m_mixer);

	SetVolume(SOUNDTYPE_SFX,   m_sfxVolume);
	SetVolume(SOUNDTYPE_VOICE, m_voiceVolume);
	SetVolume(SOUNDTYPE_MUSIC, m_musicVolume);
} //DONE//

void SoundManager::CleanupSoundDriver() //DONE//
{
	TerminateMusic();

	if (!m_noSound) {
		MIX_DestroyMixer(m_mixer); // This also calls SDL_QuitSubSystem(SDL_INIT_AUDIO) and MIX_DestroyTrack
		m_mixer = nullptr;
		m_musicTrack = nullptr;
	}

	SDL_DestroyProperties(g_noLoopProps);
	MIX_Quit();
} //DONE//

void SoundManager::CleanupRedbook() //DONE//
{
	if (m_oggTrack) {
		MIX_DestroyAudio(m_oggTrack);
		m_oggTrack = nullptr;
	}
} //DONE//

void SoundManager::ProcessRedbook()
{
	if (!g_theProfileDB->IsUseRedbookAudio()) return;

	if (!m_musicEnabled) return;

	if (GetTickCount() > m_timeToCheckMusic) {

		if(!MIX_TrackPlaying(m_musicTrack)) {
			if (m_curTrack != -1)
				PickNextTrack();

			if (m_curTrack != -1 && !m_musicAutoRestartDisabled)
				StartMusic(m_curTrack);
		}
		m_timeToCheckMusic = GetTickCount() + k_CHECK_MUSIC_PERIOD;
	}
}

void SoundManager::Process(const uint32 &target_milliseconds,
                           uint32 &used_milliseconds)
{
	CivSound *sound;

    sint32 start_time_ms = GetTickCount();

    if (m_noSound) {
        used_milliseconds = GetTickCount() - start_time_ms;
        return;
    }

	if (m_sfxSounds->GetCount() > 0) {
		m_soundWalker->SetList(m_sfxSounds);
		while (m_soundWalker->IsValid()) {
			sound = m_soundWalker->GetObj();
			Assert(sound);
			if (!sound) continue;

			if (sound->IsPlaying()) {
				if (!MIX_TrackPlaying(sound->GetTrack())) {
					m_soundWalker->Remove();

					delete sound;
				} else {
					m_soundWalker->Next();
				}
			}
		}
	}

	if (m_voiceSounds->GetCount() > 0) {

		m_soundWalker->SetList(m_voiceSounds);
		while (m_soundWalker->IsValid()) {
			sound = m_soundWalker->GetObj();
			Assert(sound);
			if (!sound) continue;

			if (sound->IsPlaying()) {
				if ((nullptr == sound->GetTrack()) ||
					(!MIX_TrackPlaying(sound->GetTrack()))) {
					m_soundWalker->Remove();
					delete sound;
				} else {
					m_soundWalker->Next();
				}
			}
		}
	}

	ProcessRedbook();

    used_milliseconds = GetTickCount() - start_time_ms;
}

bool FindSoundinList(PointerList<CivSound> * sndList, sint32 soundID)
{
    for
    (
	    PointerList<CivSound>::Walker walk(sndList);
	    walk.IsValid();
        walk.Next()
    )
    {
		if (walk.GetObj()->GetSoundID() == soundID)
		{
			return true;
		}
	}

	return false;
}

void
SoundManager::AddGameSound(const GAMESOUNDS &sound)
{
	sint32 id = gamesounds_GetGameSoundID(sound);
	AddSound(SOUNDTYPE_SFX, 0, id, 0, 0);
}

void
SoundManager::AddSound(const SOUNDTYPE &type,
                       const uint32 &associatedObject,
                       const sint32 &soundID, sint32 x, sint32 y) // though position is passed in x,y not adding AddCenterMap in here since it would not be appropriate in all cases (like e.g. mouse-click sound)
{
	if (m_noSound) return;

    bool        found   = false;
    CivSound *  sound   = new CivSound(associatedObject, soundID);

	switch (type)
    {
    default:
//  case SOUNDTYPE_MUSIC:
        break;

	case SOUNDTYPE_SFX:
		sound->SetVolume(m_sfxVolume);
		found = FindSoundinList(m_sfxSounds, soundID);
		if (!found)
		{
			m_sfxSounds->AddTail(sound);
		}
		break;

	case SOUNDTYPE_VOICE:
		sound->SetVolume(m_voiceVolume);
		found = FindSoundinList(m_voiceSounds, soundID);
		if (!found)
		{
			m_voiceSounds->AddTail(sound);
		}
		break;
	}

	if (found)
    {
        // This sound was already being played
        delete sound;
    }
    else
	{
		sound->SetTrack(MIX_CreateTrack(m_mixer));

		if (sound->GetTrack() == nullptr)
			SDL_Log("sound->setTrack(MIX_CreateTrack(m_mixer)) in SoundManager::AddSound failed: %s", SDL_GetError());

		else {
		if (MIX_SetTrackAudio(sound->GetTrack(), sound->GetAudio()))
				MIX_PlayTrack(sound->GetTrack(), g_noLoopProps);
		else
			SDL_Log("MIX_SetTrackAudio(sound->GetTrack(), sound->GetAudio()) in SoundManager::AddSound failed: %s", SDL_GetError());
		}
		sound->SetIsPlaying(true);
	}
}

void
SoundManager::AddLoopingSound(const SOUNDTYPE &type,
                              const uint32 &associatedObject,
                              const sint32 &soundID, sint32 x, sint32 y)
{
	if (m_noSound) return;

	CivSound *existingSound = FindLoopingSound(type, associatedObject);
	if (existingSound && (existingSound->GetSoundID() == soundID))
	{
		return;
	}

	CivSound *sound = NULL;
	if (existingSound && (existingSound->GetSoundID() == soundID)) {
		sound = existingSound;
	} else {
		sound = new CivSound(associatedObject, soundID);
		switch (type) {
			default:
				delete sound;
				return;

			case SOUNDTYPE_SFX:
				sound->SetTrack(MIX_CreateTrack(m_mixer));
				sound->SetVolume(m_sfxVolume);
				m_sfxSounds->AddTail(sound);
				break;

			case SOUNDTYPE_VOICE:
				sound->SetTrack(MIX_CreateTrack(m_mixer));
				sound->SetVolume(m_voiceVolume);
				m_voiceSounds->AddTail(sound);
				break;
		}
	}


	//int channel = Mix_PlayChannel(-1, sound->GetAudio(), -1);
	if (MIX_SetTrackAudio(sound->GetTrack(), sound->GetAudio()))
		MIX_PlayTrack(sound->GetTrack(), 0);
	else
		SDL_Log("MIX_SetTrackAudio(sound->GetTrack(), sound->GetAudio()) failed: %s", SDL_GetError());

	sound->SetIsLooping(true);
	sound->SetIsPlaying(true);
}

void
SoundManager::TerminateLoopingSound(const SOUNDTYPE &type,
                                    const uint32 &associatedObject)
{
	CivSound *existingSound = FindLoopingSound(type, associatedObject);

	if (!existingSound) return;

	MIX_Track *track = existingSound->GetTrack();
	if (track != nullptr){
		MIX_StopTrack(track, 0);
		MIX_DestroyTrack(track);
	}


	existingSound->SetTrack(nullptr);
}

void
SoundManager::TerminateAllLoopingSounds(const SOUNDTYPE &type)
{
	PointerList<CivSound>::PointerListNode *node = NULL;

	switch (type) {
	case SOUNDTYPE_SFX:
			node = m_sfxSounds->GetHeadNode();
		break;
	case SOUNDTYPE_VOICE:
			node = m_voiceSounds->GetHeadNode();
		break;
	case SOUNDTYPE_NONE:
	case SOUNDTYPE_MUSIC:
	default:
		break;
	}

	CivSound	*sound;

	while (node) {
		sound = node->GetObj();
		if (sound && sound->IsLooping()) {
			MIX_Track *track = sound->GetTrack();
			if (track != nullptr) {
				MIX_StopTrack(track, 0);
				MIX_DestroyTrack(track);
			}
			sound->SetTrack(nullptr);
		}
		node = node->GetNext();
	}
}

void
SoundManager::TerminateSounds(const SOUNDTYPE &type)
{
	PointerList<CivSound>::PointerListNode *node = NULL;

	switch (type) {
	case SOUNDTYPE_SFX:
			node = m_sfxSounds->GetHeadNode();
		break;
	case SOUNDTYPE_VOICE:
			node = m_voiceSounds->GetHeadNode();
		break;
	case SOUNDTYPE_NONE:
	case SOUNDTYPE_MUSIC:
	default:
		break;
	}

	CivSound	*sound;

	while (node) {
		sound = node->GetObj();
		if (sound) {
			MIX_Track *track = sound->GetTrack();
            if (track != nullptr) {
                MIX_StopTrack(track, 0);
				MIX_DestroyTrack(track);
            }
		}
		node = node->GetNext();
	}
}

void
SoundManager::TerminateAllSounds()
{
	TerminateSounds(SOUNDTYPE_SFX);
	TerminateSounds(SOUNDTYPE_VOICE);
}

void //DONE//
SoundManager::SetVolume(const SOUNDTYPE &type, const uint32 &volume)
{
	CivSound *sound;

	Assert(volume >= 0);
	Assert(volume <= SLIDER_FULL);

	if (m_noSound) return;

	switch (type) {
	case SOUNDTYPE_SFX:
		m_sfxVolume = volume;

		m_soundWalker->SetList(m_sfxSounds);
		while (m_soundWalker->IsValid()) {
			sound = m_soundWalker->GetObj();
			sound->SetVolume(volume);
			m_soundWalker->Next();
		}
		break;

	case SOUNDTYPE_VOICE:
		m_voiceVolume = volume;

		m_soundWalker->SetList(m_voiceSounds);
		while (m_soundWalker->IsValid()) {
			sound = m_soundWalker->GetObj();
			sound->SetVolume(volume);
			m_soundWalker->Next();
		}
		break;

	case SOUNDTYPE_MUSIC:
		m_musicVolume = volume;

		// Scale volume from 0-SLIDER_FULL to 0-MIX_MAX_VOLUME
		{
		float scaledVolume = (float)((double)volume * (double)MIX_MAX_VOLUME / SLIDER_FULL);
		if (scaledVolume > MIX_MAX_VOLUME)
			scaledVolume = (float)MIX_MAX_VOLUME;

		MIX_SetTrackGain(m_musicTrack, scaledVolume);
		}
		break;

	default:
		printf("SOUNDTYPE %d doesn't exist.\n", type);
		break;
	}
} //DONE//

void
SoundManager::SetMasterVolume(const uint32 &volume)
{
	if (m_noSound) return;

	CivSound *sound;

	m_soundWalker->SetList(m_sfxSounds);
	while (m_soundWalker->IsValid()) {
		sound = m_soundWalker->GetObj();
		sound->SetVolume(volume);
		m_soundWalker->Next();
	}
	m_sfxVolume = volume;

	m_soundWalker->SetList(m_voiceSounds);
	while (m_soundWalker->IsValid()) {
		sound = m_soundWalker->GetObj();
		sound->SetVolume(volume);
		m_soundWalker->Next();
	}
	m_voiceVolume = volume;
}

void SoundManager::DisableMusic() //DONE//
{
    m_musicEnabled = false;
    TerminateMusic();
} //DONE//

void SoundManager::EnableMusic() //DONE//
{
    m_musicEnabled = true;
} //DONE//

CivSound
*SoundManager::FindLoopingSound(const SOUNDTYPE &type,
                                const uint32 &associatedObject)
{
	switch (type) {
	case SOUNDTYPE_SFX:
		m_soundWalker->SetList(m_sfxSounds);
		break;
	case SOUNDTYPE_VOICE:
		m_soundWalker->SetList(m_voiceSounds);
		break;
	case SOUNDTYPE_NONE:
	case SOUNDTYPE_MUSIC:
	default:
		break;
	}

	while (m_soundWalker->IsValid()) {
		if (m_soundWalker->GetObj()->IsLooping() && m_soundWalker->GetObj()->GetAssociatedObject() == associatedObject) {
			CivSound *sound = m_soundWalker->GetObj();
			return sound;
		}
		m_soundWalker->Next();
	}

	return NULL;
}

CivSound
*SoundManager::FindSound(const SOUNDTYPE &type,
                         const uint32 &associatedObject)
{
	switch (type) {
	case SOUNDTYPE_SFX:
		m_soundWalker->SetList(m_sfxSounds);
		break;
	case SOUNDTYPE_VOICE:
		m_soundWalker->SetList(m_voiceSounds);
		break;
	case SOUNDTYPE_NONE:
	case SOUNDTYPE_MUSIC:
	default:
		break;
	}

	while (m_soundWalker->IsValid()) {
		if (m_soundWalker->GetObj()->GetAssociatedObject() == associatedObject) {
			CivSound *sound = m_soundWalker->GetObj();
			return sound;
		}
		m_soundWalker->Next();
	}

	return NULL;
}

const MUSICSTYLE
SoundManager::GetMusicStyle() const
{
    return m_style;
}

const sint32
SoundManager::GetPlayListPosition() const
{
    return m_playListPosition;
}

const sint32
SoundManager::GetLastTrack() const
{
    return m_lastTrack;
}

const sint32
SoundManager::GetUserTrack() const
{
    return m_userTrack;
}

const BOOL
SoundManager::IsAutoRepeat() const
{
    return m_autoRepeat;
}

const BOOL
SoundManager::IsMusicEnabled() const
{
    return m_musicEnabled;
}

void
SoundManager::SetAutoRepeat(const BOOL &autoRepeat)
{
    m_autoRepeat = autoRepeat;
    PickNextTrack();
}

void
SoundManager::SetLastTrack(const sint32 &track)
{
    m_lastTrack = track;
}

void
SoundManager::SetMusicStyle(const MUSICSTYLE &style)
{
    m_style = style;
    PickNextTrack();
}

void
SoundManager::SetPlayListPosition(const sint32 &pos)
{
    m_playListPosition = pos;
    PickNextTrack();
}

void
SoundManager::SetPosition(const SOUNDTYPE &type,
                          const uint32 &associatedObject,
                          const sint32 &x, const sint32 &y)
{
	PointerList<CivSound>::PointerListNode *node = NULL;

	sint32 volume = 0;

	switch (type)
    {
	case SOUNDTYPE_SFX:
		node    = m_sfxSounds->GetHeadNode();
		volume  = m_sfxVolume;
		break;
	case SOUNDTYPE_VOICE:
		node    = m_voiceSounds->GetHeadNode();
		volume  = m_voiceVolume;
		break;
	case SOUNDTYPE_NONE:
	case SOUNDTYPE_MUSIC:
	default:
		break;
	}

	while (node)
    {
	    CivSound *  sound = node->GetObj();

		if (sound && (sound->GetAssociatedObject() == associatedObject))
        {
            MIX_Audio * myChunk = sound->GetAudio();
            // Why set the volume again?
		}

		node = node->GetNext();
	}
}

void SoundManager::SetUserTrack(const sint32 &trackNum)
{
    m_userTrack = trackNum;
    PickNextTrack();
}

void SoundManager::StartMusic()
{
    StartMusic(m_curTrack);
}

void SoundManager::StartMusic(const sint32 &InTrackNum)
{
	m_musicAutoRestartDisabled = false;

	if (!g_theProfileDB->IsUseRedbookAudio()) return;

	if (m_noSound) return;

	if (m_curTrack == -1) return;

	char buf[60];
	if(!m_numTracks) {
		// first search number of tracks
		sint32 numTracks = 1;
		do {
			numTracks++;
			sprintf(buf, "music/Track%02d.ogg", numTracks);
		} while(c3files_PathIsValid(buf));
		m_numTracks = numTracks-1; // start at 2
	}
	// setting up
	if (m_numTracks <= s_startTrack) return;
	
	sint32 trackNum = InTrackNum;
	if (trackNum < 0) trackNum = 0;
	if (trackNum > m_numTracks) trackNum = m_numTracks;

	m_curTrack = trackNum;

	snprintf(buf, sizeof(buf), "music/Track%02d.ogg", m_curTrack+1);
	// clean previous if there
	CleanupRedbook();
	m_oggTrack = MIX_LoadAudio(m_mixer, CI_FixName(buf), false);
	if(m_oggTrack) {
		MIX_SetTrackAudio(m_musicTrack, m_oggTrack);
		MIX_PlayTrack(m_musicTrack, g_noLoopProps);
	}
	else
		printf("Error, music track %s not found\n", buf);
}

void SoundManager::TerminateMusic(void) //DONE//
{
	if (m_noSound) return;

	if (m_musicTrack) {
		MIX_StopTrack(m_musicTrack, 0);
	}

	CleanupRedbook();

	m_musicAutoRestartDisabled = true;
} //DONE//

void SoundManager::PickNextTrack(void)
{
	switch (m_style)
    {
    default:
//	case MUSICSTYLE_PLAYLIST:
		++m_playListPosition;
		if (m_playListPosition >= g_thePlayListDB->GetNumSongs())
        {
			m_playListPosition = 0;

			if (!m_autoRepeat)
            {
				m_curTrack = -1;
				return;
			}
		}
		m_curTrack = s_startTrack + g_thePlayListDB->GetSong(m_playListPosition);
		break;

	case MUSICSTYLE_RANDOM:
		m_curTrack = (1 + s_startTrack) + rand() % (m_numTracks - (1 + s_startTrack));
		break;

	case MUSICSTYLE_USER:
		m_curTrack = (1 + s_startTrack) + m_userTrack;
		if (!m_autoRepeat && m_curTrack >= m_lastTrack)
        {
			m_curTrack = -1;
            return;
		}
		break;
	}

	m_lastTrack = m_curTrack;
}

void SoundManager::ReleaseSoundDriver() // only used if __AUI_USE_DIRECTX__ is set.
{
	if (m_noSound) return;
	TerminateAllSounds();
}