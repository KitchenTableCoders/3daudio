#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#include "fmod_dsp.h"


using namespace ci;
using namespace ci::app;
using namespace std;


void FMODErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK) {
		cerr << "[FMOD] " << FMOD_ErrorString(result) << endl;
		exit(-1);
	}
}

void FMODListDevices()
{
	FMOD_RESULT result;
	FMOD::System* sys;
	result = FMOD::System_Create( &sys );
	FMODErrorCheck(result);
	
	int numDrivers;
	result = sys->getNumDrivers(&numDrivers);
	FMODErrorCheck(result);
	
	app::console() << "===========================" << endl;
	app::console() << "Listing audio devices:" << endl;
	for(int i=0; i<numDrivers; i++)
	{
		FMOD_GUID guid;
		char deviceName[256];
		sys->getDriverInfo(i, deviceName, 256, &guid);
		app::console() << "(" << i << ") " << deviceName << endl;
	}
	app::console() << "===========================" << endl;
	FMODErrorCheck( sys->close() );
	FMODErrorCheck( sys->release() );
}


class StepOneApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    void shutdown() override;
    
    FMOD::System* mSystem;
    FMOD::Sound* mSound;
    FMOD::Channel* mChannel;
};

void StepOneApp::setup()
{
    FMODListDevices();
    
    // 1. Setup
    // Create an FMOD::System -- this is totally abstract at this point. Just an empty container.
    FMODErrorCheck(FMOD::System_Create( &mSystem ));
    
    // Now we tell FMOD which driver the system should use.
    FMODErrorCheck(mSystem->setDriver(0));
    
    // Final setup step: we set some properties of the system.
	FMOD_INITFLAGS flags = FMOD_INIT_NORMAL; // right-click, Go To Definition
	FMODErrorCheck(mSystem->init( 32, flags, NULL ));
    
    // 2. Load a sound
    string path = getAssetPath("orch-hit.wav").string();
    FMODErrorCheck( mSystem->createSound( path.c_str(), FMOD_SOFTWARE, NULL, &mSound) );
    
    // 3. Make an "instance" of the sound known as a "FMOD::Channel"
    // Hint: Try moving this into mouseDown
    // Question: What happens to mChannel?
    FMODErrorCheck( mSystem->playSound( FMOD_CHANNEL_FREE, mSound, true, &mChannel ) );

}

void StepOneApp::mouseDown( MouseEvent event )
{
    if(mChannel->setPaused(false)!=FMOD_OK) {
        console() << "Playing" << endl;
    }
}

void StepOneApp::update()
{
}

void StepOneApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

void StepOneApp::shutdown()
{
    console() << "Exiting!" << endl;
    // 4. Release resources!
    FMODErrorCheck( mSound->release());
	FMODErrorCheck( mSystem->close() );
	FMODErrorCheck( mSystem->release() );
}

CINDER_APP_NATIVE( StepOneApp, RendererGl )
