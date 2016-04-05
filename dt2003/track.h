#ifndef _TRACK_H
#define _TRACK_H

#include "std.h"
#include "common.h"
#include "font.h"
#include <vector>
#include <list>

#define ITEMHT (FONTHT + 1)
#define TEXTCOL 0x000044
#define FRAMECOL 0x000088
#define ALTFRAMECOL 0x888888
#define BACKCOL 0xFFFFFF
#define BACKHILITECOL 0xE5E5E5
#define IPBACKCOL 0xEEEEFF
#define IPBACKHILITECOL 0xE0E0EE

#pragma warning( push )
#pragma warning( disable : 4661 )


//class CTrack; //forward decl. for crossref.
class CTracks;
class CPatterns;
class CPatternsDrawer;
class CDrawer;

class CTrackGroup {
public:
	char sName[10];
	CTrackGroup(char* AName) { strcpy(sName,AName); };
};
extern CTrackGroup null;

//base class for representing any track
class CBaseTrack {
public:
	char sName[10];
	void* pData; //must be TRACKLENGTH*sizeof(datatype) long!!

	int nPixelWidth;
	bool bReadonly;
	CBaseTrack(char* AName);
	virtual void drawitem(int n, int x, int y, CDrawer* drawer) = 0; //draws the nth item of the current pattern.
	virtual bool ishilite(int n) { return false; };

};

class CTrack : public CBaseTrack {
protected:
	CTracks* t;
public:
	
	CTrackGroup& pGroup;
	TrackTab eTab;

	CTrack(CTracks *ATracks, char* AName, CTrackGroup& AGroup, TrackTab ATab);
	//~CTrack();
	//virtual void draw(); //draws a whole column
	virtual void EnterValue(WPARAM wParam) = 0; //a 0-9a-f key was pressed.

};

//a 3-digit track accepting -0xFFF..0xFFF.
//templates for storage type!! digits en signed in constructor!
template <class T> 
class CTIntTrack : public CTrack {
protected:
	int nMax;
	int nMin;
	//int nOutChars;
	//int nWidth;
public:
	CTIntTrack(CTracks *ATracks, char* AName, CTrackGroup& AGroup, TrackTab ATab, int ADigits, bool ASigned);
	~CTIntTrack();
	T* pData;
	bool* pInterp;
	int nInterpStart;

	int nDigits;
	bool bSigned;
	int nNoteOff; //value that's actually the noteoff.

	virtual void drawitem(int n, int x, int y, CDrawer* drawer);
	virtual void EnterValue(WPARAM wParam); //a 0-9a-f key was pressed.
	virtual bool ishilite(int n) { return this->pInterp[this->t->ptns.getmempoint(n)]; };
};

//template CTIntTrack<short>;
template CTIntTrack<signed int>;
typedef CTIntTrack<signed int> CIntTrack;

class CPattern {
public:
	CPattern() {index=0; length=0x80;};
	int index;
	int length;
};

class CPatternDrawTrack : public CBaseTrack {
protected:
	
public:
	bool bEditing;
	CPatterns* p;
	std::list<CPattern>::iterator curptn;
	CPatternDrawTrack(char* AName, CPatterns *APatterns) : p(APatterns), CBaseTrack(AName) { bEditing=false; nPixelWidth = 3*FONTCHARWD+2; }
	virtual void drawitem(int n, int x, int y, CDrawer* drawer); //draws the nth item of the current pattern.
	virtual void EnterValue(WPARAM wParam); //a 0-9a-f key was pressed.
};

class CCounterTrack : public CBaseTrack {
public:
	CCounterTrack();
	virtual void drawitem(int n, int x, int y, CDrawer* drawer); //draws the value of n.
	virtual void EnterValue(WPARAM wParam) {}
};


class CDrawer{
protected:
	unsigned int *_pixels;
	Font *_font;
	HWND wnd;

	void drawtrack(CBaseTrack* tr, bool isMatt, int topitem, int botitem, int ystart, int &x, int pos);

public:
	int nXOffset;
	int nYOffset;
	int nBottom;
	bool bFocus;

	CDrawer(HWND AWnd, Font *AFont, unsigned int *APixels, int AXOffset = 0, int AYOffset = 0, int ABottom = WINHT);

	virtual void draw() = 0;
	inline void textout(char* text, int x, int y) { _font->textout(text,x,y); };
	void invalidate();
};

class CTracksDrawer : public CDrawer {
public:
	TrackTab eCurTab;

	CTracksDrawer(HWND AWnd, Font *AFont, unsigned int *APixels, int AXOffset = 0, int AYOffset = 0, int ABottom = WINHT);
	CTracks* t;
	void draw();

};
//TODO: add CPatternsDrawer that smartly uses some CTrack objects.
class CPatternsDrawer : public CDrawer {
public:

	CPatternsDrawer(HWND AWnd, Font *AFont, unsigned int *APixels, int AXOffset = 0, int AYOffset = 0, int ABottom = WINHT);
	CPatterns* p;
	void draw();
	CCounterTrack ct;
	CPatternDrawTrack pt;
};

class CPatterns : public std::list<CPattern> {
public:
	CPatterns(CPatternsDrawer& ADrawer) : drawer(ADrawer) { nPos=0;resize(1); pCurPtn=begin();/*nPtnCount=1;*/drawer.p=this; };
	void MovePos(int v); //safely move pos. (rel)
	void SetPos(int v); //safely set pos. (abs)
	void MoveCurPtn(bool up); //move cur pattern up or down.
	void InsertPtn(iterator it);
	void DeletePtn(iterator it);
	int nPos;
	//int nCurPtn;
	//int nPtnCount;
	iterator pCurPtn; 
	inline int getmempoint() { return pCurPtn->index*MAXPTNLEN+nPos; };
	inline int getmempoint(int i) { return pCurPtn->index*MAXPTNLEN+i; };
	CTracks* t;
	CPatternsDrawer& drawer;
};



class CTracks : public std::vector<CTrack*> {
protected:
	bool bInvalid;
public:
	CPatterns& ptns;
	CTracksDrawer& drawer; //must be freed separately!
	CCounterTrack ct;
	int nMatt; //index of currently selected track.
	int nTrackCount;

	bool bEditing;
	bool bModified;	

	CTracks(CTracksDrawer& ADrawer, CPatterns& APatterns);
	~CTracks();
	void invalidate();
	void MoveMatt(int v,CTrackGroup* g = NULL); //safely move matt.
	void EnterValue(WPARAM wParam); //another key was pressed.
};

#pragma warning( pop )

#endif //ndef _TRACK_H