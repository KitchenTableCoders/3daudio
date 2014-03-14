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
	result = mSystem->init( nChannels, flags, NULL );
	
	// If the selected speaker mode isn't supported by this sound card, switch it back to stereo
	if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)
	{
		app::console() << "WARNING: Falling back to basic stereo" << endl;
		FMODErrorCheck(mSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO));
		FMODErrorCheck(mSystem->init(100, FMOD_INIT_NORMAL, 0));
	}
	
	FMODErrorCheck(mSystem->getMasterChannelGroup(&mMasterChannelGroup));
}



string AudioDevice::registerSound(ci::DataSourceRef dataSource, bool looping, bool asStream)
{
	ci::fs::path relativePath = dataSource->getFilePath();
	if( relativePath.empty() )
		relativePath = ci::fs::path( dataSource->getFilePathHint() );
	return registerSound(relativePath, looping, asStream);
}

string AudioDevice::registerSound(ci::fs::path pathToSound, bool looping, bool asStream)
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
	
	if( looping ) {
		FMODErrorCheck( mSounds.at(name)->setMode( FMOD_LOOP_NORMAL ) );
	} else {
		FMODErrorCheck( mSounds.at(name)->setMode( FMOD_LOOP_OFF ) );
	}
	
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
    int samplerate;
    FMOD_SOUND_FORMAT format;
    int numoutputchannels;
    int maxinputchannels;
    FMOD_DSP_RESAMPLER resamplemethod;
    int bits;
    mSystem->getSoftwareFormat(&samplerate, &format, &numoutputchannels, &maxinputchannels, &resamplemethod, &bits);
    return numoutputchannels-getNumChannelsPlaying();
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