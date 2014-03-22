#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "FMODCommon.h"


using namespace ci;
using namespace ci::app;
using namespace std;

// In this example
// 1. Loading multiple sounds
// 2. FMOD_MODE
// 3. Max channels test

class StepTwoApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    void shutdown() override;
    
    FMOD::System* mSystem;
    vector<FMOD::Sound*> mSounds;
};

void StepTwoApp::setup()
{
    FMODListDevices();
    FMODErrorCheck(FMOD::System_Create( &mSystem ));
    FMODErrorCheck(mSystem->setDriver(0));
	FMOD_INITFLAGS flags = FMOD_INIT_NORMAL; // right-click, Go To Definition
	FMODErrorCheck(mSystem->init( 32, flags, NULL ));

    vector<fs::path> paths;
    paths.push_back( getAssetPath("gong-loud-hit.mp3") );
    paths.push_back( getAssetPath("orch-hit.wav") );
    paths.push_back( getAssetPath("trumpethit07.wav") );
    
    for(auto& path: paths) {
        FMOD::Sound* sound;
        mSystem->createSound( path.string().c_str(), FMOD_SOFTWARE, NULL, &sound );
        
        // You can change the mode of sounds at any time.
        FMOD_MODE mode = FMOD_SOFTWARE | FMOD_2D | FMOD_LOOP_OFF;
        FMODErrorCheck( sound->setMode( mode ) );
        
        mSounds.push_back(sound);
    }
}

void StepTwoApp::mouseDown( MouseEvent event )
{
    int n;
	FMODErrorCheck( mSystem->getChannelsPlaying(&n) );
	if(n>=32) {
        console() << "Too many sounds playing!" << endl;
        return;
    }

    
    
    
    FMOD::Sound* sound = mSounds[Rand::randInt(mSounds.size())];
    
    // Channels inherit the modes of sounds.
    FMOD::Channel* channel;
    mSystem->playSound( FMOD_CHANNEL_FREE, sound, false, &channel );
}

void StepTwoApp::update()
{
}

void StepTwoApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

void StepTwoApp::shutdown()
{
    // 4. Release resources!
    for(auto& sound : mSounds) {
		FMODErrorCheck(sound->release());
	}
	FMODErrorCheck( mSystem->close() );
	FMODErrorCheck( mSystem->release() );
}

CINDER_APP_NATIVE( StepTwoApp, RendererGl )
