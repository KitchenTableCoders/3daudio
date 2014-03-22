//
//  FMODCommon.h
//  StepTwo
//
//  Created by Jeffrey Crouse on 3/22/14.
//
//

#pragma once
#include "fmod.hpp"
#include "fmod_errors.h"
#include "fmod_dsp.h"

using namespace std;

void FMODErrorCheck(FMOD_RESULT result);
void FMODListDevices();