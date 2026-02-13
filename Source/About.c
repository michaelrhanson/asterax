#include "SAT.h"
#include "BuyableItems.h"
#include "globals.h"
#include "MainResources.h"
#include "AsteraxResources.h"
#include "About.h"
#include "Strings.h"

#define rArvandorLogoPict	600
#define rMikeCreditPict		601
#define rAnneCreditPict 	602
#define rIngemarCreditPict	603

#define rHazardStringList	600
#define rBigRockText		1
#define rMedRockText		2
#define rSmallRockText		3
#define rEnemy1Text1			4
#define rEnemy1Text2			5
#define rEnemy2Text1			6
#define rEnemy2Text2		    7
#define rEnemy3Text1			8
#define rEnemy3Text2			9

#define kSplashOffset			150

#define kSplashScreen			9


#define rProductionInfo 	610


void DrawNextButton(PicHandle pic, Rect *mainNextRect);
void DrawDoneButton(PicHandle pic, Rect *mainDoneRect);
void RGBFore(unsigned int red,unsigned int green,unsigned int blue);
void DrawCenteredString(StringPtr str, Point p);
void DrawPictureAt(PicHandle pic, Point p);


void DrawRsrcListString(int rsrcList, int index);
void DrawRsrcListString_Centered(int rsrcList, int index, Point p);
void DrawStringInRect_Centered(StringPtr str, Point p);

void DrawAbout(int screenNum, PicHandle nextPic, PicHandle donePic, 
											Rect *nextRect, Rect *doneRect) {
	RGBColor 		c;
	Rect 			r,pr;
	PicHandle 		thePic;
	Handle 			theText;
	char 			*text;
	Point			p;
	StringPtr		str;
	short			id;
	ResType			type;
	Str63			name;
	Str255			buffer;

	RGBFore(32767,0,0);
	
	r=gGameRect;

	InsetRect(&r,36,36 );
	DrawFuzzyRoundRect(&r,c);
	RGBFore(0,0,0);
		
	thePic = GetPicture(rSmallLogo);
	SetRect(&pr,r.left + 10,r.top + 3,r.left+10+(*thePic)->picFrame.right,r.top+3+(*thePic)->picFrame.bottom);
	DrawPicture(thePic,&pr);

	theText = GetResource('TEXT',rAboutText - 1 + screenNum);
	GetResInfo(theText,&id,&type,name);

	TextSize(16);	
	TextFace(italic | bold);
	MoveTo(pr.right+20,pr.bottom-2);
	RGBFore(65535,0,0);
	DrawString(name);

	TextFace(bold);
	TextSize(9);

	RGBFore(0,65535,65535);

	HLock(theText);
	text = *(char **)theText;
	InsetRect(&r,70,40);

	if (screenNum != kSplashScreen) {
		DrawCStringInRect(text,&r);
		HUnlock(theText);
	}

	switch (screenNum) {
		case 1:		
			p.h = r.left + (r.right-r.left)/4-16;
			p.v = r.bottom - 90;
			RGBFore(0,0,0);
			PlotOneMoney(&p);
			RGBFore(0,65535,65535);
			GetIndString(buffer, rGeneralStrings, rEmeraldium);
			DrawCenteredString(buffer,p);
			p.h = r.left + (r.right-r.left)*3/4-16;
			RGBFore(0,0,0);
			PlotOneEnergy(&p);
			RGBFore(0,65535,65535);

			GetIndString(buffer, rGeneralStrings, rShieldEnergy);
			DrawCenteredString(buffer,p);
			break;
		case 2: // ship info
			if (gGameRect.right - gGameRect.left > 640) p.v = r.top + 40;
			else p.v = r.top + 50;
			p.h = r.left + (r.right-r.left)/4-16;
			DrawShipAndName(0,&p);
			p.h = r.left + (r.right-r.left)/2-16;
			DrawShipAndName(1,&p);
			p.h = r.left + (r.right-r.left)*3/4-16;
			DrawShipAndName(2,&p);
			break;
		case 3:case 4:case 5:case 6:case 7: // upgrade options
			r.top += 30;
			PlotAllItemsAndDescriptions(screenNum-3,&r);
			break;
		case 8: // hazards
			p.v = r.top + 26;
			p.h = r.left + 32;
			
			RGBFore(0,0,0);
			DrawOneBigRock(p);
			RGBFore(65535,65535,0);
			p.v += 34;
			p.h += 16;
			DrawRsrcListString_Centered(rHazardStringList, rBigRockText, p);
			p.v -= 34;
			p.h += 112;
			RGBFore(0,0,0);
			DrawOneMediumRock(p);

			p.v += 34;
			p.h += 16;
			RGBFore(65535,65535,0);
			DrawRsrcListString_Centered(rHazardStringList, rMedRockText, p);
			p.h += 112;
			p.v -= 34;
			RGBFore(0,0,0);
			DrawOneSmallRock(p);

			p.v += 34;
			p.h += 16;
			RGBFore(65535,65535,0);
			DrawRsrcListString_Centered(rHazardStringList, rSmallRockText, p);

			p.v += 66;
			p.h = r.left + 32;

			RGBFore(0,0,0);
			DrawOneSimpleSaucer(p);
			RGBFore(65535,65535,0);
			MoveTo(p.h+40, p.v+16);
			DrawRsrcListString(rHazardStringList, rEnemy1Text1);
			MoveTo(p.h+50, p.v+30);
			DrawRsrcListString(rHazardStringList, rEnemy1Text2);

			p.v += 38;
			RGBFore(0,0,0);
			DrawOneLeech(p);
			RGBFore(65535,65535,0);
			MoveTo(p.h+40, p.v+16);
			DrawRsrcListString(rHazardStringList, rEnemy2Text1);
			MoveTo(p.h+50, p.v+30);
			DrawRsrcListString(rHazardStringList, rEnemy2Text2);

			p.v += 38;
			RGBFore(0,0,0);
			DrawOneSpiderSaucer(p);
			RGBFore(65535,65535,0);
			MoveTo(p.h+40, p.v+16);
			DrawRsrcListString(rHazardStringList, rEnemy3Text1);
			MoveTo(p.h+50, p.v+30);
			DrawRsrcListString(rHazardStringList, rEnemy3Text2);
			RGBFore(0,0,0);
			break;
		case kSplashScreen:
			RGBFore(65535,0,0);

			p.h = r.left;
			p.v = r.top;
			MoveTo(p.h, p.v);
			DrawRsrcListString(rProductionInfo, 1);
			p.v += 5;
			thePic = GetPicture(rMikeCreditPict);
			DrawPictureAt(thePic,p);

			p.v +=30;
			thePic = GetPicture(rAnneCreditPict);
			DrawPictureAt(thePic,p);

			p.v +=30;
			thePic = GetPicture(rIngemarCreditPict);
			DrawPictureAt(thePic,p);

			p.v += 40;
			MoveTo(p.h, p.v);
			DrawRsrcListString(rProductionInfo, 2);

			p.v += 30;
			thePic = GetPicture(701);
			DrawPictureAt(thePic,p);
			p.v -= 20;
			p.h += (*thePic)->picFrame.right + 10;
			thePic = GetPicture(700);
			DrawPictureAt(thePic,p);

			p.v += (*thePic)->picFrame.bottom + 15;
			r.top = p.v;
			RGBFore(0,65535,0);
			DrawCStringInRect(text,&r);
			HUnlock(theText);
			break;
		case 10:// thanks
			break;


	}
	DrawNextButton(nextPic, nextRect);
	DrawDoneButton(donePic, doneRect);
}

void DrawNextButton(PicHandle pic, Rect *mainNextRect) {
	Rect r;
	SetRect(&r,gGameRect.right - ((*pic)->picFrame.right),
				gGameRect.bottom - ((*pic)->picFrame.bottom), gGameRect.right,gGameRect.bottom);

	OffsetRect(&r,-160,-50);
	DrawPicture(pic,&r);
	*mainNextRect = r;
}

void DrawDoneButton(PicHandle pic, Rect *mainDoneRect) {
	Rect r;
	SetRect(&r,gGameRect.right - ((*pic)->picFrame.right),
				gGameRect.bottom - ((*pic)->picFrame.bottom),
				gGameRect.right, gGameRect.bottom);

	OffsetRect(&r,-60,-50);
	DrawPicture(pic,&r);
	*mainDoneRect = r;
}

void DrawCenteredString(StringPtr str, Point p) {
	MoveTo(p.h + 16 - StringWidth(str)/2,p.v+ 42);
	DrawString(str);
}

void DrawRsrcListString(int rsrcList, int index) {
	Str255 buffer;
	GetIndString(buffer,rsrcList,index);
	DrawString(buffer);
}

void DrawRsrcListString_Centered(int rsrcList, int index, Point p) {
	Str255 buffer;
	
	GetIndString(buffer,rsrcList,index);
	DrawStringInRect_Centered(buffer, p);
}

void DrawStringInRect_Centered(StringPtr buffer, Point p) {
	Rect r;
	int oldLength, i;

	oldLength = buffer[0];
	for (i=1;i<oldLength && buffer[i] != '\r';i++) ;
	buffer[0] = i;
	SetRect(&r,p.h - StringWidth(buffer)/2,p.v,p.h+7000,p.v+7000);
	buffer[0] = oldLength;
	PtoCstr(buffer);
	DrawCStringInRect((char *)buffer,&r);
	CtoPstr((char *)buffer);
}

void DrawPictureAt(PicHandle pic, Point p) {
	Rect pr, t;
	int width, height;

	pr = (*pic)->picFrame;
	width = pr.right - pr.left;
	height = pr.bottom - pr.top;
	
	SetRect(&t, p.h, p.v, p.h + width, p.v + height);
	DrawPicture(pic,&t);
}