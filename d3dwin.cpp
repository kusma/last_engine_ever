#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
#include <string>
#include "d3dwin.h"

D3DWin::D3DWin(const char *title, unsigned int width, unsigned int height, D3DFORMAT format, bool fullscreen) : direct3d(0), device(0), window(0) {
	instance = GetModuleHandle(NULL);

	WNDCLASS wc;
	wc.style = CS_VREDRAW|CS_HREDRAW;
	wc.lpfnWndProc = window_proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = instance;
	wc.hIcon = 0;
	wc.hCursor = LoadCursor(instance,IDC_ARROW);
	wc.hbrBackground = 0;
	wc.lpszMenuName = 0;
	wc.lpszClassName = "d3d9";
	RegisterClass(&wc);
	
	window = CreateWindowEx(0, "d3d9", title, WS_POPUP | WS_VISIBLE, 0, 0, width, height, 0, 0, instance, 0);
	if (!window) throw std::string("CreateWindowEx failed");

	direct3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!direct3d) throw std::string("Direct3DCreate9() failed");

	D3DPRESENT_PARAMETERS presentparameters;
	memset(&presentparameters, 0, sizeof(D3DPRESENT_PARAMETERS));
	presentparameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentparameters.BackBufferWidth = width;
	presentparameters.BackBufferHeight = height;
	presentparameters.AutoDepthStencilFormat = D3DFMT_D24S8;
	presentparameters.EnableAutoDepthStencil = TRUE;
	presentparameters.Windowed = !fullscreen;
	presentparameters.MultiSampleQuality = D3DMULTISAMPLE_NONE;
//	presentparameters.MultiSampleQuality = D3DMULTISAMPLE_2_SAMPLES;

	D3DDISPLAYMODE displaymode;
	if (!fullscreen) {
		if(FAILED(direct3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displaymode))) throw std::string("GetAdapterDisplayMode() failed");
		presentparameters.BackBufferFormat = displaymode.Format;
	} else {
		presentparameters.BackBufferFormat = format;
		ShowCursor(FALSE);
	}


	D3DCAPS9 caps;
	direct3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	if (!(caps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT)) MessageBox(NULL, "Your GPU is from Poland!", "Warning", MB_OK);

	if (FAILED(direct3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
//		D3DDEVTYPE_REF,
		window,
		(caps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT) ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&presentparameters, &device)))
		throw std::string("CreateDevice() failed");
}

D3DWin::~D3DWin() {
	ShowCursor(TRUE);

	if (device) {
		device->EvictManagedResources();
		device->Release();
		device = 0;
	}

	if (direct3d) {
		direct3d->Release();
		direct3d = 0;
	}

	if (window) {
		DestroyWindow(window);
		window = 0;
	}

	UnregisterClass("d3d9", instance);
}

LRESULT CALLBACK D3DWin::window_proc(HWND win,UINT message,WPARAM wparam,LPARAM lparam) {
	switch (message) {

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	break;

	case WM_SYSCOMMAND:
		switch (wparam) {
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			return 0;
		}
	break;

	}
    return DefWindowProc(win,message,wparam,lparam);
}
