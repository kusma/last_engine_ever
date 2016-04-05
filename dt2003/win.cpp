#define TRACKER

#include <fstream>
#include "std.h"
extern "C" {
#include "tinyptc/tinyptc.h"
}
//#include <IL/il_wrap.h>
#include "track.h"
#include "common.h"
#include "writer.h"

unsigned int screen[WINSIZE];
CTracksDrawer* tdrawer;
CPatternsDrawer* pdrawer;
CPatterns* ptns;
CTracks* tracks;
Font* font;
HWND wnd;
enum Focus { fcTracks, fcOrder };
Focus focus = fcTracks;
unsigned char coltypes[TRACKLENGTH]; //bit 1-4: chan
HWND demo;
int blkstart,blkend;
bool pause;

POINT CopyStart;
POINT CopyEnd;


#define ORDERHT 150

CTrackGroup null("");


void Open();
void Save();

int getdemopos() {
	int c = 0;
	for(CPatterns::iterator i=ptns->begin();i!=ptns->pCurPtn;i++) {
		c += i->length;
	}
	return c + ptns->nPos;
}

bool IsDemo() {
	return IsWindow(demo)!=0;
}

void StartDemo() {
	char buf[123];
	
	HINSTANCE BANANA = ShellExecute(wnd,"open","\"C:\\Documents and Settings\\kusma.MALARIA\\My Documents\\source\\last_engine_ever\\Debug\\last_engine_ever.exe\"",itoa(getdemopos(),buf,10),"C:\\Documents and Settings\\kusma.MALARIA\\My Documents\\source\\last_engine_ever",SW_SHOW);
	//HINSTANCE BANANA = ShellExecute(wnd,"open","\"D:\\code\\proj\\last_engine_ever\\Debug\\last_engine_ever.exe\"",itoa(getdemopos(),buf,10),"d:\\code\\proj\\last_engine_ever",SW_SHOW);
	if(int(BANANA) > 32)
		while(!(demo = FindWindow("d3d9",THE_DEMOTITLE))) Sleep(100); //wait until the demo is found. (DANGER!)
	SendMessage(demo,CM_MOVEPOS,getdemopos(),0);

	//MessageBox(NULL,"2","2",MB_OK);
}

void PlayDemo() { //F5 key.
	if(IsDemo()) {
		SendMessage(demo,CM_MOVEPOS,getdemopos(),0);
		SendMessage(demo,CM_PAUSE,getdemopos(),0);
		pause = !pause;
	} else {
		StartDemo();
		pause = false;
	}
}



void KeyDown(WPARAM wParam, LPARAM lParam) {
	//* global keys
	if((wParam >= VK_F1) && (wParam <= VK_F12)) {
		tdrawer->eCurTab = TrackTab(wParam - VK_F1);
		tracks->nMatt = -1;
		tracks->MoveMatt(1); //set matt to the lestmost track that's in the current tab
		tracks->invalidate();
		return;
	}
	switch(wParam) {
	case VK_CONTROL:
	case VK_CAPITAL:
		//focus = (focus==fcTracks) ? fcOrder : fcTracks;
		focus = (GetAsyncKeyState(VK_CONTROL) || (focus==fcTracks)) ? fcOrder : fcTracks;
		tdrawer->bFocus = (focus==fcTracks);
		if(pdrawer->bFocus = (focus==fcOrder)) {
			int m = tracks->ct.nPixelWidth;
			for(int i=0;i<tracks->nMatt;i++) m+=(*tracks)[i]->nPixelWidth;
			pdrawer->nXOffset = m + (*tracks)[tracks->nMatt]->nPixelWidth / 2;
			//pdrawer->nBottom = pdrawer->nXOffset + ORDERHT;
		}
		tracks->invalidate();
		break;
	/*case VK_F7:
		Open();
		break;
	case VK_F8:
		Save();
		break;
	case VK_F5:
		Save();
		PlayDemo();
		break;*/
	}

	if(focus==fcTracks) {
	// tracks keys
		switch(wParam) {
/*		case VK_F1:
			blkstart = ts->nPos;
			break;
		case VK_F2:
			blkend = ts->nPos;
			break;
		case VK_F11:
			char buf[100];
			sprintf(buf,"copy all from %d till %d to here (%d)?",blkstart - ts->nPtnStart,blkend - ts->nPtnStart,ts->nPos - ts->nPtnStart);
			if(MessageBox(GetForegroundWindow(),buf,"BANANAS!",MB_YESNO|MB_ICONQUESTION)==IDYES) {
				for(int x=1; x<TRACKCOUNT; x++) {
					int pos = ts->nPos;
					short* gak = (short*)ts->tTracks[x]->pData;
					for(int y=blkstart;y<=blkend;y++) {
						gak[pos] = gak[y];
						pos++;
					}
				}
			}
			break;*/
		case VK_UP:
			ptns->MovePos(-1);
			//SendMessage(demo,CM_MOVEPOS,getdemopos(),0);
			break; 
		case VK_DOWN:
			ptns->MovePos(1);
			//SendMessage(demo,CM_MOVEPOS,getdemopos(),0);
			break;
		case VK_PRIOR:
			ptns->MovePos(-8);
			//SendMessage(demo,CM_MOVEPOS,getdemopos(),0);
			break;
		case VK_NEXT:
			ptns->MovePos(8);
			//SendMessage(demo,CM_MOVEPOS,getdemopos(),0);
			break;
		case VK_HOME:
			ptns->SetPos(0);
			break;
		case VK_END:
			ptns->SetPos(ptns->pCurPtn->length - 1);
			break;
		case VK_LEFT:
			tracks->MoveMatt(-1);
			break;
		case VK_RIGHT:
			tracks->MoveMatt(1);
			break;
		case VK_TAB:
			//tracks->MoveMatt((GetAsyncKeyState(VK_SHIFT))?-1:1,&(*tracks)[tracks->nMatt]->pGroup);
			if(GetAsyncKeyState(VK_SHIFT)) {
				tracks->MoveMatt(-1,&(*tracks)[tracks->nMatt]->pGroup);
				tracks->MoveMatt(-1,&(*tracks)[tracks->nMatt]->pGroup);
				tracks->MoveMatt(1);
			} else {
				tracks->MoveMatt(1,&(*tracks)[tracks->nMatt]->pGroup);
			}
			break;
		default:
			if(GetAsyncKeyState(VK_SHIFT)) {
				int moo;
				CIntTrack* blark = (CIntTrack*)&*(*tracks)[tracks->nMatt];
				switch(wParam) {
				case 'B':
					blark->nInterpStart = ptns->getmempoint();
					break;
				case 'E':
					moo = ptns->getmempoint();
					for(int i=blark->nInterpStart;i<=moo;i++) 
						blark->pInterp[i] = !blark->pInterp[i];
					tracks->invalidate();
					break;
				case 'I':
					for(int i=0;i<TRACKLENGTH;i++) blark->pInterp[i] = true;
					tracks->invalidate();
					break;
				case 'A':
					for(int i=0;i<TRACKLENGTH;i++) blark->pInterp[i] = false;
					tracks->invalidate();
					break;

				case 'P':
					writetd(FILENAME,tracks);
					PlayDemo();
					break;

				case 'C':
					CopyStart.x = tracks->nMatt;
					CopyStart.y = ptns->getmempoint();
					break;
				case 'D':
					CopyEnd.x = tracks->nMatt;
					CopyEnd.y = ptns->getmempoint();
					break;
				case 'V': 
					{
						int pos;
						int top = ptns->getmempoint();
						CIntTrack* it;
						for(int i=CopyStart.x;i<=CopyEnd.x;i++) {
							pos = 0;
							it = (CIntTrack*)&*(*tracks)[i];
							for(int j=CopyStart.y;j<=CopyEnd.y;j++) {
								it->pData[top+pos] = it->pData[j];
								it->pInterp[top+pos] = it->pInterp[j];
								pos++;
							}
						}
					}
				}
			} else {
				tracks->EnterValue(wParam); //keypress might be interesting for our current channel
			}
			//if(pause) Save();
		}
		

		//SendMessage(demo,CM_MOVEPOS,tracks->nPos,0);

	//* order editor keys 
	} else {
		switch(wParam) {
		case VK_UP:
			ptns->MoveCurPtn(true);
			//SendMessage(demo,CM_MOVEPOS,getdemopos(),0);
			break; 
		case VK_DOWN:
			ptns->MoveCurPtn(false);
			//SendMessage(demo,CM_MOVEPOS,getdemopos(),0);
			break;
		case 189:
		case VK_DELETE:
			ptns->DeletePtn(ptns->pCurPtn);
			break; //ignore delete and minus keys. */
		case VK_INSERT: 
			ptns->InsertPtn(ptns->pCurPtn);
			break;
		default:
			ptns->drawer.pt.EnterValue(wParam);
		}
	}
}


//finds the ultimate length of the whole thing in rows by adding up all the patternlengths.
/* int GetSongLen() {
	unsigned short* oData = (unsigned short*)os->tTracks[1]->pData;
	int len = 0;
	for(int i=0;i<=os->nPtnEnd;i++) 
		len += oData[i];
	return len;
} */

void Open() { /*
	ifstream ifs(FILENAME, ios::binary);
	char header[sizeof(FILEHEADER) + 1];
	ZeroMemory(&header,sizeof(header));
	ifs.read(header,sizeof(FILEHEADER));
	if(strcmp(FILEHEADER,header)!=0) {
		MessageBox(wnd,"file not good!","whoopz0rz",MB_OK);
	} else {
		//read orders
		ifs.read((char*)&os->nPtnStart,sizeof(os->nPtnStart));
		ifs.read((char*)&os->nPtnEnd,sizeof(os->nPtnEnd));
		ifs.read((char*)os->tTracks[1]->pData,ORDERDATASIZE*sizeof(unsigned short));
		//read track data
		int songlen = GetSongLen();
		for(int i=1;i<ts->nTrackCount;i++) {
			ifs.read((char*)ts->tTracks[i]->pData,songlen*sizeof(short));
		}
		ts->bModified = os->bModified = true;
		UpdatePtnLens();
		UpdateColTypes();
	}
*/ }

void Save() { /*
	ofstream ofs(FILENAME, ios::binary);
	//write header
    ofs.write(FILEHEADER,sizeof(FILEHEADER));
	//write orders
	ofs.write((char*)&os->nPtnStart,sizeof(os->nPtnStart));
	ofs.write((char*)&os->nPtnEnd,sizeof(os->nPtnEnd));
	ofs.write((char*)os->tTracks[1]->pData,ORDERDATASIZE*sizeof(unsigned short));
	//write track data
	int songlen = GetSongLen();
	for(int i=1;i<ts->nTrackCount;i++) {
		ofs.write((char*)ts->tTracks[i]->pData,songlen*sizeof(short));
	}
*/ }

#define SC_CUST_MASK 0x800
#define SC_CUST_OPEN 0x801
#define SC_CUST_SAVE 0x802

//The ptc message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
	case WM_KEYDOWN:
		KeyDown(wParam,lParam);
		break;
	case WM_KEYUP:
		if(wParam==VK_CONTROL) KeyDown(wParam,lParam);
		break;
	case WM_CLOSE:
		//if(MessageBox(hWnd,"Save?","BANANAS!",MB_YESNO|MB_ICONQUESTION)==IDYES) Save();
		//Save();
		PostQuitMessage(1);
		return 0;
		break;	
	case WM_SYSCOMMAND:
		switch(wParam) {
		case SC_CUST_OPEN:
			//load
			Open();
			break;
		case SC_CUST_SAVE:
			//save
			Save();
			break;
		default:
			return 0; //ptc also processes WM_SYSCOMMAND so let's not prohibit that.
		}
		break;
	case CM_MOVEPOS:
		//ts->SetAbsPos(wParam,os);
		//UpdateWindow(wnd);
		//PostMessage(wnd,WM_USER,0,0); //dummy message to invoke the repainting.
		{
			int c = 0;
			CPatterns::iterator i;
			for(i=ptns->begin();i!=ptns->end();i++) {
				if(c+i->length > wParam) break;
				c += i->length;
			}
			if(i==ptns->end()) { i = ptns->begin(); wParam = c = 31337; }
			ptns->pCurPtn = i;
			ptns->SetPos(wParam - c);
		}
		break;
	default:
		return 0;
    }
	//no value has been returned yet, but a message has been processed anyway.
	//we'll assume we won't want ptc also to process it.
    return 1; 
}

//Application entry point
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	int i;
	if (!(wnd = ptc_open(THE_TRACKERTITLE,WINWD,WINHT,WndProc))) return 1;//,WndProc

	{
		HMENU system_menu = GetSystemMenu(wnd,FALSE);
		AppendMenu(system_menu, MF_MENUBREAK, 0, NULL);
		AppendMenu(system_menu, MF_STRING, SC_CUST_OPEN, "Open");
		AppendMenu(system_menu, MF_STRING, SC_CUST_SAVE, "Save");
	}

	
	font = new Font(screen,"txt.raw");
	font->load();

	tdrawer = new CTracksDrawer(wnd,font,screen);
	pdrawer = new CPatternsDrawer(wnd,font,screen,0,(WINHT-ORDERHT)/2+70,(WINHT+ORDERHT)/2+70);
	ptns = new CPatterns(*pdrawer);
	pdrawer->pt.p = pdrawer->p = ptns; //ugly!
	tracks = new CTracks(*tdrawer,*ptns);
	//CPattern sdgsdag;
	//sdgsdag.index = 1;
	//ptns->push_back(sdgsdag);
	
	CTrackGroup null("");
	CTrackGroup campos("pos");
	CTrackGroup camtar("tar");
	CTrackGroup camroll("roll");
	CTrackGroup tunnel("tunnel");
	CTrackGroup blur("blur");
	CTrackGroup flash("flash");
	CTrackGroup kwl2("2d");
	CTrackGroup kwlr("rot");
	tdrawer->eCurTab = ttCam;
	//(*tracks)[0] = new CCounterTrack();
//	enum Tracks { tPart, tCamPosX, tCamPosY, tCamPosZ, tCamTarX, tCamTarY, tCamTarZ, tCamRelative, 
//		tBlurPasses, tBlurStrength, tFlash, tFadeOut };
//	enum TrackTab { ttCam, ttBlur, ttFlash, ttEmpty };

	(*tracks)[tPart] = new CIntTrack(tracks,"part",null,ttCam,4,true);
	(*tracks)[tCamPosX] = new CIntTrack(tracks,"x",campos,ttCam,4,true);
	(*tracks)[tCamPosY] = new CIntTrack(tracks,"y",campos,ttCam,4,true);
	(*tracks)[tCamPosZ] = new CIntTrack(tracks,"z",campos,ttCam,4,true);
	(*tracks)[tCamTarX] = new CIntTrack(tracks,"x",camtar,ttCam,4,true);
	(*tracks)[tCamTarY] = new CIntTrack(tracks,"y",camtar,ttCam,4,true);
	(*tracks)[tCamTarZ] = new CIntTrack(tracks,"z",camtar,ttCam,4,true);
	(*tracks)[tCamRelative] = new CIntTrack(tracks,"b",null,ttCam,1,false);	

	(*tracks)[tBlurPasses] = new CIntTrack(tracks,"p",blur,ttBlur,2,false);
	(*tracks)[tBlurStrength] = new CIntTrack(tracks,"s",blur,ttBlur,2,false);
	(*tracks)[tFlash] = new CIntTrack(tracks,"fl",flash,ttBlur,2,false);
	(*tracks)[tFadeOut] = new CIntTrack(tracks,"fo",flash,ttBlur,2,false);

	(*tracks)[tCamSelector] = new CIntTrack(tracks,"sel",null,ttCam,2,false);
	(*tracks)[tCamRoll] = new CIntTrack(tracks,"r",camroll,ttCam,4,true);

	(*tracks)[tKwlOuter] = new CIntTrack(tracks,"out",kwl2,ttKwl,4,true);
	(*tracks)[tKwlInner] = new CIntTrack(tracks,"in",kwl2,ttKwl,4,true);
	(*tracks)[tKwlOuterR] = new CIntTrack(tracks,"out",kwlr,ttKwl,4,true);
	(*tracks)[tKwlInnerR] = new CIntTrack(tracks,"in",kwlr,ttKwl,4,true);

	(*tracks)[tMarsShow] = new CIntTrack(tracks,"b_b",null,ttKwl,1,false);

	readtd(FILENAME,tracks);

	//(*tracks)[tApple] = new CIntTrack(tracks,"no",moo,ttCam,2,false);
	//tracks->MoveMatt(1);

	MSG message; //yes i ripped the message handling out of ptc. we're not a realtime app.
	while(GetMessage(&message,NULL,0,0)) {
		// translate and dispatch
		TranslateMessage(&message);
		DispatchMessage(&message);

//		if((ts->bModified) || (os->bModified)) 
		if(message.message==WM_PAINT) {
			for(i=0;i<WINWD*WINHT;i++) screen[i] = BACKCOL;//FF00FF;
			//int i = 9;
			tdrawer->draw();
			pdrawer->draw();
		}

		ptc_update(screen);
	}
	
	writetd(FILENAME,tracks);

	ptc_close();

	delete tracks;
	delete ptns;
	delete tdrawer;
	

	//DeleteObject(bmp);
	//DeleteDC(dc);

	return 31337;
}