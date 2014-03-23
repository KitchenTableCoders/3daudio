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


int FMODGetNumSpeakers(FMOD_SPEAKERMODE mode)
{
	switch(mode) {
		case FMOD_SPEAKERMODE_MONO: return 1;
		case FMOD_SPEAKERMODE_STEREO: return 2;
		case FMOD_SPEAKERMODE_QUAD: return 4;
		case FMOD_SPEAKERMODE_SURROUND: return 5;
		case FMOD_SPEAKERMODE_5POINT1: return 6;
		case FMOD_SPEAKERMODE_7POINT1: return 8;
		case FMOD_SPEAKERMODE_SRS5_1_MATRIX: return 6;
		case FMOD_SPEAKERMODE_MYEARS: return 2;
		default: return 2;
	}
}

string FMODSpeakerModeDescription(FMOD_SPEAKERMODE mode) {
	switch(mode) {
		case FMOD_SPEAKERMODE_RAW:
			return "There is no specific speakermode.  Sound channels are mapped in order of input to output.  Use System::setSoftwareFormat to specify speaker count. See remarks for more information.";
		case FMOD_SPEAKERMODE_MONO:
			return "The speakers are monaural.";
		case FMOD_SPEAKERMODE_STEREO:
			return "The speakers are stereo (DEFAULT).";
		case FMOD_SPEAKERMODE_QUAD:
			return "4 speaker setup.  This includes front left, front right, rear left, rear right.";
		case FMOD_SPEAKERMODE_SURROUND:
			return "5 speaker setup.  This includes front left, front right, center, rear left, rear right.";
		case FMOD_SPEAKERMODE_5POINT1:
			return "5.1 speaker setup.  This includes front left, front right, center, rear left, rear right and a subwoofer.";
		case FMOD_SPEAKERMODE_7POINT1:
			return "7.1 speaker setup.  This includes front left, front right, center, rear left, rear right, side left, side right and a subwoofer.";
		case FMOD_SPEAKERMODE_SRS5_1_MATRIX:
			return "Stereo compatible output, embedded with surround information. SRS 5.1/Prologic/Prologic2 decoders will split the signal into a 5.1 speaker set-up or SRS virtual surround will decode into a 2-speaker/headphone setup.  See remarks about limitations.";
		case FMOD_SPEAKERMODE_MYEARS:
			return "Stereo output, but data is encoded using personalized HRTF algorithms.  See myears.net.au";
		default:
			return "Unknown SPEAKERMODE";
	}
}