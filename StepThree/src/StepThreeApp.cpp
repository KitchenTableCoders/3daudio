#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"
#include "FMODCommon.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// In this example
// 1. Loading streming sounds
// 2. Volume control
// 3. Channel groups
// 4. getCurrentSound at exit (perhaps bad example...)
// 5. Why test for FMOD_OK?
// 6. mSystem->update()

class StepThreeApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );
    void keyUp( KeyEvent event );
	void update();
	void draw();
	void shutdown() override;
    
    FMOD::System* mSystem;
    vector<FMOD::Sound*> mSounds;
    FMOD::ChannelGroup* mFXGroup;
    FMOD::ChannelGroup* mMasterChannelGroup;
    FMOD::Channel* mMusic;
    float mMusicVolume;
    float mFXVoume;
    float mMasterVolume;
    bool bReverb;
};

void StepThreeApp::setup()
{
    FMODListDevices();
    FMODErrorCheck(FMOD::System_Create( &mSystem ));
    FMODErrorCheck(mSystem->setDriver(0));
	FMOD_INITFLAGS flags = FMOD_INIT_NORMAL; // right-click, Go To Definition
	FMODErrorCheck(mSystem->init( 32, flags, NULL ));
    bReverb = false;

    vector<fs::path> paths;
    paths.push_back( getAssetPath("gong-loud-hit.mp3") );
    paths.push_back( getAssetPath("orch-hit.wav") );
    paths.push_back( getAssetPath("trumpethit07.wav") );
    
    mFXVoume = 0.5;
    FMODErrorCheck(mSystem->createChannelGroup(NULL, &mFXGroup));
    for(auto& path: paths) {
        FMOD::Sound* sound;
        mSystem->createSound( path.string().c_str(), FMOD_SOFTWARE, NULL, &sound );
        
        // You can change the mode of sounds at any time.
        FMOD_MODE mode = FMOD_SOFTWARE | FMOD_2D | FMOD_LOOP_OFF;
        FMODErrorCheck( sound->setMode( mode ) );
        
        mSounds.push_back(sound);
    }
    
    
    // Load in the music as stream from disk. It is not loaded into memory.
    mMusicVolume = 0.5;
    FMOD::Sound* music;
    FMOD_MODE mode = FMOD_SOFTWARE | FMOD_2D | FMOD_LOOP_NORMAL;
    FMODErrorCheck( mSystem->createStream( getAssetPath("Raymond Scott - Lady Gaylord.mp3").string().c_str(), mode, NULL, &music) );
    mSystem->playSound( FMOD_CHANNEL_FREE, music, false, &mMusic );
    
    
    mMasterVolume = 1.0;
    FMODErrorCheck(mSystem->getMasterChannelGroup(&mMasterChannelGroup));
    
}

void StepThreeApp::mouseDown( MouseEvent event )
{
    FMOD::Sound* sound = mSounds[Rand::randInt(mSounds.size())];
    
    // Channels inherit the modes of sounds.
    FMOD::Channel* channel;
    mSystem->playSound( FMOD_CHANNEL_FREE, sound, true, &channel );
    channel->setChannelGroup(mFXGroup);
    channel->setPaused(false);
}

void StepThreeApp::keyUp( KeyEvent event )
{
    
    if(event.getChar()=='M' && mMusicVolume < 1) {
        mMusicVolume += 0.1;
    }
    if(event.getChar()=='m' && mMusicVolume > 0) {
        mMusicVolume -= 0.1;
    }

    if(event.getChar()=='F' && mFXVoume < 1) {
        mFXVoume += 0.1;
    }
    if(event.getChar()=='f' && mFXVoume > 0) {
        mFXVoume -= 0.1;
    }
    if(event.getCode()==KeyEvent::KEY_UP && mMasterVolume < 1) {
        mMasterVolume += 0.1;
    }
    if(event.getCode()==KeyEvent::KEY_DOWN && mMasterVolume >0) {
        mMasterVolume -= 0.1;
    }
    
    if(event.getChar()=='r' || event.getChar()=='R') {
        bReverb = !bReverb;

        // FMOD_PRESET_OFF, FMOD_PRESET_UNDERWATER, FMOD_PRESET_STONEROOM, FMOD_PRESET_ARENA, etc.
        FMOD_REVERB_PROPERTIES prop1;
        
        if(bReverb) prop1 = FMOD_PRESET_ARENA;
        else prop1 = FMOD_PRESET_OFF;

        if(mSystem->setReverbAmbientProperties(&prop1)!=FMOD_OK) {
            console() << "Couldn't set reverb!" << endl;
        }
    }
}

void StepThreeApp::update()
{
    // TO DO: Add boolean for when volume is changed so that we're not constantly setting volume.
    
    if(mMusic->setVolume(mMusicVolume)!=FMOD_OK) {
        console() << "Unable to set music volume" << endl;
    }
    
    if(mFXGroup->setVolume(mFXVoume)!=FMOD_OK) {
        console() << "Unable to set FX volume" << endl;
    }
    
    if(mMasterChannelGroup->setVolume(mMasterVolume)!=FMOD_OK) {
        console() << "Unable to set master volume" << endl;
    }
    
    mSystem->update();
}

void StepThreeApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
    
    gl::color(1.0, 1.0, 1.0);
    gl::enableAlphaBlending();
    
    gl::drawString("Music Volume: "+cinder::toString(mMusicVolume), Vec2f(10, 20));
    gl::drawString("FX Volume: "+cinder::toString(mFXVoume), Vec2f(10, 40));
    gl::drawString("Master Volume: "+cinder::toString(mMasterVolume), Vec2f(10, 60));
    gl::drawString("Reverb: "+string(bReverb?"ON":"OFF"), Vec2f(10, 80));
    gl::disableAlphaBlending();
}

void StepThreeApp::shutdown()
{
    FMOD::Sound* music;
    mMusic->getCurrentSound(&music);
    music->release();
    
    // 4. Release resources!
    for(auto& sound : mSounds) {
		FMODErrorCheck(sound->release());
	}
	FMODErrorCheck( mSystem->close() );
	FMODErrorCheck( mSystem->release() );
}

CINDER_APP_NATIVE( StepThreeApp, RendererGl )
