#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

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
    
    
}

void StepFiveApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	
	// set up the camera
	gl::pushMatrices();
	gl::setMatrices( mMayaCam.getCamera() );
	

    gl::drawCoordinateFrame( 6.0f );
    
	gl::popMatrices();
}

void StepFiveApp::shutdown()
{
}


CINDER_APP_NATIVE( StepFiveApp, RendererGl )
