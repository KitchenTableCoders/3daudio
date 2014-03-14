//
//  FMODCommon.h
//  LookAroundYou
//
//  Created by Jeffrey Crouse on 3/14/14.
//
//

#pragma once
#include "cinder/Vector.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#include "fmod_dsp.h"

using namespace std;

void FMODErrorCheck(FMOD_RESULT result);
FMOD_VECTOR toFMOD(const cinder::Vec3f& vec);