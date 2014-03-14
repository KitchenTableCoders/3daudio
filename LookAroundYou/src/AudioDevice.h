//
//  AudioDevice.h
//  LookAroundYou
//
//  Created by Jeffrey Crouse on 3/14/14.
//
//

#pragma once
#include "cinder/Timeline.h"
#include "FMODCommon.h"
#include "SoundInstance.h"

using namespace cinder;

class AudioDevice {
public:
    
    AudioDevice();
    ~AudioDevice();
    void setup(int deviceID, int nChannels=32);
    void setup(string deviceName, int nChannels=32);
    void update();
    
    unsigned int getNumChannelsPlaying();
    unsigned int getNumRemainingChannels();
    
    FMOD::System* mSystem;
    FMOD::ChannelGroup* mMasterChannelGroup;
    std::map<std::string,FMOD::Sound*> mSounds;
    
    char deviceName[256];
    FMOD_GUID guid;
    
    string registerSound( ci::fs::path pathToSound,
                         bool looping=false, bool asStream=false );
	string registerSound( ci::DataSourceRef dataSource,
                         bool looping=false, bool asStream=false );
    SoundInstanceRef getSoundInstance(std::string name,
                                      float volume );
    
    static map<string,int> deviceMap;
	static map<string,int> getDeviceMap();
    static void printDeviceMap();
};


typedef shared_ptr<class AudioDevice> AudioDeviceRef;