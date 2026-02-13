#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AsteraxResources.h"
#include "main.fp"
#include "HighScores.h"
#include <GestaltEqu.h>
#include "preferences.h"
#include <Script.h>
#include "Strings.h"
#include <Packages.h>

#define rHighScoresPicID				151
#define rNewHighScoreDlog				201

extern Boolean gHasGestalt;

typedef struct {
	DateTimeRec when;
	Str31		machineTypeName;
	Point		screenSize;
	Boolean		twoPlayer;
	Boolean		doubleSpeed;
} GameRec;


typedef struct {
	Str31 who;
	long  theScore;
	int	  level;
	GameRec data;
} HighScoreRecord;


#define nScores 10
#define nDifficulty 4

HighScoreRecord scores[nDifficulty][nScores];
Boolean			markedScore[nScores];

short highscoreFileRefNum;

void LoadHighScores() {
	// if no highscore file exists, this routine must create one.
	short refNum;
	OSErr iErr;
	Str255 buffer;
	long size;
	int i;
	char *name = "High Scores";

	memset(scores,0,sizeof(HighScoreRecord)*nScores*nDifficulty);
	for (i=0;i<nScores;i++) markedScore[i] = FALSE;

	iErr = FSOpen("\pAsterax Scores",0,&refNum);
	size = sizeof(HighScoreRecord) * nScores * nDifficulty;
	if (iErr == fnfErr) {
		iErr = Create("\pAsterax Scores",0,rMyCreator,rScoreFileType);
		if (!iErr) {
			iErr = FSOpen("\pAsterax Scores",0,&refNum);
			if (iErr) HandleTerminalError(rFileErrors,rGenericOpenErr,name);
			iErr = SetEOF(refNum,size);
		}
		switch (iErr) {
			case 0:break;
			case dskFulErr:case dirFulErr:HandleTerminalError(rFileErrors,rDirFull,name);
			case ioErr:HandleTerminalError(rFileErrors,rIoErr,name);
			case vLckdErr:HandleTerminalError(rFileErrors,rLockedErr,name);
			case wPrErr:HandleTerminalError(rFileErrors,rPermErr,name);
			default:HandleTerminalError(rFileErrors,rGenericCreateErr,name);
		}
		iErr = FSWrite(refNum,&size,scores);
		// error return?
	} else if (iErr) {
		HandleTerminalError(rFileErrors,rGenericOpenErr,name);
	} else {
		iErr = FSRead(refNum,&size,scores);
		switch (iErr) {
			case 0:break;
			case eofErr:HandleTerminalError(rFileErrors,rObsoleteFileErr,name);
			case ioErr:HandleTerminalError(rFileErrors,rIoReadErr,name);
			default:HandleTerminalError(rFileErrors,rGenericReadErr,name);
		}
	}
	highscoreFileRefNum = refNum;
}

void DrawHighScores(int topMargin, Boolean full) {
	PicHandle title;
	Rect 	r;
	int 	i;
	Str255 	buffer;
	Str15	lvStr, playerStr;
	Str63	dateStr, timeStr;
	int		currentDiff;
	unsigned long secs;

	title = GetPicture(rHighScoresPicID);

	r = (*title)->picFrame;
	OffsetRect(&r,5,topMargin + 5);
	DrawPicture(title,&r);
	MoveTo(r.left,r.bottom);

	RGBFore(65535,65535,65535);
	LineTo(gGameRect.right/2-20,r.bottom);
	TextFont(geneva);
	TextSize(9);
	TextFace(bold);

	currentDiff = PrefDifficulty();
	if (currentDiff == kReallyExpertDif) return;

	if (gGameRect.right - gGameRect.left > 512) {
		GetIndString(lvStr, rGeneralStrings, rLevel);
	} else {
		GetIndString(lvStr, rGeneralStrings, rShortLevel);	
	}

	for (i=0;i<nScores;i++) {
		RGBFore(65535,65535,65535);
		if (markedScore[i]) RGBFore(0,65535,65535);
		if (!full) {
			MoveTo(r.left+2,r.bottom + 16 * (i+1));
			sprintf((char *)buffer,"%d. ",i+1);
			CtoPstr((char *)buffer);
			DrawString(buffer);
			if (scores[currentDiff][i].who[0]) {
				BlockMove(scores[currentDiff][i].who, buffer, 32);
				TruncString(120,buffer,smTruncEnd);				
				DrawString(buffer);
				MoveTo(r.left+142,r.bottom + 16 * (i+1));
				sprintf((char *)buffer,"%ld",scores[currentDiff][i].theScore);
				CtoPstr((char *)buffer);
				DrawString(buffer);		
	
				MoveTo(r.left+197,r.bottom + 16 * (i+1));
				sprintf((char *)buffer,"%#s %d",lvStr, scores[currentDiff][i].level);

				CtoPstr((char *)buffer);
				DrawString(buffer);	
				if (scores[currentDiff][i].data.doubleSpeed) {
					DrawString("\p (x2)");
				}
			}	
		} else {
			MoveTo(r.left+2,r.bottom + 32 * (i+1) - 16);
			sprintf((char *)buffer,"%d. ",i+1);
			CtoPstr((char *)buffer);
			DrawString(buffer);
			if (scores[currentDiff][i].who[0]) {
				DateTimeRec when;
				BlockMove(scores[currentDiff][i].who, buffer, 32);
				TruncString(120,buffer,smTruncEnd);				
				DrawString(buffer);

				MoveTo(r.left+142,r.bottom + 32 * (i+1) -16);
				sprintf((char *)buffer,"%ld",scores[currentDiff][i].theScore);
				CtoPstr((char *)buffer);
				DrawString(buffer);		
	
				MoveTo(r.left+197,r.bottom + 32 * (i+1)- 16);
				sprintf((char *)buffer,"%#s %d",lvStr, scores[currentDiff][i].level);

				CtoPstr((char *)buffer);
				DrawString(buffer);		

				if (scores[currentDiff][i].data.doubleSpeed) {
					DrawString("\p (x2)");
				}

				TextFace(0);

				RGBFore(32767,32767,65535);				
				MoveTo(r.left+2,r.bottom + 32 * (i+1));
				PtoCstr(scores[currentDiff][i].data.machineTypeName);
				when = scores[currentDiff][i].data.when;

				Date2Secs(&when, &secs);
				IUDateString(secs,0,dateStr);
				IUTimeString(secs,FALSE,timeStr);

				GetIndString(playerStr, rGeneralStrings, rPlayer);
				sprintf((char *)buffer," %#s %#s  %dx%d  %s  %d %#s",dateStr, timeStr,
					scores[currentDiff][i].data.screenSize.h,
					scores[currentDiff][i].data.screenSize.v,
					scores[currentDiff][i].data.machineTypeName,
					(scores[currentDiff][i].data.twoPlayer==TRUE)?2:1,
					playerStr);
				CtoPstr((char *)scores[currentDiff][i].data.machineTypeName);
				CtoPstr((char *)buffer);
				DrawString(buffer);				
				TextFace(bold);
				RGBFore(65535,65535,65535);
			}			
		}
	} 
}

void RegisterHighScore(long theScore,int level,Boolean twoPlayer, int playerNum) {
	int 			i,j;
	GrafPtr 		oldPort;
	DialogPtr 		theD;
	short 			itemHit;
	Handle 			theHandle;
	Rect 			theRect;
	Str255 			buffer;
	Str31			pnumStr;
	long 			size;
	OSErr			myErr;
	long			myFeature;	// will receive Gestalt return value
	int				currentDiff;

	currentDiff = PrefDifficulty();
	if (currentDiff == kReallyExpertDif) return;
	size = sizeof(HighScoreRecord) * nScores * nDifficulty;
	GetPort(&oldPort);
	for (i=0;i<nScores;i++) markedScore[i] = FALSE;

	for (i=0;i<nScores;i++) {
		if (theScore > scores[currentDiff][i].theScore) {	
			theD = GetNewDialog(rNewHighScoreDlog,0L,(WindowPtr)-1L);
			SetPort(theD);
			RGBFore(65535,65535,65535);
			
			SelIText(theD,4,0,32767);			
			if (twoPlayer) {
				GetIndString(pnumStr, rGeneralStrings, rPlayerOne + playerNum - 1);
				if (playerNum == 1) ParamText("\p, ",pnumStr,0L,0L);
				else ParamText("\p, ",pnumStr,0L,0L);
			} else {
				ParamText("\p",0L,0L,0L);			
			}
			
			FlushEvents(keyDownMask,0);
			
			do {
				ModalDialog(0L,&itemHit);
			} while (itemHit != 1);
	
			GetDItem(theD,4,&itemHit,&theHandle,&theRect);
			GetIText(theHandle,buffer);
			if (buffer[0] > 31) buffer[0] = 31;
			
			DisposDialog(theD);
			SetPort(oldPort);
			for (j=nScores-1;j>i;j--) {
				memcpy(&scores[currentDiff][j],
						&scores[currentDiff][j-1],sizeof(HighScoreRecord));
				markedScore[j] = markedScore[j-1];
			}
			markedScore[i] = TRUE;
			memcpy(&scores[currentDiff][i].who,buffer,32);
			scores[currentDiff][i].theScore = theScore;
			scores[currentDiff][i].level = level;
			
			scores[currentDiff][i].data.screenSize.h = 
					gGameRect.right - gGameRect.left;
			scores[currentDiff][i].data.screenSize.v = 
					gGameRect.bottom - gGameRect.top;
		
			scores[currentDiff][i].data.twoPlayer = twoPlayer;
			scores[currentDiff][i].data.doubleSpeed = (GetPrefSpeed()==1);
			GetTime(&scores[currentDiff][i].data.when);

			if (gHasGestalt) 	{ 
				myErr = Gestalt(gestaltMachineType, &myFeature);
				GetIndString(scores[currentDiff][i].data.machineTypeName,
												kMachineNameStrID,myFeature);
				if (!strncmp((char *)&scores[currentDiff][i].data.machineTypeName[1],"Macintosh ",10)) {
					memmove(&scores[currentDiff][i].data.machineTypeName[1],
							&scores[currentDiff][i].data.machineTypeName[11],
							scores[currentDiff][i].data.machineTypeName[0]-10);
					scores[currentDiff][i].data.machineTypeName[0]-=10;				
				}
			} else {
				GetIndString(scores[currentDiff][i].data.machineTypeName,rGeneralStrings, rNotAvailable);				
			}
			myErr = SetFPos(highscoreFileRefNum,1,0);
			myErr = FSWrite(highscoreFileRefNum,&size,scores);
			break;
		}
	}
}

void CloseHighScoreFile() {
	OSErr iErr;
	iErr = FSClose(highscoreFileRefNum);
}

void ResetHighScoreMarks() {
	int i;
	for (i=0;i<nScores;i++) {
		markedScore[i] = FALSE;
	}
}