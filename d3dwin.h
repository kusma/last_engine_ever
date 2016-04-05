#ifndef _D3DWINDOW_H_
#define _D3DWINDOW_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "BassPlayer.h"
#include "..\dt2003\reader.h"

class D3DWin {
public:
	D3DWin(const char *title, unsigned int width, unsigned int height, D3DFORMAT format, bool fullscreen);
	~D3DWin();

	IDirect3DDevice9 *get_device() {
		return device;
	}

	HWND get_window() {
		return window;
	}

	void setstuff(BassPlayer* p, Syncreader* s) { player = p; sync = s; };

private:
	static LRESULT CALLBACK window_proc(HWND win, UINT message, WPARAM wparam, LPARAM lparam);
	HINSTANCE instance;
	IDirect3D9 *direct3d;
	IDirect3DDevice9 *device;
	HWND window;
	bool pause;
	BassPlayer* player;
	Syncreader* sync;
};

#endif /* _D3DWINDOW_H_ */
