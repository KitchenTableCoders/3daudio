//
//  Boid.h
//  StepFour
//
//  Created by Jeffrey Crouse on 3/22/14.
//
//

#pragma once
#include "cinder/Perlin.h"
#include "cinder/Rand.h"
#include "FMODCommon.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Boid {
public:
    static float range;
    
    Boid(Perlin* perlin, FMOD::Channel* channel);
    ~Boid();
    void update(float deltaTime);
    void draw();
    
protected:
    void drawAudibility();
    void drawWaveWidget();
    void drawWaveCircle();
    Color8u mColor;
    Vec3f mPos;
    Vec3f mPosLast;
    float mRadius;
    Perlin* mPerlin;
    Vec3f mPerlinIdx;
    FMOD::Channel* mChannel;
    float waveData[512];
    FMOD::DSP* dspecho;
};

typedef shared_ptr<class Boid> BoidRef;