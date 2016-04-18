#ifndef _D3DWINDOW_H_
#define _D3DWINDOW_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "BassPlayer.h"

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

private:
	static LRESULT CALLBACK window_proc(HWND win, UINT message, WPARAM wparam, LPARAM lparam);
	HINSTANCE instance;
	IDirect3D9 *direct3d;
	IDirect3DDevice9 *device;
	HWND window;
};

#endif /* _D3DWINDOW_H_ */
