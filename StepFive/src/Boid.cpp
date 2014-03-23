//
//  Boid.cpp
//  StepFour
//
//  Created by Jeffrey Crouse on 3/22/14.
//
//

#include "Boid.h"

float Boid::range = 30;

Boid::Boid(Perlin* perlin, FMOD::Channel* channel)
{
    mColor.set(ColorModel::CM_HSV, Vec3f(Rand::randFloat(), 1.0, 1.0));
    mRadius = Rand::randFloat(5, 10);
    mPerlin = perlin;
    mPerlinIdx.set(0, Rand::randFloat()*10, Rand::randFloat()*10);
    
    mChannel = channel;
    mChannel->setPaused(false);
    
    FMOD_MODE mode;
    mChannel->getMode(&mode);
    if((mode & FMOD_3D) != FMOD_3D) {
        app::console() << "SOUND IS NOT 3D!" << endl;
    }
    
    FMODErrorCheck(mChannel->setDelay(FMOD_DELAYTYPE_END_MS, 1000, 1000));
    
    FMOD::System* system;
    mChannel->getSystemObject(&system);
    FMODErrorCheck(system->createDSPByType(FMOD_DSP_TYPE_ECHO, &dspecho));
    
    FMODErrorCheck(mChannel->addDSP(dspecho, 0));
    FMODErrorCheck(dspecho->setParameter(FMOD_DSP_ECHO_WETMIX, 1.0));
    FMODErrorCheck(dspecho->setParameter(FMOD_DSP_ECHO_DELAY, 2000));
}

Boid::~Boid()
{
    if(dspecho) dspecho->release();
}

void Boid::update(float deltaTime)
{
    mPerlinIdx.x += deltaTime * 0.2;
    mPos = mPerlin->dfBm(mPerlinIdx) * Boid::range;
    
    Vec3f velocity = (mPos - mPosLast)/deltaTime;
    
    FMOD_VECTOR _pos = toFMOD(mPos);
    FMOD_VECTOR _vel = toFMOD(velocity);
    mChannel->set3DAttributes(&_pos, &_vel);
    mChannel->set3DMinMaxDistance(0, mRadius);
    mChannel->getWaveData(waveData, 512, 0);
    mPosLast = mPos;
}

void Boid::draw()
{
    gl::pushMatrices();
    gl::translate(mPos);
    gl::color(mColor);
    drawAudibility();
    gl::color(mColor.r, mColor.g, mColor.b, 1.0);
    drawWaveWidget();
    gl::popMatrices();
}


void Boid::drawAudibility()
{
    float a=0;
	mChannel->getAudibility(&a);
    gl::drawStrokedCircle(Vec2f(0, 0), a, 30);
    gl::rotate(Vec3f(0, 90, 0));
    gl::drawStrokedCircle(Vec2f(0, 0), a, 30);
    gl::rotate(Vec3f(90, 0, 0));
    gl::drawStrokedCircle(Vec2f(0, 0), a, 30);
}

void Boid::drawWaveWidget()
{
    drawWaveCircle();
    gl::rotate(Vec3f(0, 90, 0));
    drawWaveCircle();
    gl::rotate(Vec3f(90, 0, 0));
    drawWaveCircle();
}

void Boid::drawWaveCircle()
{
    vector<Vec3f> vertices;
    float inc = M_PI/90.0;
    for(float i=0; i < M_PI*2.0; i+=inc) {
        int w = lmap<float>(i, 0, M_PI*2, 0, 512);
        float wave =waveData[w]*2.0;
        float x = wave + cos(i) * mRadius;
        float y = wave + sin(i) * mRadius;
        vertices.push_back( Vec3f(x, y, 0) );
    }
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, &vertices[0] );
    glDrawArrays( GL_LINE_STRIP, 0, vertices.size() );
}

