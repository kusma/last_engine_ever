#define BPM 120

#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
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

using namespace engine;

float flash_time = -1000031337.f;
float flash_speed = 2.f;

int main(int argc, char *argv[]) {
	try {
		D3DWin win("RETURN OF HYPNOGLOW", 640, 480, D3DFMT_X8R8G8B8, true);
		device = win.get_device();

		engine::MeshProxy mesh_proxy;
		engine::SceneProxy scene_proxy;
		engine::TextureProxy texture_proxy;

		for (unsigned i=0; i<8; i++) {
			device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
			device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		}

		BassPlayer player(win.get_window());
		player.load(std::string("hypnoshite.ogg"), 0 * (60.f / BPM));

		engine::Texture *flashtex = texture_proxy.get_resource("white.png");
		engine::Image flashimage(flashtex);

		engine::Texture *fadetex = texture_proxy.get_resource("black.png");
		engine::Image fadeimage(fadetex);

		engine::Texture *hypno_tex = texture_proxy.get_resource("hypno.jpg");
		engine::Image hypno_image(hypno_tex);

		engine::Texture *lokotex = texture_proxy.get_resource("loko.png");

		engine::Texture *returntex = texture_proxy.get_resource("return.png");
		engine::Image returnimage(returntex);

		engine::Texture *oftex = texture_proxy.get_resource("of.png");
		engine::Image ofimage(oftex);

		engine::Texture *hypnotex = texture_proxy.get_resource("hypno2.jpg");
		engine::Image hypnoimage(hypnotex);

		engine::Texture *glowtex = texture_proxy.get_resource("glow.jpg");
		engine::Image glowimage(glowtex);
/*
		engine::Texture *iljatex = texture_proxy.get_resource("ilja.png");
		engine::Image iljaimage(iljatex);
*/
		Light light1, light2;

		engine::RenderTexture render_texture(256, 256);
		engine::BlurTexture blurtexture(&render_texture);
		engine::BlurTexture blurtexture2(&blurtexture);
		engine::Image fuck_blurimage(&blurtexture2);
		engine::BlurTexture blurtexture3(&blurtexture2);
		engine::BlurTexture blurtexture4(&blurtexture3);
		engine::Image intro_blurimage(&blurtexture4);
		engine::Entity lookat;
		engine::LookAtCamera cam(&lookat);
		engine::LookAtCamera cam2;
		Grid g;

		engine::Scene *fjallscene = scene_proxy.get_resource("hypno.scene");
		fjallscene->add(&cam);
		fjallscene->add(&cam2);
		fjallscene->add(&light1);

		Timer fps_timer;
		player.play();
		bool done = false;

		while (!done) {
			float time = player.get_time() * (float(BPM) / 60);

			player.update_spectrum();
			device->BeginScene();

			cam.fov = 80 + player.sample_spectrum(0.1f) * 500;
			cam.prs.position = engine::Vector( sin(time*0.30f)*60, cos(time*0.7f)*90, sin(-time*0.7f)*90 );

			cam2.fov = 90 - player.sample_spectrum(0.2f) * 800;
			cam2.prs.position = engine::Vector( sin(-time*0.30f)*70, sin(time*0.7f)*90, cos(time*0.7f)*90 );

			render_texture.set();
			fjallscene->objects[0]->prs.rotation = Quaternion( Vector( time*0.25f, sin(-time*0.5f), player.sample_spectrum(0.2f) * 100 ) );
			fjallscene->draw(unsigned(time / 4) & 1);
			render_texture.unset();

			if (unsigned(time / 32) & 1) fjallscene->draw(unsigned(time / 4) & 1);

			blurtexture.do_blur(true,	1, 1.0f);
			blurtexture2.do_blur(false,	1, 1.0f);
			blurtexture3.do_blur(true,	2, player.sample_spectrum(0.0f) * 100);
			blurtexture4.do_blur(false,	2, player.sample_spectrum(0.0f) * 100);

			if (unsigned(time / 32) & 1) intro_blurimage.draw(D3DBLEND_ONE, D3DBLEND_ONE);
			else intro_blurimage.draw(D3DBLEND_ONE, D3DBLEND_ZERO);
/*
			if (time > 16) {
				g.reset(cos(time-1) - int(time / 8), sin(time + int(time / 8)));
				g.wave(sin(time)*0.1f, cos(time)*0.1f, 0.3f,  0.1f + player.sample_spectrum(0.4f) * 200);
				g.wave(cos(time-1)*0.2f, sin(time)*0.2f, 0.2f, 0.1f +  player.sample_spectrum(0.3f) * 200);
				g.wave(sin(time-1)*0.3f, cos(-time)*0.2f, 0.1f,  0.1f + player.sample_spectrum(0.2f) * 100);
				g.update();
				g.draw(messagestex, D3DBLEND_SRCALPHA, D3DBLEND_ONE, 0.05f);
			}
*/
			hypno_image.width = 1.5f + player.sample_spectrum(0.01f) * 10;
			hypno_image.height = 2 + player.sample_spectrum(0.01f) * 10;
			hypno_image.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, player.sample_spectrum(0.05f) * 50, -time * 3);
/*
			if (time > 80 && time < 90) {
				iljaimage.draw(D3DBLEND_DESTCOLOR, D3DBLEND_ZERO);
			}
*/
			if (time > 101) {
				g.reset(time*100, 0.f);
				g.wave(sin(time)*0.1f, cos(time)*0.1f, 0.3f,  player.sample_spectrum(0.4f) * 200);
				g.wave(cos(time-1)*0.2f, sin(time)*0.2f, 0.2f,  player.sample_spectrum(0.3f) * 200);
				g.wave(sin(time-1)*0.3f, cos(-time)*0.2f, 0.1f,  player.sample_spectrum(0.2f) * 100);
				g.update();
				if (time < 140) g.draw(lokotex, D3DBLEND_INVDESTCOLOR, D3DBLEND_SRCALPHA, 0.f);
				else g.draw(lokotex, D3DBLEND_INVDESTCOLOR, D3DBLEND_ZERO, 0.f);

				if (time < 105) flashimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, 1 - (time - 101));
			}

			if (time > 8 && time < 16) {
				if (time < 9) returnimage.draw(D3DBLEND_ONE, D3DBLEND_ONE);
				else ofimage.draw(D3DBLEND_ONE, D3DBLEND_ONE);
			}

			if (time > 10 && time < 16) {
				if (time < 11) hypnoimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, player.sample_spectrum(0.2f) * 5000);
				else glowimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, player.sample_spectrum(0.2f) * 5000);
			}

			if (time > 140) flashimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, time-140);

			device->EndScene();
			if (device->Present(0, 0, 0, 0) == D3DERR_DEVICELOST) throw std::string("Lost device.");

			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (msg.message == WM_QUIT) done = true;
				if (msg.message == WM_KEYDOWN) {
//					printf("time %f\n", time);
//					printf("time %f\n", fps_timer.get_time());
					flash_time = time;
				}
			}
			if (GetAsyncKeyState(VK_ESCAPE)) done = true;
			if (time > 144) done = true;

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
