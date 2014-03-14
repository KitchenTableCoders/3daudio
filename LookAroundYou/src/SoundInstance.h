//
//  DeviceSoundInstance.h
//
//  Created by jeff-cr on 11/21/13.
//
//	Represents a single FMOD::Channel*


#pragma once
#include "cinder/Color.h"
#include "cinder/Timeline.h"
#include "cinder/Tween.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"
#include "cinder/app/App.h"

#include "FMODCommon.h"

//#define USE_DSP_EFFECTS

class AudioDevice;
class SoundInstance {
public:
	SoundInstance(AudioDevice* device, FMOD::Sound* sound, float volume );
	virtual ~SoundInstance();
	bool is3d();
	
    //	void setLoop(bool loop=true);
	void play() { setPaused(false); }
	void pause() { setPaused(true); }

	void	setPaused(bool paused);
	void	setFrequency(float freq);
	
	void	setVolume(float vol);
	float	getVolume() const;
	
	
	bool	isPlaying();
	bool	isAudible() { return getAudibility() > 0.01; }
	double			getPositionSec() { return getPositionMs()/1000.0; }
	unsigned int	getPositionMs();
	double			getLengthSec() { return getLengthMs()/1000.0; }
	unsigned int	getLengthMs() const { return length; }
	double			getPositionNorm() { return getPositionSec()/getLengthSec(); }

	
	float	getAudibility();
	int		getLoopCount();
	
    
#ifdef USE_DSP_EFFECTS
	// Flange
	void addFlangeDSP();
	void setFlangeWetmix(float val);
	void setFlangeRate(float val);
	
	// Echo
	void addEchoDSP();
	void setEchoWetmix(float val);
	void setEchoDelay(float val);
	void setEchoDecay(float val);
	
	// Pitch
	void addPitchDSP();
	void setPitch(float pitch);
#endif
	
protected:
	
	AudioDevice*	mDevice;
	FMOD::Channel*	mChannel;
    
	FMOD_MODE mode;
	FMOD_SOUND_TYPE type;
	FMOD_SOUND_FORMAT format;
	int bits;
	unsigned int length;	// Length of sound in miliseconds.
	int nchannels;
	
	
#ifdef USE_DSP_EFFECTS
	FMOD::DSP* dspflange;
	FMOD::DSP* dspecho;
	FMOD::DSP* dsppitch;
#endif
};


typedef shared_ptr<class SoundInstance> SoundInstanceRef;
