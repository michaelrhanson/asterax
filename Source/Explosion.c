/* Big asteroid sprite for Asterax */

#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"

#define nExplosionFaces	10

FacePtr	explosionFaces[nExplosionFaces];

void InitExplosion()
{
	int i;
	for(i=0;i<nExplosionFaces;i++) explosionFaces[i] = GetFace(rBaseExplosionIcon + i);
}

pascal void SetupExplosion (SpritePtr me) {
	me->task = HandleExplosion;
	me->hitTask = NULL;
	me->mode = 0;
	me->kind = 0; /*Neutral*/
	me->appLong = 0;
	SetRect(&me->hotRect, 0,0,0,0);
}

pascal void HandleExplosion (SpritePtr me) {
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
	

	me->face = explosionFaces[me->mode/2];
	me->mode = me->mode + 1;
	
	if (me->mode >= nExplosionFaces*2) me->task = NULL;
}