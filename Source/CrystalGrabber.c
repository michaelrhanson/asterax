#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"

#define nGrabberFaces	16
FacePtr	grabberFaces[nGrabberFaces];
Handle grabberCreateSound;
extern Boolean levelOver;

extern SpritePtr p1ship,p2ship;
extern int p1Lives, p2Lives;
extern Boolean playerOneDead, playerTwoDead;

#define searchTimeConst 240

typedef struct {
	int nCrystals;
	int nEnergies;
	int searchTime;
	int whichLaunched;
	Boolean *myFlag;
} GrabberData;

void InitCrystalGrabber()
{
	int i;
	for(i=0;i<nGrabberFaces;i++) grabberFaces[i] = GetFace(rBaseGrabberIcon +i);
//    grabberCreateSound = SATGetSound(rCreateGrabberSnd);
}

pascal void SetupCrystalGrabber (SpritePtr me) {
	me->task = HandleCrystalGrabber;
	me->hitTask = HitCrystalGrabber;

	me->mode = 0;
	me->speed.h = 0;
	me->kind = 1; /*Friend kind*/
	SetRect(&me->hotRect, 0, 0, 16, 16);
	me->face = grabberFaces[0];
	me->mode = 0;
	me->appLong = 0;
	me->appPtr = NewPtr(sizeof(GrabberData));
	((GrabberData *)me->appPtr)->nCrystals = 0;
	((GrabberData *)me->appPtr)->nEnergies = 0;
	((GrabberData *)me->appPtr)->searchTime = searchTimeConst + 30;
	
//	PlaySound(kShotChannel,grabberCreateSound);
}

void SetGrabberFlag(SpritePtr me,Boolean *myFlag) {
	((GrabberData *)me->appPtr)->myFlag = myFlag;
}

void SetGrabberWhoLaunched(SpritePtr me,int who) {
	((GrabberData *)me->appPtr)->whichLaunched = who;
}

pascal void HandleCrystalGrabber (SpritePtr me) {	
	SpritePtr fore,back,target=NULL,launcher=NULL;
	int depth=0,dh,dv;
	GrabberData *gd;

	MoveSprite(me,16);	
	gd = (GrabberData *)me->appPtr;
	
	gd->searchTime --;
	if (gd->whichLaunched == 1) {
		if (p1Lives && !playerOneDead) launcher = p1ship;
		else {
		 	NewSpriteAfter(me, 0, me->position.h-4, me->position.v-4, SetupExplosion);
			me->task = NULL;
			return;
		}
	} else {
		if (p2Lives && !playerTwoDead) launcher = p2ship;
		else {
		 	NewSpriteAfter(me, 0, me->position.h-4, me->position.v-4, SetupExplosion);
			me->task = NULL;			
			return;
		}
	}
	
	if (gd->searchTime < searchTimeConst ) {
		if (gd->searchTime >= 0) {
			// scan local area for crystals and energy stuff
			fore = me->next;
			back = me->prev;
	
			while (TRUE) {
				if (fore && fore->task == HandleMoney || fore->task == HandleCrystal) {
					target = fore;
					break;
				} else if (back && back->task == HandleMoney || back->task == HandleCrystal) {
					target = back;
					break;
				}
				if (fore) fore = fore->next;
				if (back) back = back->prev;
				if (depth++ == 32) break;
			}
		} else {
			target = launcher;
		}
	
		if (target) {
			dh = (target->position.h) - me->position.h;
			dv = (target->position.v) - me->position.v;
		
			if (gd->searchTime < 0 && ABS(dh)<16 && ABS(dv)<16) {
				IncreaseShipMoney((PlayerShipData *)target->appPtr,
							gd->nCrystals);
				IncreaseShipShields((PlayerShipData *)target->appPtr,
							gd->nEnergies * 1000);	
				*(gd->myFlag) = FALSE;
				return;
			}
		
			if (dh>30) me->speed.h += moveGranularity/2;
			else if (dh>15) me->speed.h += moveGranularity/4;
			else if (dh>0) me->speed.h += moveGranularity/8;
			else if (dh<-30) me->speed.h -= moveGranularity/2;
			else if (dh<-15) me->speed.h -= moveGranularity/4;
			else if (dh<0) me->speed.h -= moveGranularity/8;
			
			if (dh < 30) {
				if (me->speed.h > moveGranularity * 4) me->speed.h -= moveGranularity;
				if (me->speed.h < -moveGranularity * 4) me->speed.h += moveGranularity;
			}
			if (me->speed.h > moveGranularity * 8) me->speed.h = moveGranularity * 8;
			else if (me->speed.h < -moveGranularity *8) me->speed.h = -moveGranularity * 8;

			if (dv>30) me->speed.v += moveGranularity/2;
			else if (dv>15) me->speed.v += moveGranularity/4;
			else if (dv>0) me->speed.v += moveGranularity/8;
			else if (dv<-30) me->speed.v -= moveGranularity/2;
			else if (dv<-15) me->speed.v -= moveGranularity/4;
			else if (dv<0) me->speed.v -= moveGranularity/8;
			
			if (dv < 30) {
				if (me->speed.v > moveGranularity * 4) me->speed.v -= moveGranularity;
				if (me->speed.v < -moveGranularity * 4) me->speed.v += moveGranularity;
			}
			if (me->speed.v > moveGranularity * 8) me->speed.v = moveGranularity * 8;
			else if (me->speed.v < -moveGranularity *8) me->speed.v = -moveGranularity * 8;
		}	
	}
	me->face = grabberFaces[me->mode/2];
	me->mode = me->mode + 1;
	if (me->mode >= nGrabberFaces*2) me->mode = 0;
	if (levelOver) {
		*(gd->myFlag) = FALSE;
	}
}

pascal void HitCrystalGrabber (SpritePtr me, SpritePtr him) {
	GrabberData *gd = (GrabberData *)me->appPtr;
	if (him->task == HandleMoney) {
		gd->nCrystals ++;
		him->task = NULL;
	} else if (him->task == HandleCrystal) {
		gd->nEnergies ++;
		him->task = NULL;	
	}
}