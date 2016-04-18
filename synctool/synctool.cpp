#include "synctool.h"

#ifdef SYNCTOOL
#include "synctoolwindow.h"
#endif

using namespace sync;

Synctool::Synctool(std::string filename, BassPlayer *bassplayer) : bassplayer(bassplayer) {
#ifdef SYNCTOOL
	CAppModule _Module;
	HINSTANCE hInstance = GetModuleHandle(0);
	HRESULT hRes = _Module.Init( NULL, hInstance );

	::InitCommonControls(); 
	hRes = ::CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));
	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_USEREX_CLASSES);

	if (!sync_win.Create(NULL, CWindow::rcDefault, _T("SyncTool(c)(r)(etc)"), WS_OVERLAPPEDWINDOW, 0)) {
		MessageBox(0, "failed to create window", 0, MB_OK);
		exit(1);
	}

	sync_win.ShowWindow(true);
	sync_win.UpdateWindow();
#endif
}

