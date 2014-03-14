//
//  Boid.h
//  LookAroundYou
//
//  Created by Jeffrey Crouse on 3/14/14.
//
//

#pragma once
#include "cinder/Rand.h"
#include "cinder/Perlin.h"
#include "FMODCommon.h"

using namespace ci;

class Boid {
public:
    Boid(Perlin* perlin);
    void update(float deltaTime);
    void draw();
    void drawWidget(float radius);
    
    static float range;
    static float radiusMultiplier;
    
protected:
    Color8u mColor;
    double mBorn;
    Vec3f mPos;
    float mSpeed;
    float mRadius;
    Vec3f mPerlinIdx;
    
    Perlin* mPerlin;
};

typedef shared_ptr<class Boid> BoidRef;