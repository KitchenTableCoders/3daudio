//
//  AudioDevice.cpp
//  LookAroundYou
//
//  Created by Jeffrey Crouse on 3/14/14.
//
//

#include "AudioDevice.h"
#include <sstream>

AudioDevice::AudioDevice()
{
    FMODErrorCheck(FMOD::System_Create( &mSystem ));
}

AudioDevice::~AudioDevice()
{
	for(auto& sound : mSounds) {
		FMODErrorCheck(sound.second->release());
	}
	
	FMODErrorCheck( mSystem->close() );
	FMODErrorCheck( mSystem->release() );
}

void AudioDevice::setup(string deviceName, int nChannels)
{
    auto device = AudioDevice::deviceMap.find(deviceName);
	if(device==AudioDevice::deviceMap.end()) {
		app::console() << "WARNING: audio device " << deviceName << " not found. ";
		deviceName = AudioDevice::deviceMap.begin()->first;
		app::console() << "Using " << deviceName << " instead" << endl;
	}
	setup( AudioDevice::deviceMap[deviceName] );
}


void AudioDevice::setup(int deviceID, int nChannels)
{
    mNumChannels = nChannels;
  	if(deviceID >= 0 && deviceID <= deviceMap.size()-1)
	{
		mSystem->setDriver(deviceID);
        mSystem->getDriverInfo(deviceID, deviceName, 256, &guid);
		
		// Get the capabilities of the sound card "mDeviceID"
		int controlpaneloutputrate;
		FMOD_SPEAKERMODE speakerMode;
		FMOD_CAPS caps; // capabilities
		FMODErrorCheck(mSystem->getDriverCaps(deviceID, &caps, &controlpaneloutputrate, &speakerMode));
		FMODErrorCheck(mSystem->setSpeakerMode(speakerMode));
	}
	else
	{
		app::console() << "Error initializing FMOD Audio Device. Device ID (" << deviceID << ") not in range." << endl;
		FMODErrorCheck(mSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND));
	}
    
    FMOD_RESULT result;
	FMOD_INITFLAGS flags = FMOD_INIT_NORMAL;
	result = mSystem->init( mNumChannels, flags, NULL );
	
    
	// If the selected speaker mode isn't supported by this sound card, switch it back to stereo
	if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)
	{
		app::console() << "WARNING: Falling back to basic stereo" << endl;
		FMODErrorCheck(mSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO));
		FMODErrorCheck(mSystem->init(100, FMOD_INIT_NORMAL, 0));
	}
	
	FMODErrorCheck(mSystem->getMasterChannelGroup(&mMasterChannelGroup));
    
    FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };
    FMOD_VECTOR forward = { 0.0f, 0.0f, 1.0f };
    FMOD_VECTOR listenerpos = { 0.0f, 0.0f, 0.0f };
    FMOD_VECTOR vel = {0, 0, 0};
    FMODErrorCheck(mSystem->set3DListenerAttributes(0, &listenerpos, &vel, &forward, &up));
}



string AudioDevice::registerSound(ci::DataSourceRef dataSource, bool looping, bool is3d, bool asStream)
{
	ci::fs::path relativePath = dataSource->getFilePath();
	if( relativePath.empty() )
		relativePath = ci::fs::path( dataSource->getFilePathHint() );
	return registerSound(relativePath, is3d, looping, asStream);
}

string AudioDevice::registerSound(ci::fs::path pathToSound, bool looping, bool is3d, bool asStream)
{
	if ( !boost::filesystem::exists( pathToSound ) ) {
		app::console() << "[NOTICE] " << pathToSound << " doesn't exist" << endl;
		return "";
	}
	
	std::stringstream namestream;
	namestream << deviceName << "-" << pathToSound.filename().string() << "-" << mSounds.size();
	string name = namestream.str();
	
	app::console() << "Adding " << name << endl;
	
	if(mSounds.find(name)!=mSounds.end()) {
		app::console() << "[WARNING] " << name << " already exists on this device. Re-adding." << endl;
	}
    
	if( asStream ) {
		FMODErrorCheck( mSystem->createStream( pathToSound.string().c_str(), FMOD_DEFAULT, NULL, &mSounds[name]) );
	} else {
		FMODErrorCheck( mSystem->createSound( pathToSound.string().c_str(), FMOD_DEFAULT, NULL, &mSounds[name]) );
	}
	
    FMOD_MODE mode;
    mode = is3d ? FMOD_3D|FMOD_3D_LINEARSQUAREROLLOFF : FMOD_2D;
	mode |= looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    FMODErrorCheck( mSounds.at(name)->setMode( mode ) );

	return name;
}

unsigned int AudioDevice::getNumChannelsPlaying()
{
	int n;
	FMODErrorCheck( mSystem->getChannelsPlaying(&n) );
	return n;
}

unsigned int AudioDevice::getNumRemainingChannels()
{
    return mNumChannels-getNumChannelsPlaying();
}

SoundInstanceRef AudioDevice::getSoundInstance(string name, float volume )
{
	auto sound = mSounds.find(name);
	if(sound==mSounds.end()) {
		app::console() << "[NOTICE]: " << name << " doesn't exist on device." << endl;
		return nullptr;
	}
    
	if(getNumRemainingChannels()<1) {
		app::console() << "[WARNING]: No remaining channels on device. Not making SoundInstance" << endl;
		return nullptr;
	}
	
	return make_shared<SoundInstance>(this, sound->second, volume );
}

void AudioDevice::update()
{
    mSystem->update();
}


// FMOD_PRESET_OFF, FMOD_PRESET_UNDERWATER, FMOD_PRESET_STONEROOM, FMOD_PRESET_ARENA, etc.
void AudioDevice::setAmbientReverb(FMOD_REVERB_PROPERTIES prop1)
{
	FMODErrorCheck(mSystem->setReverbAmbientProperties(&prop1));
}

void AudioDevice::setVolume(float vol)
{
	FMODErrorCheck(mMasterChannelGroup->setVolume(vol));
}

float AudioDevice::getVolume()
{
	float vol;
	FMODErrorCheck(mMasterChannelGroup->getVolume(&vol));
	return vol;
}






//
//  Static
//


std::map<string,int> AudioDevice::deviceMap = AudioDevice::getDeviceMap();

void AudioDevice::printDeviceMap()
{
	app::console() << "===========================" << endl;
	app::console() << "Listing audio devices:" << endl;
	for (std::map<string,int>::iterator it=deviceMap.begin(); it!=deviceMap.end(); ++it){
		app::console() << "(" << it->second << ") " << it->first << endl;
	}
	app::console() << "===========================" << endl;
}

/**
 * Create a map where the keys are the names of the devices
 * and the values are the id (0-x where x = num of devices)
 */
map<string,int> AudioDevice::getDeviceMap()
{
	FMOD_RESULT result;
	FMOD::System* sys;
	result = FMOD::System_Create( &sys );
	FMODErrorCheck(result);
	
	int numDrivers;
	result = sys->getNumDrivers(&numDrivers);
	FMODErrorCheck(result);
	
	map<string,int> deviceMap;
	//app::console() << "===========================" << endl;
	//app::console() << "Listing audio devices:" << endl;
	for(int i=0; i<numDrivers; i++)
	{
		FMOD_GUID guid;
		char deviceName[256];
		sys->getDriverInfo(i, deviceName, 256, &guid);
		//app::console() << "(" << i << ") " << deviceName << endl;
		deviceMap[string(deviceName)] = i;
	}
	//app::console() << "===========================" << endl;
	FMODErrorCheck( sys->close() );
	FMODErrorCheck( sys->release() );
	return deviceMap;
}