#define BPM 130

#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES

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

using namespace engine;


float flash_time = -1000031337.f;
float flash_speed = 2.f;

int rot_speed[26] = { 0 };
Quaternion old_rot[26];
Quaternion new_rot[26];


Scene *fjallscene;
Vector axis(0, 0, 0);
Vector shake_axis(0, 0, 0);

void recalc_speeds() {
	axis = Vector(0, 0, 0);
	((float*)&axis)[rand() % 3] = 1.f * ((rand() & 1) * 2 - 1);
	shake_axis = Vector(1.f - fabs(axis.x), 1.f - fabs(axis.y), 1.f - fabs(axis.z));

	bool both = !(rand() & 1);

	int rot_speed_left = 0;
	int rot_speed_right = 0;

/*	while (rot_speed_left == 0) rot_speed_left = (rand() & 6) - 4;
	while (rot_speed_right == 0) rot_speed_right = (rand() & 6) - 4; */

	while (rot_speed_left == 0) rot_speed_left = ((rand() % 3) - 1) << 1;
	while (rot_speed_right == 0) rot_speed_right = ((rand() % 3) - 1) << 1;

	for (unsigned i=0; i<26; i++) {
		Object *obj = fjallscene->objects[i];
		Matrix m = obj->get_absolute_matrix() * obj->get_pivot_matrix();
		Vector dir = m.transform_vector(fjallscene->objects[i]->get_mesh()->center_of_mass).normalize();
		rot_speed[i] = 0;
		if (dir.dot_product(axis) >= 0.01f) rot_speed[i] = rot_speed_left;
		if (both && (rot_speed[i] == 0) && (dir.dot_product(axis) <= -0.01f)) rot_speed[i] = rot_speed_right;
	}
}

int main(int argc, char *argv[]) {
	try {
		D3DWin win("russian electronic cubes", 800, 600, D3DFMT_X8R8G8B8, true);
		device = win.get_device();

		engine::SceneProxy scene_proxy;
		engine::MeshProxy mesh_proxy;
		engine::MaterialProxy material_proxy;
		engine::TextureProxy texture_proxy;

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

		// load music
		BassPlayer player(win.get_window());
		player.load(std::string("rewyg.mp3"), 0 * (60.f / BPM));

		// images
		Texture *flashtex = texture_proxy.get_resource("white.png");
		Image flashimage(flashtex);

		Texture *fadetex = texture_proxy.get_resource("black.png");
		Image fadeimage(fadetex);

		Texture *kgblogotex = texture_proxy.get_resource("kgblogo.png");
		Image kgblogoimage(kgblogotex);

		Material *dark = material_proxy.get_resource("dark.material");
		Material *light = material_proxy.get_resource("light.material");

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

		fjallscene = scene_proxy.get_resource("electrocube.scene");
		fjallscene->add(&cam);
		fjallscene->add(&light1);

		// randomize demo :)
		srand( GetTickCount() );
		bool spinning = false;

		Timer fps_timer;
		player.play();
		bool done = false;
		float last_time = 0.f;
		while (!done) {
			float time = player.get_time() * (float(BPM) / 60);
			float delta_time = time - last_time;

			player.update_spectrum();
			device->BeginScene();

			cam.fov = 80 + player.sample_spectrum(0.1f) * 300;
			if (0) { //(unsigned(time)>>3)&1) {
				cam.prs.position = Vector(-sin(time*0.3f)*80, cos(time*0.3f)*80, cos(-time*0.4f)*20);
			} else {
				cam.prs.position = Vector(sin(time*0.3f)*90, cos(time*0.7f)*40, cos(-time*0.3f)*90);
			}

			light1.prs.position = Vector(sin(-time*0.7)*90, sin(time*0.7f)*90, cos(time*0.7f)*90);
			light2.prs.position = Vector(sin(time*0.40f)*90, sin(time*0.6f)*70, cos(time*0.4f)*70);

			if (time > 64.f) {
				if (fmod(time, 2.f) >= 1.f) {
					if (!spinning) {
						recalc_speeds();
						spinning = true;
					}
				} else {
					if (spinning) {
						spinning = false;
						for (unsigned i=0; i<26; i++) fjallscene->objects[i]->prs.rotation = Quaternion(1, 0, 0, 0);
					}
					float t = fmod(time, 2.f);
//					Vector offs = Vector(sin((t-1)*649) * shake_axis.x, sin(t*798) * shake_axis.y, sin((t+1)*705) * shake_axis.z) * pow(1.f - t, 1.8f) * 15;
					Vector offs = axis * sin((t-1)*705) * pow(1.f - t, 1.8f) * 0.2f;
					for (unsigned i=0; i<26; i++) fjallscene->objects[i]->prs.rotation = offs;
				}

				if (spinning) {
					for (unsigned i=0; i<26; i++) {
						if (rot_speed[i] != 0) fjallscene->objects[i]->prs.rotation = fjallscene->objects[i]->prs.rotation * Quaternion(axis, delta_time * (M_PI / rot_speed[i]));
					}
				}
			}

			if (time > 160) {
				if (player.sample_spectrum(0.01f) > 0.3f) {
					for (unsigned i=0; i<26; i++) {
						fjallscene->objects[i]->get_mesh()->submeshes[0].material = (float(rand()) / RAND_MAX) > 0.75f ? light : dark;
					}
				}
			}

//			fjallscene->objects[26]->prs.rotation = Vector(sin(time), time, cos(-time));

			float fog_start = 60;
			float fog_end = 300;
			device->SetRenderState(D3DRS_FOGCOLOR, 0x070303);
			device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
			device->SetRenderState(D3DRS_FOGSTART, *((DWORD*) (&fog_start)));
			device->SetRenderState(D3DRS_FOGEND, *((DWORD*) (&fog_end)));

			render_texture.set();
			device->SetRenderState(D3DRS_FOGENABLE, true);
			fjallscene->draw();
			device->SetRenderState(D3DRS_FOGENABLE, false);

			device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
//			fadeimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, 0.5f);
			flashimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, 0.3f);
			device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			render_texture.unset();

			blurtexture1.do_blur(true,	1.0f, 1.5f);
			blurtexture2.do_blur(false,	1.0f, 1.f);
			blurtexture3.do_blur(true,	1.5f, 1.f);
			blurtexture4.do_blur(false,	1.5f, 1.f);
			blurtexture5.do_blur(true,	2.0f, 1.25f);
			blurtexture6.do_blur(false,	2.0f, 1.5f);

//			intro_blurimage.draw(D3DBLEND_ONE, D3DBLEND_ZERO);
			device->SetRenderState(D3DRS_FOGENABLE, true);
			fjallscene->draw();
			device->SetRenderState(D3DRS_FOGENABLE, false);

//			kgblogoimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, 0.2f);
			kgblogoimage.width = (3.f / 4.f) * (5.f / (time + 1));
			kgblogoimage.height = 1.f * (5.f / (time + 1));
//			kgblogoimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, 1.f / ((time + 1) * 2));
			if (time < 8) kgblogoimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, 1.f - (time * (1.f / 8)));
			intro_blurimage.draw(D3DBLEND_ONE, D3DBLEND_ONE);

			flashimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, pow(1.f - fmod(time, 1.f), 1.5f));

			if (time > 240) {
				float t = (time - 240) / (255 - 240);
				fadeimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, t);
			}

			device->EndScene();
			if (device->Present(0, 0, 0, 0) == D3DERR_DEVICELOST) throw std::string("Lost device.");
			last_time = time;

			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (msg.message == WM_QUIT) done = true;
				if (msg.message == WM_KEYDOWN) {
					printf("time %f\n", time);
//					printf("time %f\n", fps_timer.get_time());
					flash_time = time;
				}
			}

			if (GetAsyncKeyState(VK_ESCAPE)) done = true;
			if (time > 257) done = true;

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
