#include "SAT.h"
#include "Sprites.h"

extern int gLevel;

#define ABS(x) ((x)>0?(x):(-(x)))

void MoveSprite(SpritePtr me,int size) {
	Point *p;

    p = (Point *)(&me->appLong);
	p->h += me->speed.h;
	p->v += me->speed.v;

	me->position.h += p->h/moveGranularity;
	me->position.v += p->v/moveGranularity;
	p->h %= moveGranularity;
	p->v %= moveGranularity;

	if (size>0) {
		if (me->position.v <= -size) me->position.v=gSAT.offSizeV+size+me->position.v;
		else if (me->position.v>gSAT.offSizeV) me->position.v -= gSAT.offSizeV+size-3;
		
		if (me->position.h <= -size) me->position.h = gSAT.offSizeH+size+me->position.h;
		else if (me->position.h>gSAT.offSizeH) me->position.h -= gSAT.offSizeH+size-3;
	}
}


void DrawStringInRect(StringPtr buf,Rect *r) {
	int index,start;
	int pos,line=1;
	index = 1;
	while (index<buf[0]) {
		pos = r->left;
		start = index;
		while (pos < r->right && index<buf[0] && buf[index] != 13) {
			pos += CharWidth(buf[index]);
			index++;
		}
		if (pos >= r->right) while (buf[index]!=' ') index--;
		if (index==buf[0]) index++;
		MoveTo(r->left,r->top + (thePort->txSize)*line);
		DrawText(buf,start,(index-start));
		index++;
		line++;
	}
}

void DrawCStringInRect(char *buf,Rect *r) {
	int index,start;
	int pos,line=1;
	index = 0;
	while (buf[index]) {
		pos = r->left;
		start = index;
		while (pos < r->right && buf[index] && buf[index] != 13) {
			pos += CharWidth(buf[index]);
			index++;
		}
		if (pos >= r->right) while (buf[index]!=' ') index--;
		// if (index==buf[0]) index++;
		MoveTo(r->left,r->top + (thePort->txSize+2)*line);
		DrawText(buf,start,(index-start));
		if (buf[index]) index++;
		line++;
	}
}

void GetAsteroidSpeed(Point *p) {
	int maxV, min, lev;
	switch (GetDifficulty()) {
		case kBeginnerDif:maxV = 3*moveGranularity;break;
		case kNormalDif:maxV = 6*moveGranularity;break;
		case kExpertDif:maxV = 9*moveGranularity;break;
		case kReallyExpertDif:maxV = 12*moveGranularity;break;
		case kReallyExpertDif+1:maxV = 15*moveGranularity;break;
	}
	if (GetPrefNewRules()) {
		lev = gLevel;
		if (lev>30) lev = 30;
		maxV += lev/3 * (moveGranularity/2);
	}
	min = moveGranularity/8;

	if (GetPrefNewRules()) min += lev/3 * (moveGranularity/8);
	
	do {
		p->h = RangedRdm(0,maxV) - maxV/2;
	} while (ABS(p->h) < min);

	do {
		p->v = RangedRdm(0,maxV) - maxV/2;
	} while (ABS(p->v) < min);
}

void GetAsteroidSpeedFromBullet(Point *p, Point *bulletSpeed) {
	GetAsteroidSpeed(p);
	p->h = p->h * 7 / 8;
	p->v = p->v * 7 / 8;
	p->h += bulletSpeed->h/8;
	p->v += bulletSpeed->v/8;
}