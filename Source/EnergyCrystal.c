/* Energy sprite for Asterax */

#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"

#define nCrystalFaces	18
FacePtr	crystalFaces[nCrystalFaces];
extern Boolean levelOver;

Handle energyAppearsSound, shotEnergySound, gotEnergySound, energyGoesAwaySound;
// appLong is used to store fractional position: hiword has v, loword has h

void InitCrystal() {
	int i;
	for(i=0;i<nCrystalFaces;i++) crystalFaces[i] = GetFace(rBaseCrystalIcon + i);

	energyAppearsSound = SATGetSound(rEnergyAppearsSound);
	shotEnergySound = SATGetSound(rShotEnergySound);
	gotEnergySound = SATGetSound(rGotEnergySound);
	energyGoesAwaySound = SATGetSound(rEnergyGoesAwaySound);
}

pascal void SetupCrystal (SpritePtr me) {
	me->task = HandleCrystal;
	me->hitTask = HitCrystal;
	me->mode = 450;
	me->speed.h = RangedRdm(-20,20);
	me->speed.v = RangedRdm(-20,20);
	me->appLong = 0;
	
	me->kind = -1; /*Enemy kind -- can be destroyed by player shots*/
	SetRect(&me->hotRect, 4, 4, 12, 12);
	PlaySound (kYummyChannel,energyAppearsSound);

}

pascal void HandleCrystal (SpritePtr me) {
	Point *p;
	me->position.h += me->speed.h/moveGranularity;
	me->position.v += me->speed.v/moveGranularity;

    p = (Point *)(&me->appLong);
	p->h += me->speed.h%moveGranularity;
	p->v += me->speed.v%moveGranularity;
	if (p->h>moveGranularity) {
		me->position.h++;
		p->h-=moveGranularity;
	} else if (p->h<-moveGranularity) {
		me->position.h--;
		p->h+=moveGranularity;
	}

	if (p->v>moveGranularity) {
		me->position.v++;
		p->v-=moveGranularity;
	} else if (p->v<-moveGranularity) {
		me->position.v--;
		p->v+=moveGranularity;
	}
	
	if (me->position.v <= -32) me->position.v=gSAT.offSizeV+32+me->position.v;
	else if (me->position.v>gSAT.offSizeV) me->position.v -= gSAT.offSizeV;
	
	if (me->position.h <= -32) me->position.h = gSAT.offSizeH+32+me->position.h;
	else if (me->position.h>gSAT.offSizeH) me->position.h -= gSAT.offSizeH;

	me->face = crystalFaces[me->mode%nCrystalFaces];
	me->mode--;
	
	if (!me->mode) {
		me->task = NULL;
	 	NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupExplosion);
		PlaySound (kYummyChannel,energyGoesAwaySound);	
	}
	if (levelOver) me->task =NULL;
}

pascal void HitCrystal (SpritePtr me, SpritePtr him) {
	if (him->task == HandlePlayerShot) {
		if (!((PlayerShipData *)him->appPtr)->safeShots) {
			me->task = NULL;
		 	NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupExplosion);
			PlaySound (kYummyChannel,shotEnergySound);
			((PlayerShipData *)him->appPtr)->shotsFired--;		
			him->task = NULL;
		}
	} else if (him->task == HandlePlayerShip) {		
		if (!((PlayerShipData *)him->appPtr)->dead) {
			me->task = NULL;
			IncreaseShipShields((PlayerShipData *)him->appPtr,1000);
			PlaySound (kYummyChannel,gotEnergySound);
		}
	}
}

void PlotOneEnergy(Point *p) {
	SATPlotFaceToScreen(crystalFaces[5], *p, TRUE);
}