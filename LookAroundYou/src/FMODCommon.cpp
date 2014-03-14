//
//  FMODCommon.cpp
//  LookAroundYou
//
//  Created by Jeffrey Crouse on 3/14/14.
//
//

#include "FMODCommon.h"

// TO DO: change for production code!!!  DO NO EXIT!
void FMODErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK) {
		cerr << "[FMOD] " << FMOD_ErrorString(result) << endl;
		exit(-1);
	}
}

FMOD_VECTOR toFMOD(const cinder::Vec3f& cVec)
{
	FMOD_VECTOR vec;
	vec.x = cVec.x;
	vec.y = cVec.y;
	vec.z = cVec.z;
	return vec;
}