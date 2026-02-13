/* PlayerShot sprite for Asterax */

#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"

#define nShotFaces	1
FacePtr	shotFaces[nShotFaces];
Handle shotCreateSound;
extern Boolean levelOver;

void CreateScatterShot(SpritePtr me);

void InitPlayerShot()
{
	int i;
	for(i=0;i<nShotFaces;i++) shotFaces[i] = GetFace(rBaseShotIcon + 2+i);
    shotCreateSound = SATGetSound(rCreateShotSnd);
}

pascal void SetupPlayerShot (SpritePtr me)
{
	me->task = HandlePlayerShot;
	me->hitTask = HitPlayerShot;
	me->mode = 0;
	me->speed.h = 0;
	me->kind = 1; /*Friend kind*/
	SetRect(&me->hotRect, 0, 0, 4, 4);
	me->face = shotFaces[0];
	me->mode = 40;
	me->appLong = 0;
	PlaySound(kShotChannel,shotCreateSound);
}

pascal void HandlePlayerShot (SpritePtr me)
{	
	SpritePtr newShot;
	MoveSprite(me,8);	
	if (!(me->mode&0xff)) {

		if (me->mode & 0x100) {
			// scatter shot
			CreateScatterShot(me);
		}
		me->task = NULL;
		if (!(me->mode & 0x200)) ((PlayerShipData *)me->appPtr)->shotsFired--;
	} else {
		me->mode --;
	}
	if (levelOver) me->task = NULL;
}

pascal void HitPlayerShot (SpritePtr me, SpritePtr him)
{
	if (me->mode & 0x100) {
		if (him->task ==  HandleSmallAsteroid ||
			him->task ==  HandleMidAsteroid ||
			him->task == HandleBigAsteroid) {
			
			CreateScatterShot(me);	
		}
	}
/* Hit something! We can take whatever action we need here, but in this case,*/
/* we let sApple decide. (We could have omitted this function altogether */
/* and passed nil as hittask to NewSprite.)*/
}


void CreateScatterShot(SpritePtr me) {
	SpritePtr newShot;
	
	if (RangedRdm(0,4)<3) {
		newShot = NewSpriteAfter(me, 1, me->position.h, me->position.v, SetupPlayerShot);
		newShot->speed.h = (me->speed.h + me->speed.v)/2 + RangedRdm(0,moveGranularity*2)-moveGranularity;
		newShot->speed.v = (me->speed.h + me->speed.v)/2 + RangedRdm(0,moveGranularity*2)-moveGranularity;
		newShot->appPtr = me->appPtr;
		newShot->mode += 0x200 - 20;
	}
	
	if (RangedRdm(0,4)<3) {
		newShot = NewSpriteAfter(me, 1, me->position.h, me->position.v, SetupPlayerShot);
		newShot->speed.h = (-me->speed.h - me->speed.v)/2 + RangedRdm(0,moveGranularity*2)-moveGranularity;
		newShot->speed.v = (me->speed.h + me->speed.v)/2 + RangedRdm(0,moveGranularity*2)-moveGranularity;
		newShot->appPtr = me->appPtr;
		newShot->mode += 0x200 - 20;
	}
		
	if (RangedRdm(0,4)<3) {
		newShot = NewSpriteAfter(me, 1, me->position.h, me->position.v, SetupPlayerShot);
		newShot->speed.h = (me->speed.h + me->speed.v)/2 + RangedRdm(0,moveGranularity*2)-moveGranularity;
		newShot->speed.v = (-me->speed.h - me->speed.v)/2 + RangedRdm(0,moveGranularity*2)-moveGranularity;
		newShot->appPtr = me->appPtr;
		newShot->mode += 0x200 - 20;
	}
		
	if (RangedRdm(0,4)<3) {
		newShot = NewSpriteAfter(me, 1, me->position.h, me->position.v, SetupPlayerShot);
		newShot->speed.h = (-me->speed.h - me->speed.v)/2 + RangedRdm(0,moveGranularity*2)-moveGranularity;
		newShot->speed.v = (-me->speed.h - me->speed.v)/2 + RangedRdm(0,moveGranularity*2)-moveGranularity;
		newShot->appPtr = me->appPtr;
		newShot->mode += 0x200 - 20;
	}
}
