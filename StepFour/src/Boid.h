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

using namespace ci;
using namespace ci::app;
using namespace std;

class Boid {
public:
    static float range;
    
    Boid(Perlin* perlin);
    void update(float deltaTime);
    void draw();
    
protected:
    void drawWidget(float r);
    Color8u mColor;
    Vec3f mPos;
    float mRadius;
    Perlin* mPerlin;
    Vec3f mPerlinIdx;
};
