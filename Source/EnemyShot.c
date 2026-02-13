/* EnemyShot sprite for Asterax */

#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"

/************ Constants *************/
#define MIN(x,y)				( ((x)<(y))?(x):(y))
#define MAX(x,y)				( ((x)<(y))?(y):(x))
#define nEnemyShotFaces	2
#define rShotCollideSnd	216

/************ Globals ******************/
FacePtr	enemyShotFaces[nEnemyShotFaces];
Handle enemyShotCreateSound,playerHitSound,shotsCollideSound;
extern Boolean levelOver;
extern SpritePtr p1ship,p2ship;
extern int p1Lives, p2Lives;
extern Boolean playerOneDead, playerTwoDead;

/************** Code ***************/
void InitEnemyShot()
{
	int i;
	for(i=0;i<nEnemyShotFaces;i++) enemyShotFaces[i] = GetFace(rBaseEnemyShotIcon + i);
    enemyShotCreateSound = SATGetSound(rCreateEnemyShotSnd);
    playerHitSound = SATGetSound(rPlayerHitSnd);
    shotsCollideSound = SATGetSound(rShotCollideSnd);
}

pascal void SetupEnemyShot (SpritePtr me) {
	me->task = HandleEnemyShot;
	me->hitTask = HitEnemyShot;
	me->mode = 0;
	me->speed.h = 0;
	me->kind = -3; /*enemy kind*/
	SetRect(&me->hotRect, 0, 0, 4, 4);
	me->face = enemyShotFaces[0];
	me->mode = 50;
	me->appLong = 0;
	PlaySound (kShotChannel,enemyShotCreateSound);
}

pascal void HandleEnemyShot (SpritePtr me) {
	MoveSprite(me,8);
		
	if (!me->mode) {
		me->task = NULL;
	} else {
		me->mode --;
	}
	if (levelOver) me->task = NULL;
}

pascal void HitEnemyShot (SpritePtr me, SpritePtr him) {
	if (him->task == HandlePlayerShip) {
		DecreaseShipShields(((PlayerShipData *)him->appPtr),1000);
		me->task =NULL;
		PlaySound (kExplosionChannel,playerHitSound);
	} else if (him->task == HandlePlayerShot) {
		((PlayerShipData *)him->appPtr)->shotsFired--;
		him->task = NULL;
		me->task =NULL;
	 	NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupExplosion);	
		PlaySound (kExplosionChannel,shotsCollideSound);

	}
}

pascal void SetupEnemyShot_Seek (SpritePtr me) {
	me->task = HandleEnemyShot_Seek;
	me->hitTask = HitEnemyShot;
	me->destructTask = NULL;

	me->mode = 0;
	me->speed.h = 0;
	me->kind = -3; /*enemy kind*/
	SetRect(&me->hotRect, 0, 0, 4, 4);
	me->face = enemyShotFaces[1];
	me->mode = 50;
	me->appLong = 0;
	PlaySound (kShotChannel,enemyShotCreateSound);
}

pascal void HandleEnemyShot_Seek(SpritePtr me) {
	SpritePtr target;
	Boolean	goodTarget=TRUE;

	MoveSprite(me,8);

	if (p1Lives && p2Lives) {
		if (RangedRdm(0,400)>200) {
			target = p1ship;
			if (playerOneDead) goodTarget =FALSE;
		} else {
			target = p2ship;
			if (playerTwoDead) goodTarget =FALSE;
		}				
	} else if (p1Lives) {
		target = p1ship;
		if (playerOneDead) goodTarget =FALSE;
	} else if (p2Lives) {
		target = p2ship;
		if (playerTwoDead) goodTarget =FALSE;
	}

	if (goodTarget) {
		if (me->position.h > target->position.h) me->speed.h-=32;
		if (me->position.h < target->position.h) me->speed.h+=32;
		if (me->position.v > target->position.v) me->speed.v-=32;
		if (me->position.v < target->position.v) me->speed.v+=32;
		me->speed.h = MIN(me->speed.h, moveGranularity * 7);
		me->speed.h = MAX(me->speed.h, -moveGranularity * 7);
		me->speed.v = MIN(me->speed.v, moveGranularity * 7);
		me->speed.v = MAX(me->speed.v, -moveGranularity * 7);
	}
	
	if (!me->mode) {
		me->task = NULL;
	} else {
		me->mode --;
	}
	if (levelOver) me->task = NULL;
}