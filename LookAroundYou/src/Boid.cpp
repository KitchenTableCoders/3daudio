//
//  Boid.cpp
//  LookAroundYou
//
//  Created by Jeffrey Crouse on 3/14/14.
//
//

#include "Boid.h"

float Boid::range = 30;
float Boid::radiusMultiplier = 1.0;

Boid::Boid(Perlin* perlin, SoundInstanceRef sound)
{
    mBorn = app::getElapsedSeconds ();
    mColor.set(ColorModel::CM_HSV, Vec3f(Rand::randFloat(), 1.0, 1.0));

    mPerlinIdx.set(0, Rand::randFloat()*10, Rand::randFloat()*10);
    
    mRadius = Rand::randFloat(5, 10);
    mSpeed = Rand::randFloat(0.05, 0.1);
    mPerlin = perlin;
    
    mSound = sound;
    mSound->play();
}

void Boid::update(float deltaTime)
{
    mPerlinIdx.x += (mSpeed / range);
    mPos = mPerlin->dfBm(mPerlinIdx) * Boid::range;
}

void Boid::drawWidget(float r)
{
    gl::drawStrokedCircle(Vec2f(0, 0), r, 30);
    gl::rotate(Vec3f(0, 90, 0));
    gl::drawStrokedCircle(Vec2f(0, 0), r, 30);
    gl::rotate(Vec3f(90, 0, 0));
    gl::drawStrokedCircle(Vec2f(0, 0), r, 30);
}

void Boid::draw()
{
    gl::pushMatrices();
    gl::translate(mPos);
    gl::color(mColor);
    drawWidget(1);
    gl::color(mColor.r, mColor.g, mColor.b, 0.3);
    drawWidget(mRadius*Boid::radiusMultiplier);
    gl::popMatrices();
}