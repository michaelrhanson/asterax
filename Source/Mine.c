#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"
#include <math.h>

#define nMineFaces	10
FacePtr	mineFaces[nMineFaces];
Handle mineCreateSound;
extern Boolean levelOver;

#define kMineSearchTimeConst 240

typedef struct {
	Boolean up;
	int searchTime;
	int timeSinceLastSound;
	PlayerShipData *firer;
} MineData;

#define rMinePingSound 226
Handle minePingSound;

void InitMine()
{
	int i;
	for(i=0;i<nMineFaces;i++) mineFaces[i] = GetFace(rBaseMineIcon +i);
	minePingSound = SATGetSound(rMinePingSound);
}

void SetMineFirer(SpritePtr me, PlayerShipData *data) {
	MineData *md;

	md = (MineData *)me->appPtr;
	md->firer = data;
}

pascal void SetupMine (SpritePtr me) {
	me->task = HandleMine;
	me->hitTask = HitMine;
	me->mode = 0;
	me->speed.h = 0;
	me->kind = 1; /*Friend kind*/
	SetRect(&me->hotRect, 0, 0, 16, 16);
	me->face = mineFaces[0];
	me->mode = 0;
	me->appLong = 0;
	me->appPtr = NewPtr(sizeof(MineData));
	((MineData *)me->appPtr)->searchTime = kMineSearchTimeConst+15;
	((MineData *)me->appPtr)->up = TRUE;
	((MineData *)me->appPtr)->timeSinceLastSound = 0;
	((MineData *)me->appPtr)->firer = NULL;
	PlaySound(kYummyChannel,minePingSound);			
}

pascal void HandleMine (SpritePtr me)
{	
	SpritePtr fore,back,target=NULL,launcher=NULL;
	int depth=0,dh,dv;
	MineData *md;
	double angle;
	int theAngle;

	MoveSprite(me,16);	
	md = (MineData *)me->appPtr;
	md->searchTime --;
	
	if (md->searchTime > kMineSearchTimeConst) {	
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

			if (dh>0) me->speed.h += +moveGranularity;
			else if (dh<-0) me->speed.h -= moveGranularity;
			
			if (me->speed.h > moveGranularity * 4) me->speed.h = moveGranularity*4;
			else if (me->speed.h < -moveGranularity * 4) me->speed.h = -moveGranularity*4;
			
			if (dv>0) me->speed.v += moveGranularity;
			else if (dv<0) me->speed.v -= moveGranularity;

			if (me->speed.v > moveGranularity * 4) me->speed.v = moveGranularity*4;
			else if (me->speed.v < -moveGranularity * 4) me->speed.v = -moveGranularity*4;
			
			md->timeSinceLastSound++;
			if (md->timeSinceLastSound > 12 && md->timeSinceLastSound > dh/2 ) {
				PlaySound(kYummyChannel,minePingSound);			
				md->timeSinceLastSound = 0;
			}			
		}	
	} else {
		me->task = NULL;
	 	NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupExplosion);	
	}
	if (md->up) {
		me->mode++;
		if (me->mode==2*(nMineFaces-1)) md->up=FALSE;
	} else {
		me->mode--;
		if (me->mode==0) md->up=TRUE;
	}
	me->face = mineFaces[me->mode/2];

	if (levelOver) {
		me->task = NULL;
	}
}

pascal void HitMine (SpritePtr me, SpritePtr him)
{

}

PlayerShipData *GetMineFirer(SpritePtr me) {
	return ((MineData *)(me->appPtr))->firer;
}

void PlayMinePing() {
	PlaySound(kYummyChannel,minePingSound);			
}