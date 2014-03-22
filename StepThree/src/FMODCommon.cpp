//
//  FMODCommon.cpp
//  StepTwo
//
//  Created by Jeffrey Crouse on 3/22/14.
//
//

#include "FMODCommon.h"


void FMODErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK) {
		cerr << "[FMOD] " << FMOD_ErrorString(result) << endl;
		exit(-1);
	}
}

void FMODListDevices()
{
	FMOD_RESULT result;
	FMOD::System* sys;
	result = FMOD::System_Create( &sys );
	FMODErrorCheck(result);
	
	int numDrivers;
	result = sys->getNumDrivers(&numDrivers);
	FMODErrorCheck(result);
	
	cout << "===========================" << endl;
	cout << "Listing audio devices:" << endl;
	for(int i=0; i<numDrivers; i++)
	{
		FMOD_GUID guid;
		char deviceName[256];
		sys->getDriverInfo(i, deviceName, 256, &guid);
		cout << "(" << i << ") " << deviceName << endl;
	}
	cout << "===========================" << endl;
	FMODErrorCheck( sys->close() );
	FMODErrorCheck( sys->release() );
}
