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
#include "engine/VideoTexture.h"

#include "engine/SceneProxy.h"
#include "engine/MeshProxy.h"
#include "engine/TextureProxy.h"

#include "engine/curve.h"

using namespace engine;

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

float timetable[] = { 0, 1, 5, 6, 10, 14, 16, 24, 25, 28, 30, 42, 46, 76, 76.5, 77, 84, 92, 92.5, 93, 110, 112, 128, 129, 162, 164, 178, 180, 198, 199, 208, 220, 221, 223, 224, 256 };
unsigned time_index = 0;

int main(int argc, char *argv[]) {
	try {
		D3DWin win("russian electronic cubes", 640, 480, D3DFMT_X8R8G8B8, true);
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

		VideoTexture *video_texture = new VideoTexture(std::string("test.kpg"));
		texture_proxy.insert_resource("bg.jpg", video_texture);
		texture_proxy.insert_resource("cubetex.png", video_texture);
		texture_proxy.insert_resource("white.png", video_texture);

		Curve<float> curve;
		curve.add_key(0.f, 0.f);
		curve.add_key(1.f, 1.f);
		curve.add_key(2.f, 0.f);
		curve.add_key(3.f, 1.f);
		curve.add_key(4.f, 0.f);
		curve.add_key(5.f, 1.f);
		curve.add_key(6.f, 0.f);
		curve.calculate_tangents();

		// load music
		BassPlayer player(win.get_window());
		player.load(std::string("panzercrack.mp3"), 0 * (60.f / BPM));

		// images
		Image flashimage(texture_proxy.get_resource("white.png"));
		Image fadeimage(texture_proxy.get_resource("black.png"));
		Image text(texture_proxy.get_resource("text0.png"));

		unsigned current_text = 0;

		Light light1, light2;
		RenderTexture render_texture(256, 128);
		BlurTexture blurtexture1(&render_texture);
		BlurTexture blurtexture2(&blurtexture1);
		BlurTexture blurtexture3(&blurtexture2);
		BlurTexture blurtexture4(&blurtexture3);
		BlurTexture blurtexture5(&blurtexture4);
		BlurTexture blurtexture6(&blurtexture5);
		Image intro_blurimage(&blurtexture6);

		Entity lookat;
		LookAtCamera cam(&lookat);
		Scene *fjallscene = scene_proxy.get_resource("electrocube.scene");
		fjallscene->add(&cam);
		fjallscene->add(&light1);
		fjallscene->fog_near = 60;
		fjallscene->fog_far = 400;
		fjallscene->fog_color = Color(0.05f, 0.01f, 0.01f);
		fjallscene->fog = true;
		cam.zfar = 2000;

		Grid g;

		// randomize demo :)
		srand( GetTickCount() );
		bool spinning = false;


		D3DGAMMARAMP grote_ramp, normal_ramp;

		// only for manly machines
#define RAMP_GRANULARITY 8
		for (int i = 0; i < 256; i++) {
//			int val = (i + (rand() & (0xF >> 1)) > 128) * 65535;
//			int val = (i >> 6) << (7 + 8);
//			int val = (((i - 128) * 1) + 128) * 65536;
			int val = (((i - 128) * 8) + 128) * 256;
			val = max(val, 0);
			val = min(val, 0xFFFF);

			grote_ramp.red[i] = (val & 0xFFFF) ^ 1337;
			grote_ramp.blue[i] = (val & 0xFFFF);
			grote_ramp.green[i] = (val & 0xFFFF);
/*			grote_ramp.red[i] = ((i / RAMP_GRANULARITY) & 1) * 0xffff;
			grote_ramp.blue[i] = ((i / RAMP_GRANULARITY) & 1) * 0xffff;
			grote_ramp.green[i] = ((i / RAMP_GRANULARITY) & 1) * 0xffff; */
		}
//		device->SetGammaRamp(0, 0, &grote_ramp);


		Timer fps_timer;
		player.play();
		bool done = false;
		while (!done) {
			float time = float(player.get_time());
			static float last_time = 0.f;
			float delta_time = time - last_time;

			if (time_index < ARRAY_SIZE(timetable)) {
				while (timetable[time_index] < time) time_index++;
			}

			video_texture->update(time * 2);
			player.update_spectrum();
			device->BeginScene();

			float text_alpha = 0.f;
			fjallscene->objects[27]->get_mesh()->submeshes[0].material->diffuse.a = curve.get_val(time);
				// text_alpha;

			cam.fov = 90 + player.sample_spectrum(0.1f) * 1000;
			if (time_index & 1) { //(unsigned(time)>>3)&1) {
				cam.prs.position = Vector(-sin(time*0.3f)*80, cos(time*0.3f)*80, cos(-time*0.4f)*20);
			} else {
				cam.prs.position = Vector(sin(time*0.3f)*100, cos(time*0.7f)*40, cos(-time*0.3f)*60);
			}

			light1.prs.position = Vector(
				float(sin(-time * 0.7) * 70),
				float(sin(time * 0.7) * 70),
				float(cos(time * 0.7) * 70));

			light2.prs.position = Vector(
				float(sin(-time * 0.7 + M_PI) * 90),
				float(cos(time * 0.6) * 70),
				float(cos(time * 0.7 + M_PI) * 70));

			render_texture.set();
			fjallscene->draw(false);

//			device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
//			flashimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, 0.1f);
//			device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			render_texture.unset();

			blurtexture1.do_blur(true,	1.0f, 1.5f);
			blurtexture2.do_blur(false,	1.0f, 1.f);
			blurtexture3.do_blur(true,	1.5f, 1.f);
			blurtexture4.do_blur(false,	1.5f, 1.f);
			blurtexture5.do_blur(true,	2.0f, 1.25f);
			blurtexture6.do_blur(false,	2.0f, 1.4f);

			fjallscene->draw(false);

			g.reset(0,0);
			g.wave(0, 0, max(0.3f, player.sample_spectrum(0.4f) * 100), max(0.9f, player.sample_spectrum(0.2f) * 10));
			g.update();
			g.draw(intro_blurimage.get_texture(), D3DBLEND_ONE, D3DBLEND_ONE);

//			intro_blurimage.draw(D3DBLEND_ONE, D3DBLEND_ONE);
			g.reset(0,0);
			g.wave(0, 0, max(0.2f, player.sample_spectrum(0.4f) * 200), max(0.9f, player.sample_spectrum(0.4f) * 2000));
			g.twirl(0, 0, 1, 0);
			g.update();
			g.draw(&render_texture, D3DBLEND_ONE, D3DBLEND_ONE);

			if (time >= 16 && (time < 152 || time > 160)) flashimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, pow(1.f - fmod(time, 1.f), 1.5f));
			if (time_index < ARRAY_SIZE(timetable)) {
				if ((time < 75 || time > 80) && (time < 91 || time > 96)) {
					float start_time = -FLT_MAX;
					if (time > 0) start_time = timetable[time_index - 1];
					float end_time = timetable[time_index];
					float t0 = time - start_time;
					float t1 = end_time - time;
					float t = min(t0 * 1.75f, t1 * 1.75f);
					if (t > 0.001f) fadeimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, 1.25 - t);
				}
			}
			if (time > 31) {
				float t = (time - 31) / (32 - 31);
				fadeimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, t * 4);
			}

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
			if (time > 32) done = true;

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
