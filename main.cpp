#define BPM 128.94f

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
#include "synctool/synctool.h"

using namespace engine;

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

float timetable[] = {
	16, 24, 32, 40, 56, 64.5, 72, 
	80, 88, 97, 104, 112, 121, 128, 
	136, 152, 160, 164, 165, 166, 167, 168
};
float last_time = -FLT_MAX;
unsigned time_index = 0;

#ifdef SYNCTOOL
CAppModule _Module;
#endif

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
#if 0
		Image loadingscreen(texture_proxy.get_resource("load.png"));
		device->BeginScene();
		loadingscreen.draw();
		device->EndScene();
		if (device->Present(0, 0, 0, 0) == D3DERR_DEVICELOST) throw std::string("Lost device.");
#endif
/*
		VideoTexture *video_texture = new VideoTexture(std::string("test.kpg"));
		texture_proxy.insert_resource("bg.jpg", video_texture);
		texture_proxy.insert_resource("cubetex.png", video_texture);
*/

		// load music
		BassPlayer player(win.get_window());
		player.load(std::string("thorsten.mp3"), 0 * (60.f / BPM));

//		sync::Synctool sync(std::string("test.xml"), &player);

		// images
		Image flashimage(texture_proxy.get_resource("white.png"));
		Image fadeimage(texture_proxy.get_resource("black.png"));
		Image credimage(texture_proxy.get_resource("credits.png"));
		credimage.width = 0.55f;
		credimage.height = 0.25f;
		std::vector<Image*> images;

		for (unsigned i=0; i<23; i++) {
			char temp[256];
			sprintf(temp, "text%02i.png", i);
			images.push_back(new Image(texture_proxy.get_resource(temp)));
			images[i]->width = 0.55f;
			images[i]->height = 0.25f;
		}

/*		images[1] = Image(texture_proxy.get_resource("text01.png"));
		images[2] = Image(texture_proxy.get_resource("text02.png"));
		images[3] = Image(texture_proxy.get_resource("text03.png"));
		images[4] = Image(texture_proxy.get_resource("text04.png"));
*/
		unsigned current_text = 0;

		Light light1, light2;
		RenderTexture render_texture(128, 128);
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
		fjallscene->objects[0]->prs.rotation = Quaternion(Vector(1, 0, 0), M_PI / 2);
		fjallscene->add(&cam);
		fjallscene->add(&light1);
		fjallscene->fog_near = 60;
		fjallscene->fog_far = 500;
		fjallscene->fog_color = Color(0.03f, 0.01f, 0.04f);
		fjallscene->fog = true;
		cam.zfar = 2000;

		// randomize demo :)
		srand(GetTickCount());
		bool spinning = false;

		Timer fps_timer;
		player.play();

#ifdef SYNCTOOL
		player.pause();
#endif

		bool done = false;
		while (!done) {
			float time = float(player.get_time() * (double(BPM) / 60));
			static float last_time = 0.f;
			float delta_time = time - last_time;

			if (time_index < ARRAY_SIZE(timetable)) {
				while (timetable[time_index] < time) time_index++;
			}

//			video_texture->update(time * 2);
			player.update_spectrum();
			device->BeginScene();

			if (time < 8) fjallscene->objects[1]->get_mesh()->submeshes[0].material->diffuse.a = 0;
			else fjallscene->objects[1]->get_mesh()->submeshes[0].material->diffuse.a = 1;

			cam.fov = 80 + player.sample_spectrum(0.2f) * 800;
			cam.prs.position = Vector(-sin(time*0.3f)*80, cos(time*0.4f)*30, cos(-time*0.3f)*80);
			lookat.prs.position = cam.prs.position + Vector(cos(time*0.33f)*30, cos(time*0.5f)*30, -cos(time*0.2f)*80);
			fjallscene->objects[1]->prs.position = lookat.prs.position;
			fjallscene->objects[1]->prs.rotation = Quaternion(Vector(sin(time * 0.15f) * 0.3f, cos(1-time * 0.2f) * 0.3f, sin(sin(time * 0.1f) * 0.3f) * 0.3f).normalize(), M_PI / 2 + time - 4);
			fjallscene->objects[2]->prs.rotation = fjallscene->objects[1]->prs.rotation;
			fjallscene->objects[3]->prs.rotation = fjallscene->objects[1]->prs.rotation;
			fjallscene->objects[4]->prs.rotation = fjallscene->objects[1]->prs.rotation;
			cam.roll = player.sample_spectrum(0.5f);

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
			render_texture.unset();

			blurtexture1.do_blur(true,	1.0f, 1.5f);
			blurtexture2.do_blur(false,	1.0f, 1.f);
			blurtexture3.do_blur(true,	1.5f, 1.f);
			blurtexture4.do_blur(false,	1.5f, 1.f);
			blurtexture5.do_blur(true,	2.0f, 1.25f);
			blurtexture6.do_blur(false,	2.0f, 1.4f);

			fjallscene->draw(false);

			intro_blurimage.draw(D3DBLEND_ONE, D3DBLEND_ONE);

			if (time > 8) {
				if (time_index < images.size()) images[time_index]->draw(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, 1, 0);
				else images[images.size() - 1]->draw(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, 170 - time, 0);
			}

			if (time > 192) {
				float t = (time - 192) / (200 - 192);
				float t2 = (time - 200) / (192 - 200);
				credimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, min(t, t2) * 4);
			}

			if (time > 200) {
				float t = (time - 200) / (232 - 200);
				fadeimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, t);
			}

//			if (time > (232 - 16)) fadeimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPH, (time - 232) * (1.f / 16));

/*
			if (time > 178) {
				float t = (time - 178);
				credimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, 170 - time, cos(t));
			}
*/
			if (time > 8) flashimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, pow(1.f - fmod(time, 1.f), 2.5f));


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
#ifndef SYNCTOOL
			if (GetAsyncKeyState(VK_ESCAPE)) done = true;
			if (time > 232) done = true;
#endif

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
