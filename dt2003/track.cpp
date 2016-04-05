#include "track.h"

CDrawer::CDrawer(HWND AWnd, Font *AFont, unsigned int *APixels, int AXOffset, int AYOffset, int ABottom) {
	wnd = AWnd;
	_pixels = APixels;
	_font = AFont;
	nXOffset = AXOffset;
	nYOffset = AYOffset;
	nBottom = ABottom;
}


void CDrawer::invalidate() {
	InvalidateRect(wnd,NULL,false); //add stuff to update region; sends WM_PAINT when needed.
}


#define SETHILITE \
		if(tr->ishilite(i)) { \
			box(x,y,x+tr->nPixelWidth,y+ITEMHT-3,IPBACKCOL,_pixels); \
			backhilitecol = IPBACKHILITECOL; \
			backcol = IPBACKCOL; \
		} else { \
			backhilitecol = BACKHILITECOL; \
			backcol = BACKCOL; \
		}



//draws one track! magic!
void CDrawer::drawtrack(CBaseTrack* tr, bool isMatt, int topitem, int botitem, int ystart, int &x, int pos) {
	int y=ystart;
	int i;
	unsigned int bkc;

	_font->colour = TEXTCOL;

	//draw stuff before pos
	unsigned int backhilitecol, backcol;
	backhilitecol = BACKHILITECOL; 
	backcol = BACKCOL; 
	for(i = topitem; i < pos; i++) {
		//SETHILITE
		if(!(i%4)) box(x,y,x+tr->nPixelWidth,y+ITEMHT-3,backhilitecol,_pixels);
		if(tr->ishilite(i)) box(x,y-1,x+1,y+ITEMHT-1,0xFF0000,_pixels);
		tr->drawitem(i,x+2,y,this);
		y+=ITEMHT;
	}

	//draw stuff at pos
	if(isMatt) {
		//SETHILITE
		bkc = (pos%4) ? backcol : backhilitecol;
		box(x,y,x+tr->nPixelWidth,y+ITEMHT-3,bkc,_pixels);
		_font->colour = TEXTCOL;
	} else {
		_font->colour = BACKCOL;
	}
		if(tr->ishilite(i)) box(x,y-1,x+1,y+ITEMHT-1,0xFF0000,_pixels);
		tr->drawitem(i,x+2,y,this);
		y+=ITEMHT;

	//draw stuff after pos
	_font->colour = TEXTCOL;
	for(i = pos + 1; i < botitem; i++) {
		//SETHILITE
		if(!(i%4)) box(x,y,x+tr->nPixelWidth,y+ITEMHT-3,backhilitecol,_pixels);
		if(tr->ishilite(i)) box(x,y-1,x+1,y+ITEMHT-1,0xFF0000,_pixels);
		tr->drawitem(i,x+2,y,this);
		y+=ITEMHT;
	}
	
	//draw title
	_font->textout(tr->sName,x,nYOffset+ITEMHT-2);
	

	x+=tr->nPixelWidth;
	//draw frame
	box(x-1,nYOffset+ITEMHT-3,x,nBottom,FRAMECOL,_pixels);
}


CTracksDrawer::CTracksDrawer(HWND AWnd, Font *AFont, unsigned int *APixels, int AXOffset, int AYOffset, int ABottom) : CDrawer(AWnd,AFont,APixels,AXOffset,AYOffset,ABottom) {
}

void CTracksDrawer::draw() {
	//pre: nPtnStart <= nPos <= nPtnEnd.
	int x=nXOffset;
	int yoffset = nYOffset+(ITEMHT-2)*2; //don't display 2 upper rows cause of title.
	int ystart=yoffset;
	int j;
	int ptnlen = t->ptns.pCurPtn->length;

	//calculate item indices to draw and where to draw them.
	int count = (nBottom - yoffset) / ITEMHT - 1; //minus one for the safety margin
	int topitem = t->ptns.nPos - count/2;
	int botitem = topitem + count;
	if(topitem < 0) { //don't draw previous pattern
		ystart -= topitem * ITEMHT; //so start lower on the screen
		topitem = 0; //and draw the first item of the ptn there.
	}
	if(botitem > ptnlen) { //don't draw next pattern
		botitem = ptnlen; //and stop at the last item of the ptn.
	}

	//draw middle bar
	{
		int rightend = nXOffset+t->ct.nPixelWidth;
		for(j=0;j<t->nTrackCount;j++) if((*t)[j]->eTab == eCurTab) rightend += (*t)[j]->nPixelWidth;
		int ybar = yoffset+((count/2)*ITEMHT)-1;
		unsigned int col = (bFocus) ? FRAMECOL : ALTFRAMECOL;
		box(nXOffset,ybar,rightend,ybar+ITEMHT-2,col,_pixels);
	}

	//draw every track. (and some more frameworks)
	if(x>0) box(x-1,yoffset,x,nBottom,FRAMECOL,_pixels);

	//draw group titles
	{
		int b = 0;
		int e = t->ct.nPixelWidth; //(*t)[0]->nPixelWidth;
		for(j=0;j<t->nTrackCount-1;j++) if((*t)[j]->eTab == eCurTab) {
			e += (*t)[j]->nPixelWidth;
			if(&(*t)[j]->pGroup == &(*t)[j+1]->pGroup) {
				
			} else {
				_font->textout((*t)[j]->pGroup.sName,(e + b - strlen((*t)[j]->pGroup.sName)*FONTCHARWD)/2,nYOffset);
				b = e;
				//e = e + (*t)[j+1]->nPixelWidth;
			}
		}
		e += (*t)[j]->nPixelWidth;
		_font->textout((*t)[j]->pGroup.sName,(e + b - (strlen((*t)[j]->pGroup.sName)*FONTCHARWD))/2,nYOffset);
	}

	//draw actual data
	drawtrack(&t->ct,false,topitem,botitem,ystart,x, t->ptns.nPos);
	for(j=0;j<t->nTrackCount;j++) {
		if((*t)[j]->eTab == eCurTab) drawtrack((*t)[j],(j==t->nMatt),topitem,botitem,ystart,x, t->ptns.nPos);
	}
}

//---- CPatternsDrawer ----//


CPatternsDrawer::CPatternsDrawer(HWND AWnd, Font *AFont, unsigned int *APixels, int AXOffset, int AYOffset, int ABottom) : CDrawer(AWnd,AFont,APixels,AXOffset,AYOffset,ABottom), ct(), pt("",NULL) {
	bFocus = false;
}

void CPatternsDrawer::draw() {
	if(bFocus) {

		
		int x=nXOffset;
		int yoffset = nYOffset; //don't display 2 upper rows cause of title.
		int ystart=yoffset;
		int len = p->size();
		int pos = p->pCurPtn->index;

		//calculate item indices to draw and where to draw them.
		int count = (nBottom - yoffset) / ITEMHT - 1; //minus one for the safety margin
		int topitem = pos - count/2;
		int botitem = topitem + count;
		if(topitem < 0) { //don't draw previous pattern
			ystart -= topitem * ITEMHT; //so start lower on the screen
			topitem = 0; //and draw the first item of the ptn there.
		}
		if(botitem > len) { //don't draw next pattern
			botitem = len; //and stop at the last item of the ptn.
		}

		//draw bg and middle bar
		{
			int rightend = nXOffset + pt.nPixelWidth + ct.nPixelWidth;
			int ybar = yoffset+((count/2)*ITEMHT)-1;
			box(nXOffset,nYOffset,rightend,nBottom,BACKCOL,_pixels);
			box(nXOffset,ybar,rightend,ybar+ITEMHT-2,FRAMECOL,_pixels);
		}

		//TODO: draw box
		if(x>0) box(x-1,yoffset,x,nBottom,FRAMECOL,_pixels);


		//draw actual data
		pt.curptn = p->begin();
		drawtrack(&ct,false,topitem,botitem,ystart,x, pos);
		drawtrack(&pt,true,topitem,botitem,ystart,x, pos);
		textout("ptns",nXOffset+5,nYOffset);
		box(nXOffset,nBottom-1,x,nBottom,FRAMECOL,_pixels);
		box(nXOffset,nYOffset,x,nYOffset+1,FRAMECOL,_pixels);
		box(x-1,nYOffset,x,nYOffset+ITEMHT,FRAMECOL,_pixels);
	}
}


//---- CTracks ----//
CTracks::CTracks(CTracksDrawer& ADrawer, CPatterns& APatterns) : drawer(ADrawer), ptns(APatterns) {
	//drawer = ADrawer;
	//ptns = APatterns;
	drawer.t = this;
	ptns.t = this;
	resize(TRACKCOUNT,NULL);
	nTrackCount = 0;
	nMatt = 0; //watch out! there are no tracks yet!
	bEditing = false;
	invalidate();
}

//clean up pointers.
CTracks::~CTracks() {
	for(iterator i=begin();i!=end();i++) if(*i) {
		delete *i;
		//*i = NULL;
	}
}

void CTracks::invalidate() {
	//if(drawer) 
	drawer.invalidate();
}

void CPatterns::SetPos(int v) { //safely set pos.
	nPos = v;
	if(nPos>=pCurPtn->length) nPos = 0;
	if(nPos<0) nPos = pCurPtn->length - 1;
	t->bEditing = false;
	t->invalidate();
}

void CPatterns::MovePos(int v) { //safely move pos.
	SetPos(nPos+v);
}
void CPatterns::MoveCurPtn(bool up) {
	if(up) {
		if(pCurPtn==begin()) 
			pCurPtn = end();
		pCurPtn--;
	} else {
		pCurPtn++;
		if(pCurPtn==end()) pCurPtn = begin();
	}
	drawer.pt.bEditing = false;
	t->invalidate();
}

void CPatterns::InsertPtn(iterator it) {
	CPattern z;
	z.index = it->index;
	insert(it,z);
	for(;it!=end();it++) it->index++;
	t->invalidate();
}

void CPatterns::DeletePtn(iterator it) {
	if(size()>1) {
		iterator i = it;
		i++;
		if(it==pCurPtn) {
			pCurPtn = i;
			if(pCurPtn==end()) --(--pCurPtn);
		}		
		erase(it);
		for(;i!=end();i++) i->index--;
		t->invalidate();
	}
}

void CTracks::MoveMatt(int v, CTrackGroup* g) { //safely set matt.
	int c = nTrackCount;
	CTrackGroup* cg;
	do {
		nMatt = nMatt+v;
		if(nMatt>=nTrackCount) nMatt = 0;
		if(nMatt<0) nMatt = nTrackCount - 1;
		cg = &dit[nMatt]->pGroup;
		c--;
	} while ((((g!=NULL)&&(g==cg)) || (dit[nMatt]->eTab!=drawer.eCurTab)) && (c));
	bEditing = false;
	invalidate();
}

void CTracks::EnterValue(WPARAM wParam) {
	dit[nMatt]->EnterValue(wParam);
	invalidate();
}

//---- CTrack ----//
CBaseTrack::CBaseTrack(char *AName) {
	strcpy(sName,AName);
}

CTrack::CTrack(CTracks *ATracks, char* AName, CTrackGroup& AGroup, TrackTab ATab) : CBaseTrack(AName), t(ATracks), pGroup(AGroup), eTab(ATab) {
	//t = ATracks;
	if(t) {
		(*t)[t->nTrackCount] = this;
		t->nTrackCount++;
	}
	bReadonly = false;
}

void CPatternDrawTrack::drawitem(int n, int x, int y, CDrawer* drawer) { 
	if(curptn->index != n) {
		curptn = p->begin();
		while(curptn->index != n) curptn++;
	}
	char out[3]; //2 chars and a \0.
	wsprintf(out,"%02x",curptn->length);
	drawer->textout(out,x,y);
	curptn++;
}

void CPatternDrawTrack::EnterValue(WPARAM wParam) {
	unsigned char c = (char)wParam;
	int a;
	if(((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'F'))) {
		int v = c - '0';
		if(v>9) v = c - 'A' + 10;//v now contains the value that was pressed, 0-f
		a = p->pCurPtn->length;
		if(!bEditing) {
			bEditing = true;
			a = 0;
		}
		a = (a<<4) + v;
		if((a<=0x100) && (a>=0)) p->pCurPtn->length = a;
		if((a>(0x100 >> 4)) || (a<(0 >> 4))) bEditing = false; //we entered enough digits
	}
	p->t->invalidate();
}

template <class T>
CTIntTrack<T>::CTIntTrack(CTracks *ATracks, char* AName, CTrackGroup &AGroup, TrackTab ATab, int ADigits, bool ASigned) : CTrack(ATracks,AName,AGroup,ATab) {
	//cellsize = sizeof(T);
	nDigits = ADigits;
	bSigned = ASigned;

	int nOutChars = nDigits+(bSigned?1:0);
	nPixelWidth = nOutChars*FONTCHARWD + 3;
	//nOutChars++; //make room for \0.
	//nNoteOff = (1 << (nDigits*4))+1; //i.e. 0x1001 for 3 digits.
	
	//nMax = 0x10; for(int i=1;i<nDigits;i++) nMax*=0x10;
	nMax = (1 << (nDigits*4)) - 1;
	nMin = (bSigned)?-nMax:0;
	nNoteOff = nMax + 1;
	pData = new T[TRACKLENGTH];
	pInterp = new bool[TRACKLENGTH];
	nInterpStart = 0;
	for(int i=0;i<TRACKLENGTH;i++) pData[i] = nNoteOff;
	for(int i=0;i<TRACKLENGTH;i++) pInterp[i] = false;
}

template <class T>
CTIntTrack<T>::~CTIntTrack() {
	delete[] pData;
	delete[] pInterp;
}

template <class T>
void CTIntTrack<T>::drawitem(int n, int x, int y, CDrawer* drawer) {
	T v = pData[t->ptns.getmempoint(n)];
	char buf[9];// = new char[nOutChars]; //TODO precalc and store privately.
	char noteoff[9];
	noteoff[0] = ' '; for(int i=nDigits;i>0;i--) noteoff[i] = '-';
	noteoff[nDigits+1] = '\0';
	char* out;
	if(v==nNoteOff) { //0x1001 means no note, put " ---".
		out = noteoff;
	} else {
		out = buf;
		if(v>nMax) v=nMax;
		if(v<nMin) v=nMin;
		char fmt[10];
		wsprintf(fmt,"%%c%%0%dx",nDigits);
		wsprintf(out,fmt,((v<0)?'-':' '),abs(v));
	} 
	//TODO: draw out.
	drawer->textout(out+((bSigned)?0:1),x,y);
}

template <class T>
void CTIntTrack<T>::EnterValue(WPARAM wParam) {
	int curidx = t->ptns.getmempoint();
	unsigned char c = (char)wParam;
	T a;
	switch (c) {
	case 189: // '-' was pressed
		if(bSigned) {
			a = pData[curidx];
			if((a<=nMax) && (a>=nMin)) pData[curidx] *= -1;
		}
		break;
	case VK_DELETE:
		pData[curidx] = nNoteOff;
		break;
	default:
		if(((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'F'))) {
			int v = c - '0';
			if(v>9) v = c - 'A' + 10;//v now contains the value that was pressed, 0-f
			a = pData[curidx];
			if(!t->bEditing) {
				t->bEditing = true;
				a = 0;
			}
			a = (a<<4) + v;
			if((a<=nMax) && (a>=nMin)) pData[curidx] = a;
			if((a>(nMax >> 4)) || (a<(nMin >> 4))) t->bEditing = false; //we entered enough digits
		}
	}
}

CCounterTrack::CCounterTrack() : CBaseTrack("") {
	nPixelWidth = 2*FONTCHARWD+3;
	bReadonly = true;
}

void CCounterTrack::drawitem(int n, int x, int y, CDrawer* drawer) {
	char out[3]; //2 chars and a \0.
	//int v = n - _tracks->nPtnStart;
	wsprintf(out,"%02x",n);
	drawer->textout(out,x,y);
}