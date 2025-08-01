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

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __CIVSOUND_H__
#define __CIVSOUND_H__

#ifdef STREAM_TYPE
#error
#undef STREAM_TYPE
#endif

#include <SDL3_mixer/SDL_mixer.h>

#ifndef MIX_MAX_VOLUME
#define MIX_MAX_VOLUME 1.0f
#endif

class CivSound {
public:
	CivSound(const uint32 &associatedObject, const sint32 &soundID);
	~CivSound();

	const uint32 GetAssociatedObject() const;
	MIX_Audio    *GetAudio() const;
    void         SetChannel(const int &channel);
    const int    GetChannel() const;
	void		 SetTrack(MIX_Track *track);
	MIX_Track    *GetTrack() const;
	MBCHAR       *GetSoundFilename();
	const BOOL   IsPlaying() const;
	void         SetIsPlaying(const BOOL &is);
	const sint32 GetSoundID() const;
	void         SetIsLooping(const BOOL &looping);
	const BOOL   IsLooping();
	const sint32 GetVolume();
	void         SetVolume(const sint32 &volume);

private:
	MIX_Audio      *m_Audio;
	MIX_Track      *m_soundTrack;
    int             m_Channel;
	uint32 			m_associatedObject;
	MBCHAR			m_soundFilename[_MAX_PATH];
	BOOL			m_isPlaying;
	BOOL			m_isLooping;
	sint32			m_soundID;
	sint32			m_volume;
	void			*m_dataptr;
    size_t          m_datasize;
};

#endif // __CIVSOUND_H__
