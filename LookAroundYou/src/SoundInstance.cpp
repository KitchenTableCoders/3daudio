//
//  SoundInstance.cpp
//
//  Created by jeff-cr on 11/21/13.
//
//

#include "SoundInstance.h"
#include "AudioDevice.h"

#define INVALID_CHANNEL_WARNING(task) app::console() << "[WARNING] Attemping to " << task << " on an invalid channel" << endl;

using namespace ci;

SoundInstance::SoundInstance(AudioDevice* device, FMOD::Sound* sound, float volume )
: mDevice( device )
{
	//bFadeInProgress = false;
    
	mChannel = nullptr;
#ifdef USE_DSP_EFFECTS
	dspecho = NULL;
	dsppitch = NULL;
	dspflange = NULL;
#endif
	
	FMODErrorCheck(mDevice->mSystem->playSound(FMOD_CHANNEL_FREE, sound, false, &mChannel));
	
#ifdef USE_DSP_EFFECTS
	// HACK??  Add 1 second to the end of the sound to account for effects with tails.
	// ie: without this, echos will be cut off if sounds don't already have silence at the end.
	FMODErrorCheck(mChannel->setDelay(FMOD_DELAYTYPE_END_MS, 1000, 1000));
#endif
    
	//setVolume( volume );
	
	FMODErrorCheck(sound->getLength(&length, FMOD_TIMEUNIT_MS));
	FMODErrorCheck(sound->getFormat(&type, &format, &nchannels, &bits));
}

SoundInstance::~SoundInstance()
{
#ifdef USE_DSP_EFFECTS
	if(dspflange) dspflange->release();
	if(dspecho) dspecho->release();
	if(dsppitch) dsppitch->release();
#endif
    
	bool isPlaying;
	if (mChannel->isPlaying(&isPlaying) == FMOD_OK && isPlaying) {
		mChannel->stop();
	}
}

#pragma mark Basic Getters and Setters

void SoundInstance::setVolume(float vol)
{
	if(mChannel->setVolume(vol)!=FMOD_OK) {
		INVALID_CHANNEL_WARNING("set volume");
	}
}

float SoundInstance::getVolume() const
{
    float vol;
    if( mChannel->getVolume(&vol)==FMOD_OK ) {
        return vol;
    } else {
        return 0;
    }
}

bool SoundInstance::is3d()
{
	return mChannel->getMode(&mode)==FMOD_OK && (mode & FMOD_3D) == FMOD_3D;
}

bool SoundInstance::isPlaying()
{
	bool playing;
	if( mChannel->isPlaying(&playing) == FMOD_OK ) {
		return playing;
	} else {
		return false;
	}
}

int SoundInstance::getLoopCount()
{
	int loopcount;
	if(mChannel->getLoopCount(&loopcount) == FMOD_OK) {
		return loopcount;
	} else {
		INVALID_CHANNEL_WARNING("get loop count");
		return 0;
	}
}

void SoundInstance::setPaused(bool paused)
{
	if(mChannel->setPaused(paused)!=FMOD_OK) {
		INVALID_CHANNEL_WARNING("set paused state");
	}
}

void SoundInstance::setFrequency(float freq)
{
	if(mChannel->setFrequency(freq)!=FMOD_OK) {
		INVALID_CHANNEL_WARNING("set frequency of");
	}
}

unsigned int SoundInstance::getPlayheadMs()
{
	unsigned int position_ms;
	if(mChannel->getPosition(&position_ms, FMOD_TIMEUNIT_MS) == FMOD_OK) {
		return position_ms;
	} else {
		INVALID_CHANNEL_WARNING("set position of");
		return 0;
	}
}

float SoundInstance::getAudibility()
{
	float audibility;
	if(mChannel->getAudibility(&audibility)==FMOD_OK)
		return audibility;
	else {
		INVALID_CHANNEL_WARNING("get audibility of");
		return 0;
	}
}

void SoundInstance::set3DAttributes(ci::Vec3f pos, ci::Vec3f vel) {
    FMOD_VECTOR _pos = toFMOD(pos);
    FMOD_VECTOR _vel = toFMOD(vel);
    if(mChannel->set3DAttributes(&_pos, &_vel)!=FMOD_OK) {
        INVALID_CHANNEL_WARNING("set 3D attributes of");
    }
}

void SoundInstance::set3DFalloff(float min, float max) {
    if(mChannel->set3DMinMaxDistance(min, max) != FMOD_OK) {
        INVALID_CHANNEL_WARNING("set 3D falloff of");
    }
}

#pragma mark DSP stuff
#ifdef USE_DSP_EFFECTS

void SoundInstance::addPitchDSP()
{
	FMODErrorCheck(mDevice->mSystem->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsppitch));
	FMODErrorCheck(mChannel->addDSP(dsppitch, 0));
}

void SoundInstance::addFlangeDSP()
{
	FMODErrorCheck(mDevice->mSystem->createDSPByType(FMOD_DSP_TYPE_FLANGE, &dspflange));
	FMODErrorCheck(mChannel->addDSP(dspflange, 0));
}

void SoundInstance::setFlangeWetmix(float val)
{
	if(!dspflange) addFlangeDSP();
	FMODErrorCheck(dspflange->setParameter(FMOD_DSP_FLANGE_WETMIX, val));
}

void SoundInstance::setFlangeRate(float val)
{
	if(!dspflange) addFlangeDSP();
	FMODErrorCheck(dspflange->setParameter(FMOD_DSP_FLANGE_RATE, val));
}

void SoundInstance::setPitch(float pitch)
{
	if(!dsppitch) addPitchDSP();
	FMODErrorCheck(dsppitch->setParameter(FMOD_DSP_PITCHSHIFT_PITCH, pitch));
}

void SoundInstance::addEchoDSP()
{
	FMODErrorCheck(mDevice->mSystem->createDSPByType(FMOD_DSP_TYPE_ECHO, &dspecho));
	FMODErrorCheck(mChannel->addDSP(dspecho, 0));
	setEchoWetmix(0);
}

void SoundInstance::setEchoWetmix(float val)
{
	if(!dspecho) addEchoDSP();
	FMODErrorCheck(dspecho->setParameter(FMOD_DSP_ECHO_WETMIX, val));
}

void SoundInstance::setEchoDelay(float val)
{
	if(!dspecho) addEchoDSP();
	FMODErrorCheck(dspecho->setParameter(FMOD_DSP_ECHO_DELAY, val));
}

void SoundInstance::setEchoDecay(float val)
{
	if(!dspecho) addEchoDSP();
	FMODErrorCheck(dspecho->setParameter(FMOD_DSP_ECHO_DECAYRATIO, val));
}
#endif
