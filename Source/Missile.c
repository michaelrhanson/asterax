#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"
#include <math.h>

#define nMissileFaces	18
FacePtr	missileFaces[nMissileFaces];
Handle missileCreateSound;
extern Boolean levelOver;

#define kMissileSearchTimeConst 480

typedef struct {
	int searchTime;
} MissileData;

void InitMissile()
{
	int i;
	for(i=0;i<nMissileFaces;i++) missileFaces[i] = GetFace(rBaseMissileIcon +i);
//    missileCreateSound = SATGetSound(rCreateMissileSnd);
}

pascal void SetupMissile (SpritePtr me)
{
	me->mode = 0;
	me->speed.h = 0;
	me->kind = 1; /*Friend kind*/
	SetRect(&me->hotrect, 0, 0, 16, 16);
	me->face = missileFaces[0];
	me->mode = 0;
	me->appLong = 0;
	me->appPtr = NewPtr(sizeof(MissileData));
	((MissileData *)me->appPtr)->searchTime = kMissileSearchTimeConst+15;
//	PlaySound(kShotChannel,missileCreateSound);
}

pascal void HandleMissile (SpritePtr me)
{	
	SpritePtr fore,back,target=NULL,launcher=NULL;
	int depth=0,dh,dv;
	MissileData *md;
	double angle;
	int theAngle;

	MoveSprite(me,16);	
	md = (MissileData *)me->appPtr;
	md->searchTime --;
	
	if (md->searchTime > kMissileSearchTimeConst) {	
		if (!me->speed.h) angle = 90;
		else angle = (atan((double)me->speed.v/me->speed.h) * 180 / 3.14);
		if (me->speed.h<0) angle+=180;
		theAngle = (360-angle) / 20;
		me->mode = theAngle;
	} else if (md->searchTime >0 ) {
			fore = me->next;
			back = me->prev;
	
			while (TRUE) {
				if (fore && (fore->task == HandleSimpleSaucer || 
							fore->task == HandleLeechSaucer ||
							fore->task == HandleSpiderSaucer)) {
					target = fore;
					break;
				} else if (back && (back->task == HandleSimpleSaucer || 
							back->task == HandleLeechSaucer ||
							back->task == HandleSpiderSaucer)) {
					target = back;
					break;
				}
				if (fore) fore = fore->next;
				if (back) back = back->prev;
				if (depth++ == 32) break;
			}

		if (target) {
			dh = (target->position.h) - me->position.h;
			dv = (target->position.v) - me->position.v;

			if (!dh) angle = 3.14/2;
			else angle = (atan((double)dv/dh) * 180 / 3.14);
			theAngle = angle / 20;
			theAngle += 45;
			if (dh <0) theAngle+=9;
			theAngle %= 18;
	
			if (me->mode != theAngle) {
				if (ABS((me->mode-1-theAngle)) < ABS((me->mode+1-theAngle))) me->mode--;
				else me->mode++;
				me->mode = (me->mode + 18)%18;
			}

			if (dh>30) me->speed.h = moveGranularity*4;
			else if (dh>5) me->speed.h = moveGranularity*2;
			else if (dh>2) me->speed.h = moveGranularity;
			else if (dh<-30) me->speed.h = -moveGranularity*4;
			else if (dh<-5) me->speed.h = -moveGranularity*2;
			else if (dh<-2) me->speed.h = -moveGranularity;
			
			if (dv>30) me->speed.v = moveGranularity*4;
			else if (dv>5) me->speed.v = moveGranularity*2;
			else if (dv>2) me->speed.v = moveGranularity;
			else if (dv<-30) me->speed.v = -moveGranularity*4;
			else if (dv<-5) me->speed.v = -moveGranularity*2;
			else if (dv<-2) me->speed.v = -moveGranularity;
		}	
	} else {
		me->task = NULL;
	 	NewSpriteAfter(me, 0, me->position.h, me->position.v, HandleExplosion, SetupExplosion, HitExplosion);	
	}
	
	me->face = missileFaces[me->mode];

	if (levelOver) {
		me->task = NULL;
	}
}

pascal void HitMissile (SpritePtr me, SpritePtr him)
{

}