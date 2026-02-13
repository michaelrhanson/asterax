/* Main-game related code */

#include <stdio.h>
#include "SAT.h"
#include "AsteraxResources.h"
//#include "DefaultDialog.h"
#include "gamma.h"
#include "Sprites.h"
#include "Game.fp"
#include "HighScores.h"
#include "Preferences.h"
#include "Pause.h"
#include "Strings.h"
#include "MainResources.h"

RGBColor cMidGray = {32768,32768,32768};
RGBColor cDarkGray = {16384,16384,16384};
RGBColor cVeryDarkGray = {8192,8192,8192};
RGBColor cLightGray = {32768+16384,32768+16384,32768+16384};
RGBColor cBlack = {0,0,0};
RGBColor cWhite = {65535,65535,65535};
RGBColor cRed = {65535,0,0};
RGBColor cBlue = {0,0,65535};
RGBColor cCyan = {0,65535,65535};
RGBColor cMagenta = {65535,0,65535};
RGBColor cGreen = {0,65535,0};
RGBColor cYellow = {65535,65535,0};
RGBColor cMidRed = {32768,0,0};
RGBColor cMidGreen = {0,32768,0};
RGBColor cMidYellow = {32768,32768,0};

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)<(b))?(b):(a))
#define kCheatShip1 101
#define kCheatShip2 102

/* Game globals: */
/* Baaaaaaad.  Very baaaaad.*/
#define scoreBarHeight 14

int gLevel,gNPlayers;
int shieldWordLength;

SpritePtr p1ship, p2ship;
Rect p1scoreR,p2scoreR, p1ShieldR, p2ShieldR;
Rect p1InfoRect,p2InfoRect;
Boolean levelOver, noMoreRocks, gameOver;
long p1ShieldRWidth,p2ShieldRWidth;
Boolean playerOneDead, playerTwoDead;

int p1Lives, p2Lives;
int p1Bank, p2Bank;
int p1ShipType, p2ShipType;
int gDifficulty;

long hold1score, hold2score;
extern char	p1keys[keyNum],p2keys[keyNum];

PlayerShipData *p1insurance, *p2insurance;


/* Prototypes: */
void DrawPlayerShields(PlayerShipData *data);
void RedrawPlayerShipInfo(PlayerShipData *data);
void SetInsurance(PlayerShipData *insur, int pnum, PlayerShipData *data);
void DisplayDepositMessage();
void HandlePlayerInsurancePayment(PlayerShipData *data, PlayerShipData **insurPtr, int pnum, int vpos);
void SetGameRectangles();
void HandlePeriodicShip(int *oldshields, long *holdscore, PlayerShipData *data);
void ProcessNewGame(void);
void DoTheGame(void);

void RunGame(int nPlayers) {
	gLevel = 1;
	gNPlayers = nPlayers;

	gameWasEndedWithSuspend = FALSE;
	DetermineGameParameters(nPlayers,&p1ShipType,&p2ShipType);
	FadeDown(20);
	PaintRect(&gGameRect);

	if (p1ShipType == -1) return;	
	DoTheGame();
	if (!gameWasEndedWithSuspend) {
		RegisterHighScore(hold1score,gLevel,(gNPlayers==2),1);
		if (gNPlayers==2) RegisterHighScore(hold2score,gLevel,TRUE,2);
	}
	CleanUpGame();
}

void DoTheGame(void) {
	SATHideMBar(NULL);
	SetGameRectangles();
	HideCursor();
	SetupGameSprites();
	gameOver = FALSE;
	FadeUp(20);
	ProcessNewGame();	
	RGBFore(0,0,0);
}

void RunCheatGame(int nPlayers, int startingLevel) {
	gLevel  = startingLevel;
	gNPlayers = nPlayers;
	gameWasEndedWithSuspend = FALSE;
	p1ShipType = kCheatShip1;
	p2ShipType = kCheatShip2;
	DoTheGame();	
	CleanUpGame();
}

void ResumeGame(void) {
	PlayerShipData *data1=NULL, *data2=NULL;

	gameOver = FALSE;
	GetSuspendParameters(&gLevel, &hold1score, &p1Lives, &p1Bank, &data1, &p1insurance,&hold2score, &p2Lives, &p2Bank,&data2, &p2insurance);
	gameWasEndedWithSuspend = FALSE;
	FadeDown(20);
	PaintRect(&gGameRect);

	SATHideMBar(NULL);
	SetGameRectangles();
	HideCursor();

	if (data1) {
		p1ship = NewSprite(0, gGameRect.right/2-30, gGameRect.bottom/2, SetupPlayerShip);
		p1ship->appPtr = (Ptr)data1;
		playerOneDead = FALSE;
		data1->pnum = 1;
		p1ShipType = data1->graphicID;
		LoadPlayerKeys(1, data1);
	} else {
		playerOneDead = TRUE;
		p1ship = NULL;		
	}

	if (data2) {
		p2ship = NewSprite(0, gGameRect.right/2+30, gGameRect.bottom/2, SetupPlayerShip);
		p2ship->appPtr = (Ptr)data2;
		playerTwoDead = FALSE;
		data2->pnum = 2;
		p2ShipType = data2->graphicID;
		LoadPlayerKeys(2, data2);

		gNPlayers = 2;
	} else {
		playerTwoDead = TRUE;
		p2ship = NULL;		
		gNPlayers = 1;
	}
	FadeUp(20);
	HandlePurchases();	

	if (!gameOver) {
		gLevel ++;	
		ResetAnyLeeches();
		ResetPlayerShips();

		DisplayBeginLevelMessage();
		ProcessGame();

		RGBFore(0,0,0);
		if (!gameWasEndedWithSuspend) {
			RegisterHighScore(hold1score,gLevel,(gNPlayers==2),1);
			if (gNPlayers==2) RegisterHighScore(hold2score,gLevel,TRUE,2);
		}
	}
	CleanUpGame();
	FadeUp(20);
}

void SetGameRectangles() {
	SetRect(&p1scoreR,gGameRect.left,gGameRect.top,gGameRect.left+gGameRect.right /4-80,gGameRect.top+scoreBarHeight);
	SetRect(&p2scoreR,gGameRect.left+gGameRect.right/2,gGameRect.top, gGameRect.left+gGameRect.right*3/4-80,gGameRect.top+scoreBarHeight);
	SetRect(&p1ShieldR,gGameRect.left+gGameRect.right/4+10,gGameRect.top, gGameRect.left+gGameRect.right/2,gGameRect.top+scoreBarHeight);
	SetRect(&p2ShieldR,gGameRect.left+gGameRect.right*3/4+10,gGameRect.top, gGameRect.left+gGameRect.right,gGameRect.top+scoreBarHeight);
	SetRect(&p1InfoRect,p1scoreR.right,gGameRect.top,p1ShieldR.left, gGameRect.top+scoreBarHeight);
	SetRect(&p2InfoRect,p2scoreR.right,gGameRect.top,p2ShieldR.left, gGameRect.top+scoreBarHeight);
	p1ShieldRWidth = p1ShieldR.right - p1ShieldR.left;
	p2ShieldRWidth = p2ShieldR.right - p2ShieldR.left;
}

void ProcessNewGame(void) {
	playerOneDead = playerTwoDead = FALSE;

	p1Lives = 3;
	p1Bank = p2Bank = 0;
	if (gNPlayers==2) p2Lives = 3;
	else p2Lives = 0;

	hold1score = hold2score = 0;
	p1insurance  = p2insurance = NULL;
	ProcessGame();
}


ProcessGame() {
	long 				l,levelOverDelay, newLifeDelay1, newLifeDelay2, gameOverDelay, periodicEvent;
	SpritePtr 			list;
	int 				shipCount,oldShield1, oldShield2,j,alienNum;
	unsigned char 		km[16];
	PlayerShipData 		*dataOne, *dataTwo;
	
	
	while (TRUE) {
		SetupLevel();
		RunSAT(FALSE);
		PeekOffscreen();
		DrawStructure();

		levelOverDelay=0;
		levelOver = FALSE;
		noMoreRocks = FALSE;

		oldShield1 = oldShield2 = -1;
		if (p1Lives) DrawScore((PlayerShipData *)p1ship->appPtr);
		if (p2Lives) DrawScore((PlayerShipData *)p2ship->appPtr);

		if (p1Lives) RedrawPlayerShipInfo((PlayerShipData *)p1ship->appPtr);
		if (p2Lives) RedrawPlayerShipInfo((PlayerShipData *)p2ship->appPtr);

		periodicEvent = GetAlienTimeDelay();
		dataOne = NULL;
		dataTwo = NULL;
		while (TRUE) {
			l = TickCount();
			RunSAT(PrefFastDraw()); /* Run the animation */

			dataOne = dataTwo = NULL;
			if (p1Lives > 0 && !playerOneDead) dataOne = (PlayerShipData *)p1ship->appPtr;
			if (p2Lives > 0 && !playerTwoDead) dataTwo = (PlayerShipData *)p2ship->appPtr;

			if (dataOne) HandlePeriodicShip(&oldShield1,&hold1score,dataOne);			
			if (dataTwo) HandlePeriodicShip(&oldShield2,&hold2score,dataTwo);						

			if (playerOneDead && p1Lives >0) {
				newLifeDelay1++;
				
				if (newLifeDelay1 > 120) {
					CreateP1Ship();
					playerOneDead = FALSE;
					((PlayerShipData *)(p1ship->appPtr))->score = hold1score;								
					RedrawPlayerShipInfo(dataOne);
					DrawScore(dataOne);
					DrawPlayerShields(dataOne);

				}
			}
			
			if (playerTwoDead && p2Lives > 0) {
				newLifeDelay2++;
				if (newLifeDelay2 > 120) {
					CreateP2Ship();
					playerTwoDead = FALSE;
					((PlayerShipData *)(p2ship->appPtr))->score = hold2score;								
					RedrawPlayerShipInfo(dataTwo);
					DrawScore(dataTwo);		
					DrawPlayerShields(dataTwo);
				}
			}
						
			if (!playerOneDead && dataOne && dataOne->dead) {
				// player one has died!

				playerOneDead = TRUE;

				newLifeDelay1 = 0;
				p1Lives--;
				
				RedrawPlayerShipInfo(dataOne);
				DrawScore(dataOne);
				DrawPlayerShields(dataOne);

				if ((p1Lives <= 0 && gNPlayers == 1) || (p1Lives <= 0 && gNPlayers == 2 && p2Lives <=0)) {
					gameOver = TRUE;
					gameOverDelay = 0;
				} else {
					if (p1Lives > 0) DrawPlayerShipRebuildingMessage(dataOne);
				}

				p1ship->task = NULL;
				//DisposePtr(p1ship->appPtr);
				//p1ship->appPtr = NULL;
			}
			
			if (!playerTwoDead && p2Lives && dataTwo && dataTwo->dead) {
				// player two has died!
				playerTwoDead = TRUE;
				newLifeDelay2 = 0;
				p2Lives--;
				RedrawPlayerShipInfo(dataTwo);
				DrawScore(dataTwo);		
				DrawPlayerShields(dataTwo);
	
				if ((p1Lives <= 0 && gNPlayers == 2 && p2Lives <=0)) {
					gameOver = TRUE;
					gameOverDelay = 0;		
				} else {
					if (p2Lives > 0) DrawPlayerShipRebuildingMessage(dataTwo);
				}
				p2ship->task = NULL;
				//DisposePtr(p2ship->appPtr);
				//p2ship->appPtr = NULL;
			}

			/* Check for periodic event */
			periodicEvent--;
			if (periodicEvent < 100) {
				if (alienProximityWarning) {
					if (periodicEvent % 30 == 0) {
						PlayMinePing();
					}
				}
			
			}
			if (!periodicEvent) periodicEvent = HandlePeriodicAlienCheck();

			/* Sit and spin */
			if (syncToClock) while (l > TickCount() - GetPrefSpeed()) /*Maximize speed to 30 fps*/;
			
			/* Exit Conditions */
			if (noMoreRocks) {
				levelOverDelay++;
				if (levelOverDelay > 120 && !gameOver) break;
			}
			if (gameOver) {
				gameOverDelay ++;
				if (gameOverDelay > 180) break;
			}
			
			/* Break Keys */
			GetKeys( (long *) km);
			if (IsPressed(escapeKey,km) || (IsPressed(q_key,km) && IsPressed(commandKey,km))) {
				gameOver = TRUE;
				RGBFore(0,0,0);
				break;
			}
			if (IsPressed(capsLock,km)) {
				gameOver = HandlePause(FALSE);
				if (gameOver) break;
				if (dataOne) {
					DrawScore(dataOne);
					DrawPlayerShields(dataOne);
					RedrawPlayerShipInfo(dataOne);
				}
				if (dataTwo) {
					DrawScore(dataTwo);		
					RedrawPlayerShipInfo(dataTwo);				
					DrawPlayerShields(dataTwo);
				}
				DrawStructure();
			}
		}
		if (gameOver) break;		
		levelOver = TRUE;
		newLifeDelay1 = 999;newLifeDelay2=999;
		RunSAT(PrefFastDraw());

		if (playerOneDead && p1Lives >0) {
			CreateP1Ship();
			playerOneDead = FALSE;
			((PlayerShipData *)(p1ship->appPtr))->score = hold1score;								
		}
		
		if (playerTwoDead && p2Lives > 0) {
			CreateP2Ship();
			playerTwoDead = FALSE;
			((PlayerShipData *)(p2ship->appPtr))->score = hold2score;								
		}
		
		ResetPlayerShips();
		HandleEndOfLevel();
		if (gameOver) break;
		ResetPlayerShips();
	}
}


CleanUpGame() {
	SpritePtr foo,sp,temp;
	// first set the task of all sprites to NULL... create a new one and run forwards and backwards from it
	foo = NewSprite(0, 30, 30, SetupPlayerShip);
	sp = foo->next;
	while (sp) {
		sp->task = NULL;
		sp = sp->next;
	}
	sp = foo->prev;
	while (sp) {
		sp->task = NULL;
		sp = sp->prev;
	}
	foo->task = NULL;
	
	// then run one frame to get rid of all of them
	RunSAT(PrefFastDraw());

	// then kill 'em all.
	foo = NewSprite(0, 30, 30, SetupPlayerShip);
	sp = foo->next;
	while (sp) {
		temp = sp->next;
		KillSprite (sp);
		sp = temp;
	}
	sp = foo->prev;
	while (sp) {
		temp = sp->prev;
		KillSprite(sp);
		sp = temp;
	}
	KillSprite(foo);
	PeekOffscreen();
	
	// background should be redrawn for the next game, tho.
	
	ShowCursor();
	InvalRect(&gGameRect);
	SATShowMBar();
	SATHideMBar(NULL);
	ClearBackground();
	PeekOffscreen();
	ShutUpAllChannels();
	FlushEvents(everyEvent,0);
	FadeDown(20);
}

Handle GetDItemHandle(DialogPtr theD,int theItem) {
	short itemType;
	Handle theHndl;
	Rect theR;
	GetDItem(theD,theItem,&itemType,&theHndl,&theR);
	return theHndl;
}

SetupGameSprites() {
	CreateP1Ship();
	((PlayerShipData *)p1ship->appPtr)->levelStartCount = 0;
	((PlayerShipData *)p1ship->appPtr)->money = 0; // debugStartingMoney;

	if (gNPlayers == 2) {
		CreateP2Ship();
		((PlayerShipData *)p2ship->appPtr)->levelStartCount = 0;
		((PlayerShipData *)p2ship->appPtr)->money = 0; // debugStartingMoney;
	}
}

void CreateP1Ship(void) {
	PlayerShipData *data;

	p1ship = NewSprite(0, gGameRect.right/2 - 30, gGameRect.bottom/2, SetupPlayerShip);

	if (p1insurance) {
		p1ship->appPtr = (Ptr) p1insurance;
		p1insurance = NULL;
	} else {
		p1ship->appPtr = (Ptr)SetUpShip(p1ShipType);
		data = (PlayerShipData *)(p1ship->appPtr);
		LoadPlayerKeys(1, data);
		data->pnum = 1;
	}
}


void CreateP2Ship(void) {
	PlayerShipData *data;

	p2ship = NewSprite(0, gGameRect.right/2 + 30, gGameRect.bottom/2, SetupPlayerShip);
	if (p2insurance) {
		p2ship->appPtr = (Ptr)p2insurance;
		p2insurance = NULL;
	} else {
		p2ship->appPtr = (Ptr)SetUpShip(p2ShipType);
		data = (PlayerShipData *)(p2ship->appPtr);
		LoadPlayerKeys(2, data);
		data->pnum = 2;
	}
}

DrawStructure() {
	Rect r;
	Str31 buffer;
	
	TextFont(geneva);
	TextSize(9);
	TextFace(0);
	GetIndString(buffer, rGeneralStrings, rShields);
	shieldWordLength = StringWidth(buffer);
	TextFace(bold);
	RGBForeColor(&cDarkGray);
	MoveTo(gGameRect.left,p1scoreR.bottom);
	LineTo(gGameRect.right,p1scoreR.bottom);
	if (gGameRect.left != screenBits.bounds.left || gGameRect.right != screenBits.bounds.right ||
		gGameRect.bottom != screenBits.bounds.bottom || gGameRect.top != screenBits.bounds.top) {
		r = gGameRect;
		InsetRect(&r,-8,-8);
		RGBForeColor(&cCyan);
		PenSize(2,2);
//		OffsetRect(&r,-ox,-oy);
		FrameRoundRect(&r,4,4);
		PenSize(1,1);
	}

	RGBForeColor(&cBlack);
}

void HandlePeriodicShip(int *oldshields, long *holdscore, PlayerShipData *data) {
	if ((data->redrawScore)) {
		*holdscore = data->score;
		DrawScore(data);
		data->redrawScore = FALSE;
	}
	if (data->redrawInfo) RedrawPlayerShipInfo(data);
	if (data->redrawSpecial) RedrawPlayerShipSpecial(data);
	if (*oldshields/10 != data->shields/10) DrawPlayerShields(data);
	*oldshields = data->shields;
	RGBForeColor(&cBlack);
}


ResetPlayerShips() {
	if (p1Lives > 0) {
		p1ship->position.h = gGameRect.right/2 - 30;
		p1ship->position.v = gGameRect.bottom/2;
		p1ship->speed.h = p1ship->speed.v = 0;
		((PlayerShipData *)p1ship->appPtr)->shotsFired = 0;
	}
	if (p2Lives > 0) {
		p2ship->position.h = gGameRect.right/2 + 30;
		p2ship->position.v = gGameRect.bottom/2;
		p2ship->speed.h = p2ship->speed.v = 0;
		((PlayerShipData *)p2ship->appPtr)->shotsFired = 0;
	}
}

HandleEndOfLevel() {
	int j;
	PlayerShipData *p1dat=NULL,*p2dat=NULL;
	Boolean quit;

	if (p1Lives>0) p1dat = (PlayerShipData *)p1ship->appPtr;
	if (p2Lives>0) p2dat = (PlayerShipData *)p2ship->appPtr;

	DisplayDepositMessage();
	levelOver = FALSE;



	if (p1Lives >0 && p2Lives >0) {
		RedrawPlayerShipInfo((PlayerShipData *)p1ship->appPtr);
		RedrawPlayerShipInfo((PlayerShipData *)p2ship->appPtr);
		quit = HandleMoneyTransfer(p1dat, p2dat);
		if (quit) gameOver = TRUE;
	}
	
	if (!gameOver) HandlePurchases();	
	if (gameOver) return;
	gLevel ++;
	ResetAnyLeeches();
	DisplayBeginLevelMessage();
}	


void DisplayDepositMessage() {
	Str255 			buffer;
	Str15			levelStr, clearedStr;
	PlayerShipData 	*dat;
	RGBColor 		c;
	long 			l;
	Point 			p;
	Rect 			r;

	RGBForeColor(&cBlack);
	r = gGameRect;
	r.top += scoreBarHeight+1;
	PaintRect(&r);
	RGBForeColor(&cGreen);

	GetIndString(levelStr, rGeneralStrings, rLevel);
	GetIndString(clearedStr, rGeneralStrings, rCleared);
		
	sprintf((char *)buffer,"%#s %d %#s",levelStr, gLevel, clearedStr);
	CtoPstr((char *)buffer);
	MoveTo(gGameRect.right/2 - StringWidth(buffer)/2,gGameRect.bottom/2);
	DrawString(buffer);
	PlayLevelOverSound();
	if (p1Lives > 0) {
		DrawPlayerMoneyMessage(((PlayerShipData *)p1ship->appPtr)->money, 1,60,p1Bank);
		((PlayerShipData *)p1ship->appPtr)->money += p1Bank;
	}
	if (p2Lives > 0) {
		DrawPlayerMoneyMessage(((PlayerShipData *)p2ship->appPtr)->money, 2,80,p2Bank);
		((PlayerShipData *)p2ship->appPtr)->money += p2Bank;
	}
	
	if (p1insurance) {
		HandlePlayerInsurancePayment((PlayerShipData *)p1ship->appPtr, &p1insurance, 1,120);
	}
	if (p2insurance) {
		HandlePlayerInsurancePayment((PlayerShipData *)p2ship->appPtr, &p2insurance, 2,140);
	}
	
	l = TickCount();
	while (TickCount() - l < 240) ;

	RGBForeColor(&cBlack);
	PaintRect(&r);
}

void HandlePlayerInsurancePayment(PlayerShipData *data, PlayerShipData **insurPtr, int pnum, int vpos) {
	Str255 buffer;
	Str63  pnumStr, payStr, cancelStr, remainStr;
	Point p;
	int payment;

	GetIndString(payStr, rGeneralStrings, rInsurancePaymentIs);
	GetIndString(pnumStr, rGeneralStrings, rPlayerOne + pnum - 1);
	GetIndString(cancelStr, rGeneralStrings, rPolicyCancelled);
	GetIndString(remainStr, rGeneralStrings, rRemain);
	PtoCstr(pnumStr);
	PtoCstr(payStr);

	payment = CalculateShipPayment(*insurPtr);
	if (payment > data->money) {
		sprintf((char *)buffer,"%s %s %d ppp.  %#s", pnumStr, payStr, payment, cancelStr);
		CtoPstr((char *)buffer);
		MoveTo(gGameRect.right/2 - StringWidth(buffer)/2,gGameRect.bottom/2+vpos);
		sprintf((char *)buffer,"%s %s ",pnumStr, payStr);
		CtoPstr((char *)buffer);
		RGBForeColor(&cGreen);DrawString(buffer);
		RGBForeColor(&cCyan);DrawInt(payment);
		GetPen(&p);p.v-=12;p.h+=3;RGBForeColor(&cBlack);PlotOneMoney(&p);
		RGBForeColor(&cRed);
		DrawString("\p    ");
		DrawString(cancelStr);
		*insurPtr = NULL;
	} else {
		data->money -= payment;
		sprintf((char *)buffer,"%s %s %d ppp.  %d ppp %s", pnumStr, payStr, payment, data->money, remainStr);
		CtoPstr((char *)buffer);
		MoveTo(gGameRect.right/2 - StringWidth(buffer)/2,gGameRect.bottom/2+vpos);
		sprintf((char *)buffer,"%s %s ",pnumStr, payStr);
		CtoPstr((char *)buffer);
		RGBForeColor(&cGreen);DrawString(buffer);
		RGBForeColor(&cCyan);DrawInt(payment);
		GetPen(&p);p.v-=12;p.h+=3;RGBForeColor(&cBlack);PlotOneMoney(&p);
		sprintf((char *)buffer,"    .  ");
		CtoPstr((char *)buffer);
		RGBForeColor(&cGreen);DrawString(buffer);
		RGBForeColor(&cCyan);DrawInt(data->money);
		GetPen(&p);p.v-=12;RGBForeColor(&cBlack);PlotOneMoney(&p);
		sprintf((char *)buffer,"    %#s", remainStr);
		CtoPstr((char *)buffer);
		RGBForeColor(&cGreen);DrawString(buffer);
	}
	RGBForeColor(&cBlack);
}

DrawPlayerMoneyMessage(int new, int pnum, int vpos, int bank) {
	Str255 buffer;
	Str31  pnumStr, inBankStr;
	Point p;
	
	GetIndString(pnumStr, rGeneralStrings, rPlayerOne + pnum - 1);
	GetIndString(inBankStr, rGeneralStrings, rInBank);
	
	sprintf((char *)buffer,"%#s  %d + %d ppp %#s = %d ppp",pnumStr,new,bank, inBankStr, new+bank);
	CtoPstr((char *)buffer);
	MoveTo(gGameRect.right/2 - StringWidth(buffer)/2,gGameRect.bottom / 2 + vpos);

	RGBForeColor(&cGreen);
	DrawString(pnumStr);DrawChar(' ');
	RGBForeColor(&cCyan);
	DrawInt(new);
	GetPen(&p);p.v -= 12;p.h += 3;RGBForeColor(&cBlack);PlotOneMoney(&p);
	RGBForeColor(&cGreen);
	DrawString("\p      + ");
	RGBForeColor(&cCyan);
	DrawInt(bank);
	GetPen(&p);p.v -= 12;p.h += 3;RGBForeColor(&cBlack);PlotOneMoney(&p);
	RGBForeColor(&cGreen);
	DrawString("\p      ");
	DrawString(inBankStr);
	DrawString("\p = ");
	RGBForeColor(&cCyan);
	DrawInt(new+bank);
	GetPen(&p);p.v -= 12;p.h += 3;RGBForeColor(&cBlack);PlotOneMoney(&p);
}

void DisplayStatusMessage(StringPtr buffer) {
	long t;
	Rect r;
	
	RGBForeColor(&cBlack);
	r = gGameRect;
	r.top += scoreBarHeight+1;
	PaintRect(&r);
	RGBForeColor(&cGreen);
	MoveTo(gGameRect.right/2 - StringWidth(buffer)/2,gGameRect.bottom/2);
	DrawString(buffer);
	
	FadeUp(20);
	
	t=TickCount();
	while (TickCount()-t<50) ;
	RGBForeColor(&cBlack);
	PaintRect(&r);

	if (p1Lives) {
		RedrawPlayerShipInfo((PlayerShipData *)p1ship->appPtr);
		DrawPlayerShields((PlayerShipData *)p1ship->appPtr);
	}
	if (p2Lives) {
		RedrawPlayerShipInfo((PlayerShipData *)p2ship->appPtr);
		DrawPlayerShields((PlayerShipData *)p2ship->appPtr);
	}
}

void DisplayBeginLevelMessage() {
	Str255 	buffer;
	Str15	enteringStr, levelStr;
	int 	j;
	
	GetIndString(enteringStr, rGeneralStrings, rEntering);
	GetIndString(levelStr, rGeneralStrings, rLevel);
	
	sprintf((char *)buffer,"%#s %#s %d",enteringStr, levelStr, gLevel);
	CtoPstr((char *)buffer);
	FadeDown(20);

	if (p1Lives) {
		RedrawPlayerShipInfo((PlayerShipData *)p1ship->appPtr);
		DrawPlayerShields((PlayerShipData *)p1ship->appPtr);
	}
	if (p2Lives) {
		RedrawPlayerShipInfo((PlayerShipData *)p2ship->appPtr);
		DrawPlayerShields((PlayerShipData *)p2ship->appPtr);
	}
	DisplayStatusMessage(buffer);

}

void DrawScore(PlayerShipData *data) {
	Str31 buffer;
	RGBForeColor(&cBlack);

	if (p1Lives && data == (PlayerShipData *)p1ship->appPtr) {
		PaintRect(&p1scoreR);
		RGBForeColor(&cRed);
		MoveTo(p1scoreR.left+2,p1scoreR.bottom-1);

		if (gGameRect.right - gGameRect.left > 512) GetIndString(buffer, rGeneralStrings, rScore);
		else GetIndString(buffer, rGeneralStrings, rShortScore);
		
		DrawString(buffer);
		NumToString(data->score,buffer);
		hold1score = data->score;
		DrawString(buffer);
	} else if (p2Lives && data == (PlayerShipData *)p2ship->appPtr) {
		PaintRect(&p2scoreR);
		RGBForeColor(&cCyan);
		MoveTo(p2scoreR.left+2,p2scoreR.bottom-1);

		if (gGameRect.right - gGameRect.left > 512) GetIndString(buffer, rGeneralStrings, rScore);
		else GetIndString(buffer, rGeneralStrings, rShortScore);

		DrawString(buffer);
		NumToString(data->score,buffer);
		hold2score = data->score;
		DrawString(buffer);
	}
	RGBForeColor(&cBlack);
}

void DrawPlayerShields(PlayerShipData *data) {
	Rect 		*psr;
	long 		rectWidth;
	Rect 		r,tr,subR;
	Str31		buffer;
	int			i,j,k;
	RGBColor	c, shadow;
	
	if (p1Lives && data == (PlayerShipData *)p1ship->appPtr) psr = &p1ShieldR;
	else if (p2Lives && data == (PlayerShipData *)p2ship->appPtr) psr = &p2ShieldR;	
	else return;
	
	rectWidth = psr->right- psr->left;
	
	SetRect(&r,psr->left+shieldWordLength, psr->top+4,
			   psr->left+shieldWordLength + 
			   		(rectWidth-shieldWordLength) * (long)data->shields / data->shieldMax,
			   psr->bottom);
	if (r.right < psr->left + shieldWordLength) r.right = psr->left + shieldWordLength;
	SetRect(&tr,r.left,r.top,psr->right,r.bottom);

	RGBForeColor(&cMidGray);
	MoveTo(psr->left,psr->bottom-1);
	TextFace(0);

	GetIndString(buffer, rGeneralStrings, rShields);
	DrawString(buffer);
	TextFace(bold);
	RGBForeColor(&cDarkGray);
	FrameRect(&tr);
	InsetRect(&r,1,1);
	if (data->shields < data->shieldMax / 5) {
		c = cMidRed;
	} else if (data->shields < data->shieldMax * 2 / 5) {
		c = cMidYellow;
	} else c = cMidGreen;

	RGBForeColor(&c);
	PaintRect(&r);
	RGBForeColor(&cBlack);
	SetRect(&r,r.right,r.top,psr->right-1,r.bottom);
	if (r.right <= r.left) r.right = r.left;
	if (r.left <= psr->left + shieldWordLength) r.left = psr->left+1+shieldWordLength;
	PaintRect(&r);
}

void RedrawPlayerShipSpecial(PlayerShipData *data) {
	Point p;
	Rect *infoRect,r;
	
	
	if (p1Lives && data == (PlayerShipData *)p1ship->appPtr) {
		infoRect = &p1InfoRect;
	} else if (p2Lives && data == (PlayerShipData *)p2ship->appPtr) {
		infoRect = &p2InfoRect;
	} else {
		return;
	}

	data->redrawSpecial = FALSE;
	p.h = infoRect->left + 32; p.v = infoRect->top -2;
	SetRect(&r,p.h,p.v,p.h+16,p.v+16);
	RGBForeColor(&cBlack);
	PaintRect(&r);
	
	PlotSpecialGraphic(p, data);
}

void DrawPlayerShipRebuildingMessage(PlayerShipData *data) {
	Rect 		*infoRect;
	Boolean 	insured;
	Str31		buffer;
	
	if (data == (PlayerShipData *)p1ship->appPtr) {
		infoRect = &p1InfoRect;
		insured = (p1insurance!=NULL);
	} else if (gNPlayers== 2 && data == (PlayerShipData *)p2ship->appPtr) {
		infoRect = &p2InfoRect;
		insured = (p2insurance!=NULL);
	} else {
		return;
	}

	RGBForeColor(&cBlack);
	PaintRect(infoRect);
	MoveTo(infoRect->left, infoRect->top+12);
	RGBForeColor(&cRed);
	if (insured) GetIndString(buffer, rGeneralStrings, rRestoringShip);
	else GetIndString(buffer, rGeneralStrings, rBuildingShip);
	DrawString(buffer);
	RGBForeColor(&cBlack);
	
}

void RedrawPlayerShipInfo(PlayerShipData *data) {
	Point 		p;
	int 		lives;
	Rect 		*infoRect;
	Str31		buffer;

	data->redrawInfo = FALSE;

	if (data == (PlayerShipData *)p1ship->appPtr) {
		infoRect = &p1InfoRect;
		lives = p1Lives;
	} else if (gNPlayers== 2 && data == (PlayerShipData *)p2ship->appPtr) {
		infoRect = &p2InfoRect;
		lives = p2Lives;
	} else {
		return;
	}
	RGBForeColor(&cBlack);
	PaintRect(infoRect);
	if (lives < 1) {
		MoveTo(infoRect->left, infoRect->top+13);
		RGBForeColor(&cRed);
		GetIndString(buffer, rGeneralStrings, rGameOver);
		DrawString(buffer);
	} else {
		p.h = infoRect->left;
		p.v = infoRect->top-1;
		PlotOneMoney(&p);
		MoveTo(p.h+17,p.v + 14);
		RGBForeColor(&cGreen);
		TextFace(0);
		DrawInt(data->money);
		MoveTo(infoRect->left + 48, infoRect->top + 13);

		GetIndString(buffer, rGeneralStrings, rLives);
		DrawString(buffer);
		
		DrawInt(lives);
		TextFace(bold);
	}
	RGBForeColor(&cBlack);
	if (lives > 0) RedrawPlayerShipSpecial(data);
}

void SetupLevel(void) {
	int 		i,x,y, numRocks;
	GrafPtr 	oldPort;
	RGBColor 	c;
	Rect 		r;
	int 		searchDepth;

	if (GetPrefNewRules()) {
		if (gLevel > 20) numRocks = 6 + gLevel / 5;
		else if (gLevel > 9) numRocks = 4 + gLevel / 3;
		else numRocks = 2 + (gLevel+1) / 2;
	} else {
		numRocks = gLevel/2 + 3;
	}
	
	for (i=0;i<numRocks;i++) {
		switch (RangedRdm(1,4)) {
			case 1:	NewSprite(-2, RangedRdm(16,gSAT.offSizeH), 16, SetupBigAsteroid);break;
			case 2:	NewSprite(-2, 16, RangedRdm(0,gSAT.offSizeV-16), SetupBigAsteroid);break;
			case 3:	NewSprite(-2, RangedRdm(16,gSAT.offSizeH), gSAT.offSizeV-16, SetupBigAsteroid);break;
			case 4:	NewSprite(-2, gSAT.offSizeH-16, RangedRdm(16,gSAT.offSizeV-16), SetupBigAsteroid);break;
		}
	}
	searchDepth = GetSearchDepth();
	ConfigureSAT (FALSE, kVPositionSort, kKindCollision, searchDepth);

	GetPort(&oldPort);
	SetPort(gSAT.backScreen);
	SetRect(&r,0,0,gSAT.offSizeH,gSAT.offSizeV);
	PaintRect(&r);
	DrawStars();
	SetPort(oldPort);
	SATBackChanged(&r);
}

void ClearBackground() {
	GrafPtr oldPort;
	Rect r;

	GetPort(&oldPort);
	SetPort(gSAT.backScreen);
	SetRect(&r,0,0,gSAT.offSizeH,gSAT.offSizeV);
	PaintRect(&r);
	SetPort(oldPort);
	SATBackChanged(&r);
	RunSAT(PrefFastDraw());
}

void DrawStars() {
	RGBColor c;
	int x,y,i;

	c.red= c.green = c.blue = 0;
	for (i=0;i<60;i++) {
		c.red = RangedRdm(20000,65535);
		c.blue = RangedRdm(20000,65535);
		c.green = RangedRdm(20000,65535);
		x = RangedRdm(0,gSAT.offSizeH);
		y = RangedRdm(0,gSAT.offSizeV);
		SetCPixel(x,y,&c); 
	}
}

unsigned short RangedRdm( unsigned short min, unsigned short max )
{
	unsigned	qdRdm;	/* treat return value as 0-65536 */
	long	range, t;

	qdRdm = Random();
	range = max - min;
	t = (qdRdm * range) / 65536; 	/* now 0 <= t <= range */
	return( t+min );
}

void DecreasePlayerLives(PlayerShipData *data) {
	if (p1Lives && data == (PlayerShipData *)p1ship->appPtr) p1Lives--;
	else if (p1Lives && data == (PlayerShipData *)p2ship->appPtr) p2Lives--;
}

void IncreasePlayerLives(PlayerShipData *data) {
	if (p1Lives && data == (PlayerShipData *)p1ship->appPtr) p1Lives++;
	else if (p2Lives && data == (PlayerShipData *)p2ship->appPtr) p2Lives++;
}

void SetPlayerInsurance(PlayerShipData *data) {
	if (data == (PlayerShipData *)p1ship->appPtr) {
		p1insurance = SetUpShip(p1ShipType);
		SetInsurance(p1insurance, 1, data);
	} else if (data == (PlayerShipData *)p2ship->appPtr) {
		p2insurance = SetUpShip(p2ShipType);
		SetInsurance(p2insurance, 2, data);
	}
}

PlayerShipData *GetPlayerInsurance (PlayerShipData *data) {
	if (p1ship && data==(PlayerShipData *)p1ship->appPtr) {
		return p1insurance;
	} else if (p2ship && data==(PlayerShipData *)p2ship->appPtr) {
		return p2insurance;
	}
	return NULL;
}

void SetInsurance(PlayerShipData *insur, int pnum, PlayerShipData *data) {
	int i;

	LoadPlayerKeys(pnum, insur);
	insur->pnum = pnum;

	insur->shotSpeed = data->shotSpeed;
	insur->maxNumberShots = data->maxNumberShots;
	insur->safeShots 		= data->safeShots; 
	insur->shotsPerBurst 	= data->shotsPerBurst;
	insur->shotDelay 		= data->shotDelay;
	insur->scatterShots 	= data->scatterShots;
	insur->shotsInOneShot 	= data->shotsInOneShot;
	insur->shieldMax 		= data->shieldMax;
	insur->shields			= data->shieldMax;

	insur->thrustRate 		= data->thrustRate;
	insur->topSpeed 		= data->topSpeed;
	insur->retroRate		= data->retroRate;

	for (i=0;i<data->nSpecials;i++) {
		insur->specials[i] = data->specials[i];
	}
	insur->specialSelected = 0;
	insur->nSpecials = data->nSpecials;
}

int GetDifficulty() {
	int dif;
	dif = PrefDifficulty();
//	if (gNPlayers==2) dif++;
	return dif;
}

// returns new delay time
int HandlePeriodicAlienCheck() {
	int alienNum, newTime;

	if (gLevel == 1) return 10000;
	newTime = GetAlienTimeDelay();

	switch (PrefDifficulty()) {
		case kBeginnerDif:
			if (RangedRdm(1,4)!=3) return newTime;
			break;
		case kNormalDif:
			if (RangedRdm(1,4)==3) return newTime;
			break;
		case kExpertDif:
			if (RangedRdm(1,5)==4) return newTime;
			break;
		case kReallyExpertDif:				
			if (RangedRdm(1,6)==5) return newTime;
	}
	
	alienNum  = RangedRdm(0,gLevel+3);

	switch (PrefDifficulty()) {
		case kBeginnerDif:
			if (alienNum < 6 || alienNum % 5 < 3) NewSprite(-3,0,0, SetupSimpleSaucer);
			else if (alienNum % 6 < 4) if (!AreAnyLeeches()) NewSprite(-3,0,0, SetupLeechSaucer);									 
			else NewSprite(-3,0,0, SetupSpiderSaucer);
			break;
		case kNormalDif:
			if (alienNum < 5 || alienNum % 3 < 2) NewSprite(-3,0,0, SetupSimpleSaucer);
			else if (alienNum < 9 || alienNum % 2 == 0) if (!AreAnyLeeches()) NewSprite(-3,0,0, SetupLeechSaucer);									 
			else NewSprite(-3,0,0, SetupSpiderSaucer);
			break;
		case kExpertDif:
			if (alienNum < 4 || alienNum % 3 < 2) NewSprite(-3,0,0, SetupSimpleSaucer);
			else if (alienNum < 8 || alienNum % 2 == 0) if (!AreAnyLeeches()) NewSprite(-3,0,0, SetupLeechSaucer);									 
			else NewSprite(-3,0,0, SetupSpiderSaucer);
			break;
		case kReallyExpertDif:				
			if (alienNum % 3 == 0) NewSprite(-3,0,0, SetupSimpleSaucer);
			else if (alienNum % 3 == 1) if (!AreAnyLeeches()) NewSprite(-3,0,0, SetupLeechSaucer);									 
			else NewSprite(-3,0,0, SetupSpiderSaucer);
			break;

	}
	return newTime;
}

int GetAlienTimeDelay() {
	int periodicEvent;
	switch (PrefDifficulty()) {
		case kBeginnerDif:periodicEvent = RangedRdm(600,1800);break;
		case kNormalDif:periodicEvent = RangedRdm(400,1200);break;
		case kExpertDif:periodicEvent = RangedRdm(300,600);break;
		case kReallyExpertDif:periodicEvent = RangedRdm(200,400);break;				
	}				
	return periodicEvent;
}

int GetSearchDepth(void) {
	return ((gLevel / 5)+1)*32;
}

void CheckForCrystal(SpritePtr me) {
	int 			chance;
	SpritePtr		new;

	chance = 35 - gLevel/2;
	if (chance < 15) chance = 15;
	if (RangedRdm(0,100) < chance) {
	 	new = NewSpriteAfter(me, -2, me->position.h, me->position.v, SetupMoney);		
		GetAsteroidSpeed(&new->speed);
	}
}

void CheckForEnergy(SpritePtr me) {
	int 			chance;
	SpritePtr		new;

	chance = 30 - gLevel/2;
	if (chance < 7) chance = 7;
	if (RangedRdm(0,100) < chance) {
	 	new = NewSpriteAfter(me, -2, me->position.h, me->position.v, SetupCrystal);		
		GetAsteroidSpeed(&new->speed);
	}
}


void SuspendGame(void) {
	PlayerShipData *dataOne, *dataTwo;
	
	dataOne = dataTwo = NULL;
	if (p1Lives > 0 && !playerOneDead) dataOne = (PlayerShipData *)p1ship->appPtr;
	if (p2Lives > 0 && !playerTwoDead) dataTwo = (PlayerShipData *)p2ship->appPtr;

	if (dataTwo) {
		SetSuspendParameters(gLevel, hold1score, p1Lives, p1Bank, dataOne, p1insurance,
						  hold2score, p2Lives, p2Bank, dataTwo, p2insurance);
	} else {
		SetSuspendParameters(gLevel, hold1score, p1Lives, p1Bank, dataOne, p1insurance,
						  0, 0, 0, NULL, NULL);	
	}
	PauseSignalGameOver();
	gameWasEndedWithSuspend = TRUE;
	EnableItem(GetMHandle(rGameMenu),rmResume);
}
