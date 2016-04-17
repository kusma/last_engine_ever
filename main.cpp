#define BPM 130

#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES

#include <float.h>

#include <windows.h>
#include <d3d9.h>
#include <string>

#include "d3dwin.h"
#include "Timer.h"
#include "BassPlayer.h"

IDirect3DDevice9 *device;

#include "engine/Vector.h"
#include "engine/Matrix.h"
#include "engine/Quaternion.h"
#include "engine/Scene.h"
#include "engine/Texture.h"
#include "engine/RenderTarget.h"
#include "engine/RenderTexture.h"
#include "engine/Image.h"
#include "engine/BlurTexture.h"
#include "engine/Grid.h"

#include "engine/SceneProxy.h"
#include "engine/MeshProxy.h"
#include "engine/TextureProxy.h"

#include "engine/LinearCurve.h"

using namespace engine;

int main(int argc, char *argv[]) {
	try {
		D3DWin win("russian electronic cubes", 800, 600, D3DFMT_X8R8G8B8, true);
		device = win.get_device();

		SceneProxy scene_proxy;
		MeshProxy mesh_proxy;
		MaterialProxy material_proxy;
		TextureProxy texture_proxy;

		for (unsigned i=0; i<8; i++) {
			device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		}

		// loading sceen
		Image loadingscreen(texture_proxy.get_resource("load.png"));
		device->BeginScene();
		loadingscreen.draw();
		device->EndScene();
		if (device->Present(0, 0, 0, 0) == D3DERR_DEVICELOST) throw std::string("Lost device.");
		Sleep(10000);


		// load music
		BassPlayer player(win.get_window());
		player.load(std::string("musikk.mp3"), 0 * (60.f / BPM));

		// images
		Image flashimage(texture_proxy.get_resource("white.png"));

		Timer fps_timer;
		player.play();
		bool done = false;
		while (!done) {
			float time = float(player.get_time() * (double(BPM) / 60));
			static float last_time = 0.f;
			float delta_time = time - last_time;

			player.update_spectrum();
			device->BeginScene();

			device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);
			flashimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, player.sample_spectrum(0.7f) * 1000);

			device->EndScene();
			if (device->Present(0, 0, 0, 0) == D3DERR_DEVICELOST) throw std::string("Lost device.");
			last_time = time;

			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (msg.message == WM_QUIT) done = true;
				if (msg.message == WM_KEYDOWN) printf("time %f\n", time);
			}

			if (GetAsyncKeyState(VK_ESCAPE)) done = true;
			if (time > 265) done = true;

#ifndef NDEBUG
			double time__ = fps_timer.get_time();
			fps_timer.reset();
			printf("FPS: %f\r", 1.f / time__);
#endif
		}
	} catch (std::string reason) {
		MessageBox( NULL, reason.c_str(), NULL, MB_OK );
		return 1;
	}

	return 0;
}
