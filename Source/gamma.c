// File "gamma.c" - Source for Altering the Gamma Tables of GDevices
//   Last updated 3/13/93, MJS

// * ****************************************************************************** *
//
//	This is the Source Code for the Gamma Utils Library file. Use this to build
//		new functionality into the library or make an A4-based library. 
//	See the header file "gamma.h" for much more information. -- MJS
//
// * ****************************************************************************** *

#include <GestaltEqu.h>
#include <Quickdraw.h>
#include <Video.h>
#include <Traps.h>
#include "gamma.h"

long			gammaUtilsInstalled;
globalGammasHdl	gammaTables;

// * ****************************************************************************** *
// * ****************************************************************************** *

Boolean IsGammaAvailable() {
	GDHandle theGDevice;

	if (NGetTrapAddress(kGetDeviceListTrapNum, ToolTrap) ==
			NGetTrapAddress(_Unimplemented, ToolTrap)) return(0);
	
	for(theGDevice = GetDeviceList(); theGDevice; theGDevice = GetNextDevice(theGDevice))
		if (TestDeviceAttribute(theGDevice, screenDevice) && 
				TestDeviceAttribute(theGDevice, noDriver)) return(0);

	return(-1);
	}

// * ****************************************************************************** *
// * ****************************************************************************** *

Boolean IsOneGammaAvailable(GDHandle theGDevice) {
	
	if (NGetTrapAddress(kGetDeviceListTrapNum, ToolTrap) ==
			NGetTrapAddress(_Unimplemented, ToolTrap)) return(0);
	
	if (TestDeviceAttribute(theGDevice, screenDevice) && 
			TestDeviceAttribute(theGDevice, noDriver)) return(0);
	
	return(-1);
	}

// * ****************************************************************************** *
// * ****************************************************************************** *

OSErr SetupGammaTools() {
	short errorCold=0;
	globalGammasHdl tempHdl;
	GammaTblPtr	masterGTable;
	GDHandle theGDevice;

	if (gammaUtilsInstalled == kGammaUtilsSig) return(-1);
	
	gammaTables = 0;
	gammaUtilsInstalled = kGammaUtilsSig;
	for(theGDevice = GetDeviceList(); theGDevice; theGDevice = GetNextDevice(theGDevice)) {
		if (errorCold = GetDevGammaTable(theGDevice, &masterGTable)) return(errorCold);
		
		tempHdl = (globalGammasHdl) NewHandle(sizeof(globalGammas));
		if (tempHdl == 0) return(errorCold = MemError());
		
		(*tempHdl)->size = sizeof(GammaTbl) + masterGTable->gFormulaSize +
				(masterGTable->gChanCnt * masterGTable->gDataCnt * masterGTable->gDataWidth / 8);
		(*tempHdl)->dataOffset = masterGTable->gFormulaSize;
		(*tempHdl)->theGDevice = theGDevice;
		
		(*tempHdl)->saved = (GammaTblHandle) NewHandle((*tempHdl)->size);
		if ((*tempHdl)->saved == 0) return(errorCold = MemError());
		(*tempHdl)->hacked = (GammaTblHandle) NewHandle((*tempHdl)->size);
		if ((*tempHdl)->hacked == 0) return(errorCold = MemError());
	
		BlockMove((Ptr) masterGTable, (Ptr) *(*tempHdl)->saved, (*tempHdl)->size);
		
		(*tempHdl)->next = gammaTables;
		gammaTables = tempHdl;
		}

	return(0);
	}

// * ****************************************************************************** *
// * ****************************************************************************** *

OSErr DoGammaFade(short percent) {
	short errorCold=0;
	register long size, i, theNum;
	globalGammasHdl tempHdl;
	unsigned char *dataPtr;

	if (gammaUtilsInstalled != kGammaUtilsSig) return(-1); 
	
	for(tempHdl = gammaTables; tempHdl; tempHdl = (*tempHdl)->next) {
	
		BlockMove((Ptr) *(*tempHdl)->saved, (Ptr) *(*tempHdl)->hacked, (*tempHdl)->size);
		dataPtr = (unsigned char *) (*(*tempHdl)->hacked)->gFormulaData + (*tempHdl)->dataOffset;
		size = (*(*tempHdl)->hacked)->gChanCnt * (*(*tempHdl)->hacked)->gDataCnt;
		
		for(i=0; i < size; i++) {
			theNum = dataPtr[i];
			theNum = (theNum * percent) / 100;
			dataPtr[i] = theNum;
			}
		
		if (errorCold = SetDevGammaTable((*tempHdl)->theGDevice, (*tempHdl)->hacked))
			return(errorCold);
		}
		
	return(0);
	}

// * ****************************************************************************** *
// * ****************************************************************************** *

OSErr DoOneGammaFade(GDHandle theGDevice, short percent) {
	short errorCold=0;
	register long size, i, theNum;
	globalGammasHdl tempHdl;
	unsigned char *dataPtr;

	if (gammaUtilsInstalled != kGammaUtilsSig) return(-1); 
	for(tempHdl = gammaTables; tempHdl && (theGDevice != (*tempHdl)->theGDevice);
			tempHdl = (*tempHdl)->next);
		
	BlockMove((Ptr) *(*tempHdl)->saved, (Ptr) *(*tempHdl)->hacked, (*tempHdl)->size);
	dataPtr = (unsigned char *) (*(*tempHdl)->hacked)->gFormulaData + (*tempHdl)->dataOffset;
	size = (*(*tempHdl)->hacked)->gChanCnt * (*(*tempHdl)->hacked)->gDataCnt;
	
	for(i=0; i < size; i++) {
		theNum = dataPtr[i];
		theNum = (theNum * percent) / 100;
		dataPtr[i] = theNum;
		}
	
	errorCold = SetDevGammaTable((*tempHdl)->theGDevice, (*tempHdl)->hacked);
	
	return(errorCold);
	}

// * ****************************************************************************** *
// * ****************************************************************************** *

OSErr DisposeGammaTools() {
	globalGammasHdl tempHdl, nextHdl;

	if (gammaUtilsInstalled != kGammaUtilsSig) return(-1); 
	for(tempHdl = gammaTables; tempHdl; tempHdl = nextHdl) {
		nextHdl = (*tempHdl)->next;
		DisposeHandle((Handle) (*tempHdl)->saved);
		DisposeHandle((Handle) (*tempHdl)->hacked);
		DisposeHandle((Handle) tempHdl);
		}
		
	gammaUtilsInstalled = 0;
	return(0);
	}

// * ****************************************************************************** *
// * ****************************************************************************** *

OSErr GetDevGammaTable(GDHandle theGDevice, GammaTblPtr *theTable) {
	short errorCold=0;
	CntrlParam  *myCPB;
 
	((long *) theTable)[0] = 0;
 
	if (IsOneGammaAvailable(theGDevice) == 0) return(-1);
			
	if ((myCPB = (CntrlParam *) NewPtrClear(sizeof(CntrlParam))) == 0) return(MemError());
	myCPB->csCode = cscGetGamma;
	myCPB->ioCRefNum = (*theGDevice)->gdRefNum;
	*(GammaTblPtr **) myCPB->csParam = theTable;
	errorCold = PBStatus((ParmBlkPtr) myCPB, 0);

	DisposePtr((Ptr) myCPB);
	return(errorCold);
	}

// * ****************************************************************************** *
// * ****************************************************************************** *

OSErr SetDevGammaTable(GDHandle theGDevice, GammaTblPtr *theTable) {
	CntrlParam *myCPB;
	short errorCold=0;
	CTabHandle cTab;
	GDHandle saveGDevice;
 
	if (IsOneGammaAvailable(theGDevice) == 0) return(-1);

	if ((myCPB = (CntrlParam *) NewPtrClear(sizeof(CntrlParam))) == 0) return(MemError());
	myCPB->csCode = cscSetGamma;
	myCPB->ioCRefNum = (*theGDevice)->gdRefNum;
	**(GammaTblPtr **) myCPB->csParam = *theTable;

	errorCold = PBControl((ParmBlkPtr) myCPB, 0);
 
	if (errorCold == 0) {
		saveGDevice = GetGDevice();
		SetGDevice(theGDevice);
 		cTab = (*(*theGDevice)->gdPMap)->pmTable;
		SetEntries (0, (*cTab)->ctSize, (*cTab)->ctTable);
		SetGDevice(saveGDevice);
		}

	DisposePtr((Ptr) myCPB);
	return (errorCold);
	}
