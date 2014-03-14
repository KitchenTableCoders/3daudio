#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Timer.h"
#include "cinder/params/Params.h"
#include "cinder/Rand.h"
#include "cinder/MayaCamUI.h"
#include "Boid.h"
#include "AudioDevice.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class LookAroundYouApp : public AppNative {
public:
	void prepareSettings( AppBasic::Settings *settings ) override;
	void setup() override;
	void resize() override;
	void update() override;
	void draw() override;
	void mouseMove( MouseEvent event ) override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void mouseWheel( MouseEvent event ) override;
	void mouseUp( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;
	void keyUp( KeyEvent event ) override;
	void shutdown() override;
    
private:
	params::InterfaceGl mParams;
	Timer mTimer;
	float mFrameRate;
	MayaCamUI mMayaCam;
	Vec2f mMousePos;
    
    Perlin* mPerlin;
    vector<BoidRef> mBoids;
    AudioDeviceRef mAudioDevice;
    SoundInstanceRef music;
    vector<string> fxnames;
};


void LookAroundYouApp::prepareSettings( AppBasic::Settings *settings )
{
	settings->setFrameRate(60);
	settings->setWindowSize(1024, 768);
	//	settings->enableHighDensityDisplay();
	//	settings->setAlwaysOnTop();
	//	settings->setBorderless();
	//	settings->setWindowPos( 0, 0 );
	//	settings->setFullScreen();
}

void LookAroundYouApp::setup()
{
	Rand::randomize();
	
	getWindow()->setTitle("MPCDigital App");
	mParams = params::InterfaceGl( "Settings", Vec2i( 300, 200 ) );
	mParams.addParam( "Framerate", &mFrameRate, "", true );
	
    mParams.addParam("Radius", &Boid::radiusMultiplier, "min=0.5 max=3.0 step=0.1");
    mParams.addParam("Range", &Boid::range, "min=10.0 max=100 step=1.0");
    
    AudioDevice::printDeviceMap();
    mAudioDevice = make_shared<AudioDevice>();
    mAudioDevice->setup(0);
    console() << "Channels Available: " << mAudioDevice->getNumRemainingChannels() << endl;
    
    DataSourceRef data = loadAsset("music/Music For Airports - 1 1.mp3");
    string musicName = mAudioDevice->registerSound(data, true, false, true);
    music = mAudioDevice->getSoundInstance(musicName, 1.0);
    music->play();
    
    fxnames.push_back(mAudioDevice->registerSound(loadAsset("fx/blue.aif"), true, true));
    fxnames.push_back(mAudioDevice->registerSound(loadAsset("fx/fliup.aif"), true, true));
    fxnames.push_back(mAudioDevice->registerSound(loadAsset("fx/green.aif"), true, true));
    fxnames.push_back(mAudioDevice->registerSound(loadAsset("fx/howl.wav"), true, true));
    fxnames.push_back(mAudioDevice->registerSound(loadAsset("fx/orange.aif"), true, true));
    fxnames.push_back(mAudioDevice->registerSound(loadAsset("fx/red.aif"), true, true));
    fxnames.push_back(mAudioDevice->registerSound(loadAsset("fx/white.aif"), true, true));
    fxnames.push_back(mAudioDevice->registerSound(loadAsset("fx/yellow.aif"), true, true));
    
    
    mPerlin = new Perlin();
    mPerlin->setSeed(clock());
	mPerlin->setOctaves(1);
    for(int i=0; i<10; i++) {
        int n = Rand::randInt(0, fxnames.size());
        SoundInstanceRef fx = mAudioDevice->getSoundInstance(fxnames[n], 1.0);
        BoidRef boid = make_shared<Boid>(mPerlin, fx);
        mBoids.push_back(boid);
    }
    
	
	// enable the depth buffer (for 3D)
	gl::enableDepthRead();
	gl::enableDepthWrite();
    gl::enableAlphaBlending();
}

void LookAroundYouApp::resize()
{
	console() << getWindowBounds();
	CameraPersp cam = mMayaCam.getCamera();
	cam.setAspectRatio( getWindowAspectRatio() );
	mMayaCam.setCurrentCam( cam );
	gl::enableDepthRead();
}

void LookAroundYouApp::update()
{
	float deltaTime = mTimer.getSeconds();
	mTimer.start();
	
    gl::color(Color::white());
    for(int i=0; i<mBoids.size(); i++) {
        mBoids[i]->update(deltaTime);
    }
    
    mAudioDevice->update();
	mFrameRate = getAverageFps();
}

void LookAroundYouApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	
	// set up the camera
	gl::pushMatrices();
	gl::setMatrices( mMayaCam.getCamera() );
	
    
    for(int i=0; i<mBoids.size(); i++) {
        mBoids[i]->draw();
    }

    gl::drawCoordinateFrame( 6.0f );
    
	gl::popMatrices();
	
	// Draw the interface
	mParams.draw();
    
    float pos = music->getPlayheadNorm();
    Rectf bar(0, 0, app::getWindowWidth()*pos, 10);
    gl::drawSolidRect(bar);
}

void LookAroundYouApp::mouseDown( MouseEvent event )
{
	mMousePos = event.getPos();
	mMayaCam.mouseDown( mMousePos );
}

void LookAroundYouApp::mouseMove( MouseEvent event )
{
	mMousePos = event.getPos();
}

void LookAroundYouApp::mouseDrag( MouseEvent event )
{
	mMousePos = event.getPos();
	
	// Added/hacked support for international mac laptop keyboards.
	bool middle = event.isMiddleDown() || ( event.isMetaDown() && event.isLeftDown() );
	bool right = event.isRightDown() || ( event.isControlDown() && event.isLeftDown() );
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown() && !middle && !right, middle, right );}

void LookAroundYouApp::mouseUp( MouseEvent event )
{
}

void LookAroundYouApp::mouseWheel( MouseEvent event )
{
}

void LookAroundYouApp::keyDown( KeyEvent event )
{
}

void LookAroundYouApp::keyUp( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		app::setFullScreen( !app::isFullScreen() );
	} else if(event.getChar()==KeyEvent::KEY_SPACE) {
		mParams.maximize( !mParams.isMaximized() );
	}
}

void LookAroundYouApp::shutdown()
{
}

CINDER_APP_NATIVE( LookAroundYouApp, RendererGl )
