/* Shield hit sprite for Asterax */

#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"

#define nShieldHitFaces	5
#define nSuperShieldFaces 1

FacePtr	shieldHitFaces[nShieldHitFaces];
FacePtr	superShieldFaces[nSuperShieldFaces];

void InitShieldHit()
{
	int i;
	for(i=0;i<nShieldHitFaces;i++) shieldHitFaces[i] = GetFace(rBaseShieldHitIcon + i);
	InitSuperShield();
}

pascal void SetupShieldHit (SpritePtr me)
{
	me->task = HandleShieldHit;
	me->hitTask = NULL;
	
	me->mode = 0;
	me->kind = 0; /*Neutral*/
	SetRect(&me->hotRect, 0,0,0,0);
	me->appLong = 0;
}

pascal void HandleShieldHit (SpritePtr me)
{
	me->face = shieldHitFaces[me->mode/3];
	me->mode = me->mode + 1;
	
	me->position.h += me->speed.h/moveGranularity;
	me->position.v += me->speed.v/moveGranularity;
	
	if (me->mode >= nShieldHitFaces*3) me->task = NULL;
}

void InitSuperShield()
{
	int i;
	for(i=0;i<nSuperShieldFaces;i++) superShieldFaces[i] = GetFace(rBaseSuperShieldIcon + i);
}

pascal void SetupSuperShield (SpritePtr me)
{
	me->task = HandleSuperShield;
	me->hitTask = NULL;
	
	me->mode = 0;
	me->kind = 0; /*Neutral*/
	SetRect(&me->hotRect, 0,0,0,0);
	me->appLong = 0;
	me->speed.h = me->speed.v = 0;
}

pascal void HandleSuperShield (SpritePtr me)
{
	me->face = superShieldFaces[0];
}
