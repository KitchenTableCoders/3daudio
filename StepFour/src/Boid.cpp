//
//  Boid.cpp
//  StepFour
//
//  Created by Jeffrey Crouse on 3/22/14.
//
//

#include "Boid.h"

float Boid::range = 30;

Boid::Boid(Perlin* perlin)
{
    mColor.set(ColorModel::CM_HSV, Vec3f(Rand::randFloat(), 1.0, 1.0));
    mRadius = Rand::randFloat(2, 5);
    mPerlin = perlin;
    mPerlinIdx.set(0, Rand::randFloat()*10, Rand::randFloat()*10);
}

void Boid::update(float deltaTime)
{
    mPerlinIdx.x += 0.01;
    mPos = mPerlin->dfBm(mPerlinIdx) * Boid::range;
}

void Boid::draw()
{
    gl::pushMatrices();
    gl::translate(mPos);
    gl::color(mColor);
    drawWidget(1);
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