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
    
    system->createDSPByType(FMOD_DSP_TYPE_ECHO, &dspecho);
    FMODErrorCheck(mChannel->addDSP(dspecho, 0));
    FMODErrorCheck(dspecho->setParameter(FMOD_DSP_ECHO_WETMIX, 1.0));
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
    
    mPosLast = mPos;
}

void Boid::draw()
{
    float audibility=0;
	mChannel->getAudibility(&audibility);


    gl::pushMatrices();
    gl::translate(mPos);
    gl::color(mColor);
    drawWidget(audibility);
    gl::color(mColor.r, mColor.g, mColor.b, 0.3);
    drawWidget(mRadius);
    gl::popMatrices();
}

void Boid::drawWidget(float r)
{
    gl::drawStrokedCircle(Vec2f(0, 0), r, 30);
    gl::rotate(Vec3f(0, 90, 0));
    gl::drawStrokedCircle(Vec2f(0, 0), r, 30);
    gl::rotate(Vec3f(90, 0, 0));
    gl::drawStrokedCircle(Vec2f(0, 0), r, 30);
}

