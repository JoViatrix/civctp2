//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : General declarations
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
// - Use SDL for sound and cdrom (originally)
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragmas commented out
// - includes fixed for case sensitive filesystems
// - sdl sound and cdrom
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "civsound.h"
#include "SoundRecord.h"
#include "prjfile.h"

#include <SDL3/SDL.h>

extern ProjectFile  *g_SoundPF;

CivSound::CivSound(const uint32 &associatedObject, const sint32 &soundID)
  : m_Audio(nullptr), m_soundTrack(nullptr),
    m_associatedObject(associatedObject),
	m_volume(MIX_MAX_VOLUME)

{
    const char *fname;
	if(soundID < 0)
		fname = 0;
	else
		fname = g_theSoundDB->Get(soundID)->GetValue();

	m_soundID = soundID;
    m_isPlaying = FALSE;
    m_isLooping = FALSE;

    if (0 == fname) {
        m_soundFilename[0] = 0;
        m_dataptr = NULL;
        m_datasize = 0;
        return;
    }

    strcpy(m_soundFilename, fname);

    size_t      l_dataSize = 0;
    m_dataptr   = g_SoundPF->getData(m_soundFilename, l_dataSize);
    m_datasize  = static_cast<sint32>(l_dataSize);
    m_Audio = MIX_LoadAudio_IO(NULL, SDL_IOFromMem(m_dataptr, static_cast<int>(m_datasize)), 0, 1);
}

CivSound::~CivSound()
{
	if (m_Audio) {
		MIX_DestroyAudio(m_Audio);
		MIX_DestroyTrack(m_soundTrack);
	}

    if (m_dataptr) {
        g_SoundPF->freeData(m_dataptr);
	}
}

const uint32
CivSound::GetAssociatedObject() const
{
	return m_associatedObject;
}

MIX_Audio *
CivSound::GetAudio() const
{
	return m_Audio;
}

MIX_Track *
CivSound::GetTrack() const
{
	return m_soundTrack;
}


MBCHAR
*CivSound::GetSoundFilename()
{
	return m_soundFilename;
}

const sint32
CivSound::GetSoundID() const
{
	return m_soundID;
}

const sint32
CivSound::GetVolume()
{
	return m_volume;
}

const BOOL
CivSound::IsLooping()
{
	return m_isLooping;
}

const BOOL
CivSound::IsPlaying() const
{
	return m_isPlaying;
}

void
CivSound::SetTrack(MIX_Track *track)
{
    m_soundTrack = track;
}

void
CivSound::SetIsLooping(const BOOL &looping)
{
	m_isLooping = looping;
}

void
CivSound::SetIsPlaying(const BOOL &is)
{
	m_isPlaying = is;
}

void
CivSound::SetVolume(const sint32 &volume)
{
    if (0 == m_Audio) return;

	sint32 vol = volume;

	if (vol < 0)
	{
		SDL_Log("The volume asked to set %d < 0. Set to 0.", volume);
		vol = 0;
	}
	if (vol > 10)
	{
		SDL_Log("The volume asked to set %d > 10. Set to 10.", volume);
		vol = 10;
	}

	m_volume = vol;
}

const bool
CivSound::Play(SDL_PropertiesID properties)
{
	// Scale volume from 0-SLIDER_FULL to 0-MIX_MAX_VOLUME
	float scaledVolume = (float)((double)m_volume * (double)MIX_MAX_VOLUME / 10.0f); //SLIDER_FULL=10

	if (scaledVolume > MIX_MAX_VOLUME)
		scaledVolume = (float)MIX_MAX_VOLUME;

	if (!MIX_SetTrackGain(m_soundTrack, scaledVolume))
	{
		SDL_Log("MIX_SetTrackGain(m_soundTrack, scaledVolume) failed: %s", SDL_GetError());
		return false;
	}

	if (!MIX_PlayTrack(m_soundTrack, properties))
	{
		SDL_Log("MIX_PlayTrack(m_soundTrack, properties) failed: %s", SDL_GetError());
		return false;
	}
	m_isPlaying = true;
	return true;
}