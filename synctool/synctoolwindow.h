#pragma once

#ifdef SYNCTOOL

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlwinx.h>
#include <atlmisc.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
// #include <atlctrlw.h>
// #include <atlctrlx.h>
#include <atlgdi.h>
#include <atlsplit.h>
#include <atlres.h>
#include <atlscrl.h>
#include <atluser.h>

class SynctoolWindow : public CFrameWindowImpl<SynctoolWindow>, public CUpdateUI<SynctoolWindow> {
public:
//	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	BEGIN_MSG_MAP(CMyWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		CHAIN_MSG_MAP(CFrameWindowImpl<SynctoolWindow>)
	END_MSG_MAP()

	BEGIN_UPDATE_UI_MAP(SynctoolWindow)
//		UPDATE_ELEMENT(ID_EDIT_PASTE, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
	END_UPDATE_UI_MAP()

	LRESULT OnCreate(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
//		m_wndCmdBar.AttachMenu(GetMenu());
		m_hWndClient = splitter.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		log_window.Create(splitter, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE);
		log_window.ResetContent();
		for (unsigned i=0; i<100; i++) {
			char temp[256];
			sprintf(temp, "Warning: BIG not initialized %i", i);
			log_window.AddString(temp);
		}

		tab_window.Create(splitter, rcDefault, _T("balle"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		UpdateLayout();
		splitter.SetSplitterPanes(tab_window, log_window);
		RECT rect;
		GetClientRect(&rect);
		splitter.SetSplitterPos(((rect.bottom - rect.top) / 6) * 5);
		return 0;
	}

	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL&) {
		PostQuitMessage(0);
		return 0;
	}

	LRESULT OnKeyDown(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		// do da stuff
	}

private:
	//	CSplineEditor spline_editor;
	//	CTrackEditor track_editor;
	CTabCtrl tab_window;
	CListBox log_window;
	CHorSplitterWindow splitter;
};

#endif