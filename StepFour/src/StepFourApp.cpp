#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Perlin.h"
#include "Boid.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// In this example:
// 1. MayaCamUI
// 2. Boid class


class StepFourApp : public AppNative {
  public:
    void prepareSettings( AppBasic::Settings *settings ) override;
	void setup();
	void mouseDown( MouseEvent event );
    void mouseDrag( MouseEvent event );
	void update();
	void draw();
    void resize();
    void shutdown() override;
    
    Perlin* mPerlin;
    Boid* b;
    MayaCamUI mMayaCam;
    Timer mTimer;
};


void StepFourApp::prepareSettings( AppBasic::Settings *settings )
{
	settings->setFrameRate(60);
	settings->setWindowSize(1024, 768);
	//settings->enableHighDensityDisplay();
	//	settings->setAlwaysOnTop();
	//	settings->setBorderless();
	//	settings->setWindowPos( 0, 0 );
	//	settings->setFullScreen();
}

void StepFourApp::setup()
{
    Rand::randomize();
    
    // Setup noise generator to drive Boid motion
    mPerlin = new Perlin();
    mPerlin->setSeed(clock());
	mPerlin->setOctaves(1);
    
    
    b = new Boid(mPerlin);
}

void StepFourApp::resize()
{
	console() << getWindowBounds();
	CameraPersp cam = mMayaCam.getCamera();
	cam.setAspectRatio( getWindowAspectRatio() );
	mMayaCam.setCurrentCam( cam );
	gl::enableDepthRead();
}

void StepFourApp::mouseDown( MouseEvent event )
{
	mMayaCam.mouseDown( event.getPos() );
}

void StepFourApp::mouseDrag( MouseEvent event )
{
	// Added/hacked support for international mac laptop keyboards.
	bool middle = event.isMiddleDown() || ( event.isMetaDown() && event.isLeftDown() );
	bool right = event.isRightDown() || ( event.isControlDown() && event.isLeftDown() );
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown() && !middle && !right, middle, right );
}


void StepFourApp::update()
{
    float deltaTime = mTimer.getSeconds();
	mTimer.start();
    
    b->update(deltaTime);
}

void StepFourApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::pushMatrices();
	gl::setMatrices( mMayaCam.getCamera() );
    
    gl::drawCoordinateFrame( 10.0f );
    
    b->draw();
    
	gl::popMatrices();
    
}

void StepFourApp::shutdown()
{
    delete b;
}


CINDER_APP_NATIVE( StepFourApp, RendererGl )
