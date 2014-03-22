#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"
#include "Resources.h"
#include "Boid.h"
#include "FMODCommon.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// In this example
// 1. set3DListenerAttributes
// 2. Using bundle resources (http://libcinder.org/docs/v0.8.4/_cinder_resources.html)
// 3. addDSP
// 4. set3DAttributes and set3DMinMaxDistance
// 5. getAudibility


class StepFiveApp : public AppNative {
  public:
    void prepareSettings( AppBasic::Settings *settings );
	void setup();
	void mouseDown( MouseEvent event );
    void mouseDrag( MouseEvent event );
	void update();
	void draw();
    void resize();
    void shutdown();
    
    Timer mTimer;
    MayaCamUI mMayaCam;
    Perlin* mPerlin;
    vector<BoidRef> mBoids;
    
    FMOD::System* mSystem;
    vector<FMOD::Sound*> mSounds;
};

void StepFiveApp::prepareSettings( AppBasic::Settings *settings )
{
	settings->setFrameRate(60);
	//settings->setWindowSize(1024, 768);
	//	settings->enableHighDensityDisplay();
	//	settings->setAlwaysOnTop();
	//	settings->setBorderless();
	//	settings->setWindowPos( 0, 0 );
	settings->setFullScreen();
}

void StepFiveApp::setup()
{
    Rand::randomize();
    // Setup noise generator to drive Boid motion
    mPerlin = new Perlin();
    mPerlin->setSeed(clock());
	mPerlin->setOctaves(1);
    
    
    FMODListDevices();
    FMODErrorCheck(FMOD::System_Create( &mSystem ));
    FMODErrorCheck( mSystem->setDriver(0) );
	FMODErrorCheck( mSystem->init( 32, FMOD_INIT_NORMAL, NULL ) );
    
    // You can change these at runtime...
    FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };
    FMOD_VECTOR forward = { 0.0f, 0.0f, 1.0f };
    FMOD_VECTOR listenerpos = { 0.0f, 0.0f, 0.0f };
    FMOD_VECTOR vel = {0, 0, 0};
    FMODErrorCheck(mSystem->set3DListenerAttributes(0, &listenerpos, &vel, &forward, &up));
    
    vector<DataSourceRef> sources;
    sources.push_back(loadResource(RES_SOUND_01));
    sources.push_back(loadResource(RES_SOUND_02));
    sources.push_back(loadResource(RES_SOUND_03));
    sources.push_back(loadResource(RES_SOUND_04));
    sources.push_back(loadResource(RES_SOUND_05));
    sources.push_back(loadResource(RES_SOUND_06));
    sources.push_back(loadResource(RES_SOUND_07));
    sources.push_back(loadResource(RES_SOUND_08));
    
    for(auto& source : sources)
    {
        ci::fs::path relativePath = source->getFilePath();
        if( relativePath.empty() )
            relativePath = ci::fs::path( source->getFilePathHint() );
        
        FMOD::Sound* sound;
        FMOD_MODE mode = FMOD_3D | FMOD_3D_LINEARSQUAREROLLOFF | FMOD_LOOP_NORMAL;
        mSystem->createSound( relativePath.string().c_str(), mode, NULL, &sound );
        mSounds.push_back(sound);
        
        FMOD::Channel* channel;
        mSystem->playSound( FMOD_CHANNEL_FREE, sound, true, &channel );
        
        BoidRef boid = make_shared<Boid>(mPerlin, channel);
        mBoids.push_back(boid);
    }

    
    gl::enableDepthRead();
	gl::enableDepthWrite();
    gl::enableAlphaBlending();
}

void StepFiveApp::mouseDown( MouseEvent event )
{
	mMayaCam.mouseDown( event.getPos() );
}

void StepFiveApp::mouseDrag( MouseEvent event )
{
	// Added/hacked support for international mac laptop keyboards.
	bool middle = event.isMiddleDown() || ( event.isMetaDown() && event.isLeftDown() );
	bool right = event.isRightDown() || ( event.isControlDown() && event.isLeftDown() );
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown() && !middle && !right, middle, right );
}

void StepFiveApp::resize()
{
	console() << getWindowBounds();
	CameraPersp cam = mMayaCam.getCamera();
	cam.setAspectRatio( getWindowAspectRatio() );
	mMayaCam.setCurrentCam( cam );
	gl::enableDepthRead();
}


void StepFiveApp::update()
{
    float deltaTime = mTimer.getSeconds();
	mTimer.start();
    
    for(int i=0; i<mBoids.size(); i++) {
        mBoids[i]->update(deltaTime);
    }
    
    mSystem->update();
}

void StepFiveApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	
	// set up the camera
	gl::pushMatrices();
	gl::setMatrices( mMayaCam.getCamera() );
	
    gl::drawCoordinateFrame( 6.0f );
    
    for(int i=0; i<mBoids.size(); i++) {
        mBoids[i]->draw();
    }
    
	gl::popMatrices();
}

void StepFiveApp::shutdown()
{
    for(auto& sound : mSounds) {
		FMODErrorCheck(sound->release());
	}
    FMODErrorCheck( mSystem->close() );
	FMODErrorCheck( mSystem->release() );
}


CINDER_APP_NATIVE( StepFiveApp, RendererGl )
