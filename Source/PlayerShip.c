/* PlayerShip sprite for Asterax */

#include <math.h>
#include <ToolUtils.h>
#include "SAT.h"
#include "AsteraxResources.h"
#include "Sprites.h"
#include "PointsToAngle.h"
#include "Strings.h"

#define rHyperSound	225
#define nShipFaces	36
#define nShips		3
#define kCheatShip1 101
#define kCheatShip2 102

extern Boolean noMoreRocks;

int GetSearchDepth(void);

void ShipTarget(SpritePtr me,PlayerShipData *data);
void ShipThrust(SpritePtr me, PlayerShipData *data);
void CheckForGameOver(SpritePtr me);
void HandleSpecialAction(SpritePtr me, PlayerShipData *data);
void ShipTurnRight(SpritePtr me,PlayerShipData *data);

void SetUpCheatShip(PlayerShipData *data, int playerNum);
FacePtr	shipFaces[nShips][nShipFaces];

Handle thrustSound, hyperSound;

static float fastSin[nShipFaces],fastCos[nShipFaces];

void InitPlayerShip()
{
	int i,j;
	for (j=0;j<nShips;j++) {
		for(i=0;i<nShipFaces;i++) shipFaces[j][i] = GetFace(rBaseShipIcon + rShipDelta * j + i);
	}
	thrustSound = SATGetSound(rShipThrustSound);
	hyperSound = SATGetSound(rHyperSound);
	for (i=0;i<nShipFaces;i++) {
		fastSin[i] = sin(i * 6.28 /nShipFaces);
		fastCos[i] = cos(i * 6.28 /nShipFaces);
	}
}

pascal void SetupPlayerShip (SpritePtr me)
{
	PlayerShipData *data;

	me->task = HandlePlayerShip;
	me->hitTask = HitPlayerShip;

	me->mode = 0;
	me->speed.h = 0;
	me->kind = 1; /*Friend kind*/
	SetRect(&me->hotRect, 0, 0, 32, 32);
	
	me->speed.h = me->speed.v =0;
	
	me->appPtr = (Ptr)NewPtr(sizeof(PlayerShipData));
	me->appLong = 0;
	((PlayerShipData *)me->appPtr)->suspend = FALSE;
	((PlayerShipData *)me->appPtr)->noShieldLoss = FALSE;
}

pascal void HandlePlayerShip (SpritePtr me)
{
	PlayerShipData *data;
	SpritePtr new;
	
	data = (PlayerShipData *)me->appPtr;
	if (data->suspend) return;
	if (data->thrustSoundCount) data->thrustSoundCount--;

	me->face = shipFaces[data->graphicID][me->mode];
//	if (!data->noShieldLoss) data->shields--;
	if (data->levelStartCount) {
	 	if (data->levelStartCount % 3 == 0) {
	 		new = NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupShieldHit);
			new->speed.h = me->speed.h;
			new->speed.v = me->speed.v;
			new->mode = 11;
		}
		data->levelStartCount--;
	}
	
	if (me->speed.h > data->topSpeed) me->speed.h -= moveGranularity;
	else if (me->speed.h < -data->topSpeed) me->speed.h += moveGranularity;
	if (me->speed.v > data->topSpeed) me->speed.v -= moveGranularity;
	else if (me->speed.v < -data->topSpeed) me->speed.v += moveGranularity;
	
	MoveSprite(me,32);

	if (data->whichSpecialActive >= 0) {
		HandleSpecialAction(me,data);
	}
	
	if (!data->noControl) {
		PlayerShipKeyControl(me,data);
	} else if (data->target.h && data->target.v) {
		ShipTarget(me,data);	
	}
	
	if (data->shields <= 0) {
		PlayerShipDeath(me);
	}
	CheckForGameOver(me);
}

void HandleSpecialAction(SpritePtr me, PlayerShipData *data) {
	SpritePtr newShot;
	
	switch (data->whichSpecialActive) {
		case sStarburst:
			switch (data->specialData.starBurst.starBurstPhase) {
				case starBurstBegin:
					data->noControl = TRUE;
					data->target.h= data->target.v=0;
					if (data->specialData.starBurst.starBurstTimer > 30) {
						ShipTurnRight(me,data);
					} else if (data->specialData.starBurst.starBurstTimer > 15) {
						ShipTurnRight(me,data);ShipTurnRight(me,data);
					} else {
						ShipTurnRight(me,data);ShipTurnRight(me,data);ShipTurnRight(me,data);
					}
					data->specialData.starBurst.starBurstTimer --;
					if (data->specialData.starBurst.starBurstTimer == 0) {
						data->specialData.starBurst.starBurstPhase = starBurstGo;
						data->specialData.starBurst.starBurstTimer = 120;
						ConfigureSAT (FALSE, kVPositionSort, kKindCollision, GetSearchDepth()*2);
					}
					break;
				case starBurstGo:
					ShipTurnRight(me,data);ShipTurnRight(me,data);ShipTurnRight(me,data);ShipTurnRight(me,data);ShipTurnRight(me,data);
					newShot = NewSpriteAfter(me, 1, me->position.h+16, me->position.v+16, SetupPlayerShot);
					newShot->speed.h = me->speed.h + data->shotSpeed*moveGranularity * fastSin[me->mode];
					newShot->speed.v = me->speed.v - data->shotSpeed*moveGranularity * fastCos[me->mode];
					newShot->appPtr = (Ptr)data;
					data->specialData.starBurst.starBurstTimer --;
					if (data->specialData.starBurst.starBurstTimer == 0) {
						data->specialData.starBurst.starBurstPhase = starBurstEnd;
						data->specialData.starBurst.starBurstTimer = 60;

					}	
					break;
				case starBurstEnd:
					if (data->specialData.starBurst.starBurstTimer > 45) {
						ShipTurnRight(me,data);ShipTurnRight(me,data);ShipTurnRight(me,data);
					} else if (data->specialData.starBurst.starBurstTimer > 30) {
						ShipTurnRight(me,data);ShipTurnRight(me,data);
					} else {
						ShipTurnRight(me,data);
					}
					data->specialData.starBurst.starBurstTimer --;
					if (data->specialData.starBurst.starBurstTimer == 0) {
						data->shotsFired = 0;
						data->shotTime = 0;						
						data->noControl = FALSE;
						data->whichSpecialActive = -1;
						ConfigureSAT (FALSE, kVPositionSort, kKindCollision, GetSearchDepth());
					}
					break;
			}
			break;
		case sSuperShields:
			data->specialData.superShield.theShieldSprite->position.h = me->position.h-6;
			data->specialData.superShield.theShieldSprite->position.v = me->position.v-6;
			data->specialData.superShield.superShieldTimer --;

			if (data->specialData.superShield.superShieldTimer < 30) {
				if (data->specialData.superShield.superShieldTimer % 5 < 2) {
					data->specialData.superShield.theShieldSprite->position.h = -300;
				}
			}

			if (!data->specialData.superShield.superShieldTimer) {
				data->specialData.superShield.theShieldSprite->task = NULL;
				data->whichSpecialActive = -1;
			}
			break;
		case sGrabber:
			if (!data->specialData.grabber.stillOut) {
				data->specialData.grabber.grabberSprite->task = NULL;
				data->whichSpecialActive = -1;
				data->redrawInfo = TRUE;
			}
			break;
		case sHyperspace:
		case sRecharge:
			data->specialData.timer.theTime --;
			if (!data->specialData.timer.theTime) {
				data->whichSpecialActive = -1;
				data->redrawInfo = TRUE;				
			}
			break;
	}
}

void ShipTarget(SpritePtr me,PlayerShipData *data) {
	short dh,dv;
	double angle;
	int theAngle;
	float costheta,sintheta;

	dh = data->target.h - me->position.h;			/* calculate the deltas */
	dv = data->target.v - me->position.v;

	if (!dh) angle = 3.14/2;
	else angle = (atan((double)dv/dh) * 180 / 3.14);
	theAngle = angle / 10;
	theAngle += 45;
	if (dh <0) theAngle+=18;
	theAngle %= 36;
	
	if (me->mode != theAngle) {
		if (ABS((me->mode-1-theAngle)) < ABS((me->mode+1-theAngle))) me->mode--;
		else me->mode++;
		me->mode = (me->mode + 36)%36;
	}
	if (ABS(me->mode-theAngle) < 3) {
		if (dh>30) me->speed.h = moveGranularity*4;
		else if (dh>10) me->speed.h = moveGranularity*2;
		else if (dh>5) me->speed.h = moveGranularity;
		else if (dh>2) me->speed.h = moveGranularity/2;
		else if (dh<-30) me->speed.h = -moveGranularity*4;
		else if (dh<-10) me->speed.h = -moveGranularity*2;
		else if (dh<-5) me->speed.h = -moveGranularity;
		else if (dh<-2) me->speed.h = -moveGranularity/2;

		if (dv>30) me->speed.v = moveGranularity*4;
		else if (dv>10) me->speed.v = moveGranularity*2;
		else if (dv>5) me->speed.v = moveGranularity;
		else if (dv>2) me->speed.v = moveGranularity/2;
		else if (dv<-30) me->speed.v = -moveGranularity*4;
		else if (dv<-10) me->speed.v = -moveGranularity*2;
		else if (dv<-5) me->speed.v = -moveGranularity;
		else if (dv<-2) me->speed.v = -moveGranularity/2;
	}
}

void ShipTurnRight(SpritePtr me,PlayerShipData *data) {
	if (GetPrefSpeed()==1) {
		if ((++data->turnCount)%2==0) me->mode++;
	} else me->mode++;
	if (me->mode == nShipFaces) me->mode = 0;
}

void ShipTurnLeft(SpritePtr me, PlayerShipData *data) {
	if (GetPrefSpeed()==1) {
		if ((++data->turnCount)%2==0) me->mode--;
	} else me->mode--;
	if (me->mode<0) me->mode = nShipFaces-1;
}

void ShipThrust(SpritePtr me, PlayerShipData *data) {
	Point tempVel;
	int	  curAccel;
	long  abVel,dif,ts,newX,newY,newVel;
/*
	when the absolute velocity is close to data->topSpeed, the accleration should
	be small.  When the absolute velocity is close to 0, the acceleration
	should be big.
	
	(data->topSpeed - vel)	= 0 at top speed
	data->topSpeed - vel = data->topSpeed at 0
*/
	
	abVel = (long)me->speed.h*me->speed.h + (long)me->speed.v*me->speed.v;
	ts = (data->topSpeed * (long)data->topSpeed);
	dif =  ts - abVel;
	if (dif < 0) dif = 0;
	
	newX = (me->speed.h + data->thrustRate * fastSin[me->mode]);
	newY = (me->speed.v - data->thrustRate * fastCos[me->mode]);
	newVel = newX * newX + newY * newY;
	if (newVel < abVel) {
		me->speed.h = newX;
		me->speed.v = newY;
	} else {
		me->speed.h += data->thrustRate * dif / ts * fastSin[me->mode];
		me->speed.v -= data->thrustRate * dif / ts * fastCos[me->mode];
	}
	
//	tempVel.h = me->speed.h + data->thrustRate*fastSin[me->mode];
//	tempVel.v = me->speed.v - data->thrustRate*fastCos[me->mode];
	
	if (data->thrustSoundCount == 0) {
		data->thrustSoundCount = 15;
		if (AreAnyLeeches()) {
			PlaySound(kYummyChannel,thrustSound);
		} else {
			PlaySound(kBackgroundChannel,thrustSound);
		}
	}
	
//	if ((long)tempVel.h * tempVel.h + (long)tempVel.v * tempVel.v < data->topSpeed) {
//	if (ABS(tempVel.h) < data->topSpeed && ABS(tempVel.v) < data->topSpeed) {
//		me->speed.h = tempVel.h;
//		me->speed.v = tempVel.v;
//	}
}

void ShipRetro(SpritePtr me,PlayerShipData *data) {
	if (me->speed.h<data->retroRate && me->speed.h>-data->retroRate) me->speed.h=0;
	else {	
		if (me->speed.h<0) me->speed.h+=data->retroRate;
		else if (me->speed.h>0) me->speed.h-=data->retroRate;
	}
	if (me->speed.v<data->retroRate && me->speed.v>-data->retroRate) me->speed.v=0;
	else {	
		if (me->speed.v<0) me->speed.v+=data->retroRate;
		else if (me->speed.v>0) me->speed.v-=data->retroRate;
	}
}

void ShipFireShot(SpritePtr me, PlayerShipData *data) {
	SpritePtr newShot;
	int i;
	if (TickCount() - data->shotTime > data->shotDelay) {		
		if (data->shootMoney) {
			if (data->money) {
				if (me->mode == 9 || me->mode == 10 || me->mode == 28 || me->mode == 27) {
				 	newShot = NewSpriteAfter(me, -2, me->position.h+16, me->position.v+16, SetupMoney);
					newShot->speed.h = fastSin[me->mode] * data->shotSpeed * moveGranularity;			
					newShot->speed.v = 0;
					if (me->mode == 9 || me->mode == 10) newShot->position.h+=22;
					else if (me->mode == 28 || me->mode == 27) newShot->position.h-=28;
					data->money --;
					data->redrawInfo= TRUE;
				}
			}
		} else {
			for (i=0;i<data->shotsInOneShot && data->shotsFired < data->maxNumberShots;i++) {
				data->shotsFired++;
				data->shotsFiredInBurst++;
				newShot = NewSpriteAfter(me, 1, me->position.h+16, me->position.v+16, SetupPlayerShot);
	
				if (data->shotsInOneShot == 2) {
					newShot->speed.h = me->speed.h + data->shotSpeed*moveGranularity * sin((me->mode+.5-(float)i/2.0) * 6.28 / nShipFaces);
					newShot->speed.v = me->speed.v - data->shotSpeed*moveGranularity * cos((me->mode+.5-(float)i/2.0) * 6.28 / nShipFaces);
				} else if (data->shotsInOneShot == 3) {
					newShot->speed.h = me->speed.h + data->shotSpeed*moveGranularity * sin((me->mode-.5+(float)i/2.0) * 6.28 / nShipFaces);
					newShot->speed.v = me->speed.v - data->shotSpeed*moveGranularity * cos((me->mode-.5+(float)i/2.0) * 6.28 / nShipFaces);				
				} else {
					newShot->speed.h = me->speed.h + data->shotSpeed*moveGranularity * fastSin[me->mode];
					newShot->speed.v = me->speed.v - data->shotSpeed*moveGranularity * fastCos[me->mode];
				}
				newShot->appPtr = (Ptr)data;
				if (data->scatterShots) newShot->mode += 0x100;
			}
		}
		data->shotTime = TickCount();
		data->shotKeyRelease = FALSE;
	}
}

ShipSelectSpecial(SpritePtr me,PlayerShipData *data) {
	if (!data->nSpecials) {
		data->redrawSpecial = TRUE;
		return;
	}
	if (data->selSpecialPhase) return;
	data->specialSelected++;
	if (data->specialSelected == data->nSpecials) data->specialSelected=0;
	data->redrawSpecial = TRUE;
	data->selSpecialPhase = 32;
}

void PlotSpecialGraphic(Point p, PlayerShipData *data) {
	Rect r;
	RgnHandle oldClip;
	RGBColor c;
	int which;
	if (!data->nSpecials) {
		SetRect(&r,p.h,p.v,p.h+16,p.v+16);
		RGBFore(0,0,0);
		PaintRect(&r);
		return;
	}
	oldClip = NewRgn();
	GetClip(oldClip);

	c.red= c.blue=c.green =0;
	RGBForeColor(&c);
	if (data->selSpecialPhase > 16) {
		SetRect(&r,p.h,p.v,p.h+32-data->selSpecialPhase,p.v+16);
		PaintRect(&r);
		which = data->specialSelected-1;
		if (which<0) which = data->nSpecials-1;
		SetRect(&r,r.right,p.v,p.h+16,p.v+16);
		ClipRect(&r);
		p.h = r.left;
		PlotSmallSpecialFace(data->specials[which],&p);
		SetClip(oldClip);
		DisposeRgn(oldClip);
	} else {
		SetRect(&r,p.h,p.v,p.h+(16-data->selSpecialPhase),p.v+16);
		ClipRect(&r);
		p.h -= data->selSpecialPhase;
		PlotSmallSpecialFace(data->specials[data->specialSelected],&p);
		SetClip(oldClip);
		DisposeRgn(oldClip);
	}
	if (data->selSpecialPhase) {
		data->selSpecialPhase--;
		data->redrawSpecial = TRUE;
	}
}

ShipActivateSpecial(SpritePtr me,PlayerShipData *data) {
	SpritePtr sp;
	Boolean removeAfterUse = TRUE;
	int i;
	if (!data->nSpecials) return;
	if (data->selSpecialPhase) return;
	if (data->whichSpecialActive >= 0) return;
	
	data->whichSpecialActive = data->specials[data->specialSelected];
	switch (data->specials[data->specialSelected]) {
		case sStarburst:data->specialData.starBurst.starBurstPhase = starBurstBegin;
					    data->specialData.starBurst.starBurstTimer = 60;					    
					    break;
		case sSuperShields:
						data->specialData.superShield.theShieldSprite = NewSpriteAfter(me,1,me->position.h-6,me->position.v-6,SetupSuperShield);
						data->specialData.superShield.superShieldTimer = 180 + RangedRdm(60,240);
						break;
		case sGrabber:	sp = data->specialData.grabber.grabberSprite = NewSpriteAfter(me,1,me->position.h+4,me->position.v+4,SetupCrystalGrabber);
						sp->speed.h = fastSin[me->mode] * data->shotSpeed * moveGranularity;			
						sp->speed.v = -fastCos[me->mode] * data->shotSpeed * moveGranularity;							
						data->specialData.grabber.stillOut= TRUE;
						SetGrabberFlag(sp,&(data->specialData.grabber.stillOut));
						SetGrabberWhoLaunched(sp,data->pnum);
						removeAfterUse = FALSE;
						break;
		case sMine:  sp = NewSpriteAfter(me,1,me->position.h+4,me->position.v+4,SetupMine);
						sp->speed.h = fastSin[me->mode] * data->shotSpeed * moveGranularity/2;			
						sp->speed.v = -fastCos[me->mode] * data->shotSpeed * moveGranularity/2;							
						SetMineFirer(sp,data);
						data->whichSpecialActive = -1;
						break;
		case sHyperspace:// move to a random location
						me->position.h = RangedRdm(0,gGameRect.right);
						me->position.v = RangedRdm(0,gGameRect.bottom);
						me->speed.h /= 10;
						me->speed.v /= 10;
						removeAfterUse = FALSE;
						data->specialData.timer.theTime = 50;
						PlaySound(kYummyChannel,hyperSound);	
						break;		
		case sRecharge:IncreaseShipShields(data,4000);
						data->specialData.timer.theTime = 50;
						break;
	}
	if (removeAfterUse) {
		for (i=data->specialSelected;i<data->nSpecials-1;i++) {
			data->specials[i]=data->specials[i+1];
		}
		data->nSpecials--;
		data->specialSelected--;
		ShipSelectSpecial(me,data);
		data->selSpecialPhase = 16;
	}
}

PlayerShipKeyControl(SpritePtr me,PlayerShipData *data) {
	unsigned char km[16];
	int i;
	
	GetKeys( (long *) km);
	if (IsPressed(data->rightKey,km)) ShipTurnRight(me,data);
	if (IsPressed(data->leftKey,km)) ShipTurnLeft(me,data);

	if (!data->noThrust && IsPressed(data->thrustKey,km)) ShipThrust(me,data);
	else {
		if (data->thrustSoundCount && !AreAnyLeeches()) ShutUpChannel(kBackgroundChannel);
		data->thrustSoundCount = 0;
		if (!data->noThrust && data->retroRate) ShipRetro(me,data);
	}

	if (!data->noShieldLoss && IsPressed(data->fireKey,km) && 
		(data->shotKeyRelease || data->shotsFiredInBurst < data->shotsPerBurst) &&
		data->shotsFired < data->maxNumberShots) {
			ShipFireShot(me,data);
	} else if (!IsPressed(data->fireKey,km)) {
			data->shotKeyRelease = TRUE;
			data->shotsFiredInBurst = 0;
	}
	
	if (IsPressed(data->selSpecialKey,km)) ShipSelectSpecial(me,data);
	if (!data->noShieldLoss && IsPressed(data->actSpecialKey,km)) ShipActivateSpecial(me,data);
	
}

PlayerShipDeath(SpritePtr me) {
	// the player is DAID.
	SpritePtr new;
	PlayerShipData *data;
	int i;
	
	data = (PlayerShipData *)me->appPtr;

 	new = NewSpriteAfter(me, 0, me->position.h+Random()%6-3, me->position.v+Random()%6-3, SetupExplosion);
	new->speed = me->speed;

 	NewSpriteAfter(me, 0, me->position.h +Random()%6-3, me->position.v+Random()%6-3, SetupExplosion);
	new->speed.h = me->speed.h * 8 / 10; new->speed.v = me->speed.v * 8 / 10;
 
 	NewSpriteAfter(me, 0, me->position.h+Random()%6-3, me->position.v+Random()%6-3, SetupExplosion);
	new->speed.h = me->speed.h * 6 / 10; new->speed.v = me->speed.v * 6 / 10;
 
 	NewSpriteAfter(me, 0, me->position.h+Random()%6-3, me->position.v+Random()%6-3, SetupExplosion);
	new->speed.h = me->speed.h * 4 / 10; new->speed.v = me->speed.v * 4 / 10;
 
 	NewSpriteAfter(me, 0, me->position.h+Random()%6-3, me->position.v+Random()%6-3, SetupExplosion);
	new->speed.h = me->speed.h * 2 / 10; new->speed.v = me->speed.v * 2 / 10;

	data->dead = TRUE;
	PlayShipExplodingSound();
	
	for (i=0;i<data->money && i<30;i++) {
		if (RangedRdm(0,4)<3) {
		 	NewSpriteAfter(me, 0, me->position.h+Random()%6-3, me->position.v+Random()%6-3, SetupMoney);
			new->speed.h = RangedRdm(1,4*moveGranularity)-2*moveGranularity;
			new->speed.v = RangedRdm(1,4*moveGranularity)-2*moveGranularity;
		}
	}		
}



pascal void HitPlayerShip (SpritePtr me, SpritePtr him)
{
	// the player has collided with a rock!!!  this is bad.
	SpritePtr new;
	int dh,dv, lim, i;
	
	if (him->task == HandleBigAsteroid ||	
		him->task == HandleSmallAsteroid ||
		him->task == HandleMidAsteroid ||
		him->hitTask == HitEnemyShot) {

		dh = me->position.h - him->position.h;
		dv = me->position.v - me->position.v;	
		if (dh*dh + dv*dv < 32 *32) {
			// it's inside the circle... therefore a hit

			if (him->task == HandleBigAsteroid) {
				me->speed.h += him->speed.h * 2/3;
				me->speed.v += him->speed.v * 2/3;

				lim = RangedRdm(2,4);
				for (i=0;i<lim;i++) {
				 	new = NewSpriteAfter(me, -2, him->position.h, him->position.v, SetupMidAsteroid);
					GetAsteroidSpeed(&new->speed);
					new->speed.h += me->speed.h/4;
					new->speed.v += me->speed.v/4;
				}
				him->task = NULL;
				PlayDestroyBigRockSound();
				NewSpriteAfter(me, 0, him->position.h, him->position.v, SetupExplosion);
				DecreaseShipShields((PlayerShipData *)me->appPtr,2800);
			} else if (him->task == HandleMidAsteroid) {
				me->speed.h += him->speed.h /3;
				me->speed.v += him->speed.v /3;

				lim = RangedRdm(2,4);
				for (i=0;i<lim;i++) {
				 	new = NewSpriteAfter(me, -2, him->position.h, him->position.v, SetupSmallAsteroid);
					GetAsteroidSpeed(&new->speed);
					new->speed.h += me->speed.h/4;
					new->speed.v += me->speed.v/4;
				}
				him->task = NULL;
				PlayDestroyBigRockSound();
				NewSpriteAfter(me, 0, him->position.h, him->position.v, SetupExplosion);
				DecreaseShipShields((PlayerShipData *)me->appPtr,1600);
			} else if (him->task == HandleSmallAsteroid) {
				me->speed.h += him->speed.h /5;
				me->speed.v += him->speed.v /5;
				
				him->task = NULL;
				PlayDestroyBigRockSound();
				NewSpriteAfter(me, 0, him->position.h, him->position.v, SetupExplosion);
				DecreaseShipShields((PlayerShipData *)me->appPtr,1000);
			}

		 	new = NewSpriteAfter(me, 0, me->position.h, me->position.v, SetupShieldHit);
			new->speed.h = me->speed.h;
			new->speed.v = me->speed.v;
		}
	}
}


PlayerShipData *SetUpShip(int type) {
	 PlayerShipData *data;
	int i;

	data = (PlayerShipData *)NewPtr(sizeof(PlayerShipData));
	if (!data) {
		ExitToShell();
	}



	data->fracLoc.h = 0;
	data->fracLoc.v = 0;
	data->money = 0;
	data->value = 0;
	data->turnCount = 0;
	data->score = 0;
	data->target.h = data->target.v = 0;
	data->dead = FALSE;
	data->suspend = FALSE;
	data->noThrust = FALSE;
	data->noShieldLoss = FALSE;
	data->noControl = FALSE;
	data->shootMoney = FALSE;
	data->redrawInfo = TRUE;
	data->redrawScore = TRUE;
	data->redrawSpecial = FALSE;
	data->selSpecialPhase = 0;
	data->levelStartCount = 90;
	data->shotKeyRelease = TRUE;
	data->thrustSoundCount = 0;


// just to be careful:
	data->maxNumberShots = 8;
	data->shotsPerBurst = 1;
	data->shotsInOneShot = 1;
	data->shotDelay = 6;
	data->shotSpeed = 4;

	data->shieldMax = data->shields = 8000;
	data->thrustRate = moveGranularity/10;
	data->retroRate = 0;
	data->topSpeed = 4 * moveGranularity;				
	data->turnRate = 1;

	data->graphicID = type;
	data->safeShots = FALSE;
	data->scatterShots = FALSE;
	data->shotsFired = 0;
	data->shotTime = 0;
	data->shotsFiredInBurst = 0;
	data->specialSelected = 0;
	data->nSpecials = 0;
	data->whichSpecialActive = -1;

	if (type == kCheatShip1) {
		SetUpCheatShip(data, 1);
		return data;
	} else if (type == kCheatShip2) {
		SetUpCheatShip(data, 2);
		return data;
	}	

	switch (type) {
		case 0:// crab
				// shots:
				data->maxNumberShots = 8;
				data->shotsPerBurst = 1;
				data->shotsInOneShot = 1;
				data->shotDelay = 6;
				data->shotSpeed = 4;

				data->shieldMax = data->shields = 8000;
				data->thrustRate = moveGranularity/10;
				data->retroRate = 0;
				data->topSpeed = 4 * moveGranularity;				
				data->turnRate = 1;
				break;
		case 1: // mantis		
				data->maxNumberShots = 4;
				data->shotsPerBurst = 1;
				data->shotsInOneShot = 1;
				data->shotDelay = 6;
				data->shotSpeed = 4;
				data->shieldMax =data->shields = 4000;
				data->thrustRate = moveGranularity/5;
				data->retroRate = 0;
				data->topSpeed = 8*moveGranularity;				
				data->turnRate = 1;
				break;		
		case 2: // moth
				data->maxNumberShots = 6;
				data->shotsPerBurst = 12;
				data->shotsInOneShot = 1;
				data->shotDelay = 6;
				data->shotSpeed = 4;

				data->shieldMax =data->shields = 6000;
				data->thrustRate = moveGranularity/8;
				data->retroRate = 0;
				data->topSpeed = 6L*moveGranularity;				
				data->turnRate = 1;
				break;						
		
	}

//	data->scatterShots = TRUE;

/*	data->specials[data->nSpecials] = sSuperShields;
	data->specialSelected = data->nSpecials;
	data->nSpecials++;									
	
	data->specials[data->nSpecials] = sMine;
	data->specialSelected = data->nSpecials;
	data->nSpecials++;								
		
	data->shieldMax = data->shields = 12000;
	data->shotsInOneShot = 3;
	data->safeShots= TRUE;									
	data->maxNumberShots = 20;
*/

	return data;
}

void AddEquipmentToShip(PlayerShipData *data, int itemType, int itemCode) {
	switch (itemType) {
		case kWeaponItem:break;
		case kShieldItem:break;
		case kEngineItem:break;
		case kSpecialItem:break;
	}
}


void GetEquipmentGameEffect(StringPtr str,int itemType,int itemCode) {
	char *ch = (char *)str;
	switch (itemType) {
		case kWeaponItem:break;
		case kShieldItem:break;
		case kEngineItem:break;
		case kSpecialItem:break;
	}
}

void CheckForGameOver(SpritePtr me) {
 	// check to see if there are no more rocks (ugh.  there _must_ be a better way!)
 	SpritePtr check;
	Boolean any= FALSE;
 
 	check = me->next;
	while (check && !any) {
		if (check->task == HandleSmallAsteroid ||
				check->task == HandleMidAsteroid ||
				check->task == HandleBigAsteroid) any=TRUE;
			check=check->next;
   	}
   	if (!any) {
		check = me->prev;
		while (check && !any) {
		if (check->task == HandleSmallAsteroid ||
				check->task == HandleMidAsteroid ||
				check->task == HandleBigAsteroid) any=TRUE;
		check=check->prev;	   			
   		}
   	}
   	noMoreRocks = !any;
}

void DecreaseShipShields(PlayerShipData *data, int howMuch) {
	if (data->whichSpecialActive == sSuperShields) return;
	if (data->levelStartCount) return;
	data->shields-=howMuch;
}

void IncreaseShipShields(PlayerShipData *data, int howMuch) {
	data->shields += howMuch;
	if (data->shields > data->shieldMax) data->shields = data->shieldMax;
}

int ShipShields(PlayerShipData *data) {
	return data->shields;
}

void IncreaseShipMoney(PlayerShipData *data, int howMuch) {
	data->money += howMuch;
}

void PlotOneShip(int which, Point *p) {
	SATPlotFaceToScreen(shipFaces[which][21], *p, TRUE);
}

void DrawShipAndName(int which, Point *p) {
	RGBColor c;
	Str31 buffer;

	GetIndString(buffer, rShipNameStrings, which+1);
	c.red=c.blue=c.green=0;RGBForeColor(&c);
	PlotOneShip(which,p);
	c.red=65535;c.blue=c.green=0;RGBForeColor(&c);
	MoveTo(p->h+16-StringWidth(buffer)/2,p->v+44);
	DrawString(buffer);
}

pascal void SetupShipPuppet (SpritePtr me)
{
	me->task = HandleShipPuppet;
	me->hitTask = NULL;
	me->mode = 0;
	me->speed.h = 0;
	me->kind = 1; /*Friend kind*/
	SetRect(&me->hotRect, 0, 0, 32, 32);
	me->speed.h = me->speed.v =0;	
	me->appPtr = NULL;
	me->appLong = 0;
}

pascal void HandleShipPuppet (SpritePtr me)
{
	Point origin;
	int angle;
	
	me->mode ++;
	if (me->mode == nShipFaces) me->mode = 0;
	me->face = shipFaces[(int)me->appPtr][me->mode];
	MoveSprite(me,32);
}