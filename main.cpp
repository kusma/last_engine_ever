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

Quaternion old_rot[26];
Quaternion new_rot[26];

Scene *fjallscene;
Vector axis(0, 0, 0);

void recalc_speeds(bool long_spin = false) {
	axis = Vector(0, 0, 0);
	((float*)&axis)[rand() % 3] = 1.f * ((rand() & 1) * 2 - 1);

	bool both = !(rand() & 1);
	both = true;

	int rot_speed_left = 0;
	int rot_speed_right = 0;
	if (long_spin) {
		while (rot_speed_left == 0) rot_speed_left = (rand() % 5) - 2;
		while (rot_speed_right == 0) rot_speed_right = (rand() % 5) - 2;
	} else {
		while (rot_speed_left == 0) rot_speed_left = (rand() % 3) - 1;
		while (rot_speed_right == 0) rot_speed_right = (rand() % 3) - 1;
	}


	Quaternion left_quat = Quaternion(axis, float(M_PI * 0.5f * rot_speed_left));
	Quaternion right_quat = Quaternion(axis, float(M_PI * 0.5f * rot_speed_right));

	for (unsigned i=0; i<26; i++) {
		Object *obj = fjallscene->objects[i];
		Matrix m = obj->get_absolute_matrix() * obj->get_pivot_matrix();
		Vector dir = m.transform_vector(fjallscene->objects[i]->get_mesh()->center_of_mass).normalize();

		old_rot[i] = obj->prs.rotation;
		new_rot[i] = obj->prs.rotation;

		bool taken = false;
		if (dir.dot(axis) >= 0.01f) {
			new_rot[i] = obj->prs.rotation * left_quat;
			taken = true;
		}
		if (both && !taken && (dir.dot(axis) <= -0.01f)) new_rot[i] = obj->prs.rotation * right_quat;
	}
}


#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

LinearKey<float> rot_keys[] = {
	LinearKey<float>(0.f, 0.f),
	LinearKey<float>(64.f, 0.f),
/*	LinearKey<float>(76.f, 12.f),
	LinearKey<float>(76.25f,14.f),
	LinearKey<float>(76.25f,15.f),
	LinearKey<float>(76.5f,16.f),
	LinearKey<float>(76.5f,17.f),
	LinearKey<float>(77.f, 18.f),
	LinearKey<float>(78.f, 14.f),
	LinearKey<float>(80.f, 16.f), */
	LinearKey<float>(152, 152-64),
	LinearKey<float>(152+8, 152-64),
	LinearKey<float>(256.f, 256.f-64-8),
};

float timetable[] = { 0, 1, 5, 6, 10, 14, 16, 24, 25, 28, 30, 42, 46, 76, 76.5, 77, 84, 92, 92.5, 93, 110, 112, 128, 129, 162, 164, 178, 180, 198, 199, 208, 220, 221, 223, 224, 256 };
unsigned time_index = 0;

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

		LinearCurve<float> rot_curve(rot_keys, ARRAY_SIZE(rot_keys));

		// load music
		BassPlayer player(win.get_window());
		player.load(std::string("rewyg.mp3"), 0 * (60.f / BPM));

		// images
		Image flashimage(texture_proxy.get_resource("white.png"));
		Image fadeimage(texture_proxy.get_resource("black.png"));
		Image kgblogoimage(texture_proxy.get_resource("kgblogo.png"));
		Image overlay(texture_proxy.get_resource("overlay.png"));
		Image overlay2(texture_proxy.get_resource("overlay2.png"));
		Image end(texture_proxy.get_resource("end.png"));

		Image text[] = {
			texture_proxy.get_resource("text0.png"),
			texture_proxy.get_resource("text1.png"),
			texture_proxy.get_resource("text2.png"),
			texture_proxy.get_resource("text3.png"),
			texture_proxy.get_resource("text4.png"),
			texture_proxy.get_resource("text5.png"),
			texture_proxy.get_resource("text6.png"),
			texture_proxy.get_resource("text7.png"),
			texture_proxy.get_resource("text8.png"),
			texture_proxy.get_resource("text9.png"),
		};

		unsigned current_text = 0;
		for (unsigned i=0; i<ARRAY_SIZE(text); i++){
			text[i].width = 0.6f;
			text[i].height = 0.3f * (4.f / 3);
			text[i].left = -0.4f;
			text[i].top =  -0.5f;
		}

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
		fjallscene->fog_near = 60;
		fjallscene->fog_far = 400;
		fjallscene->fog_color = Color(0.05f, 0.01f, 0.01f);
		fjallscene->fog = true;
		cam.zfar = 2000;

		// randomize demo :)
		srand( GetTickCount() );
		bool spinning = false;

		Timer fps_timer;
		player.play();
		bool done = false;
		while (!done) {
			float time = float(player.get_time() * (double(BPM) / 60));
			static float last_time = 0.f;
			float delta_time = time - last_time;

			if (time_index < ARRAY_SIZE(timetable)) {
				while (timetable[time_index] < time) time_index++;
			}

			player.update_spectrum();
			device->BeginScene();

			if (time < 256) {
				float text_alpha = 0.f;

				if (time > 96) {
					current_text = unsigned((time - 96) / 16) % ARRAY_SIZE(text);
					float itime = fmodf(time, 16);
					float t0 = itime;
					float t1 = 16 - itime;
					float t = min(t0, t1);

					text_alpha = t;
					fjallscene->objects[27]->get_mesh()->submeshes[0].material->texture = text[current_text].texture;
					fjallscene->objects[27]->get_mesh()->submeshes[0].material->update();
				}
				fjallscene->objects[27]->get_mesh()->submeshes[0].material->diffuse.a = text_alpha;

				cam.fov = 80 + player.sample_spectrum(0.1f) * 300;
				if (time_index & 1) { //(unsigned(time)>>3)&1) {
					cam.prs.position = Vector(-sin(time*0.3f)*80, cos(time*0.3f)*80, cos(-time*0.4f)*20);
				} else {
					cam.prs.position = Vector(sin(time*0.3f)*100, cos(time*0.7f)*40, cos(-time*0.3f)*60);
				}

				light1.prs.position = Vector(
					float(sin(-time * 0.7) * 90),
					float(sin(time * 0.7) * 90),
					float(cos(time * 0.7) * 90));

				light2.prs.position = Vector(
					float(sin(-time * 0.7 + M_PI) * 90),
					float(cos(time * 0.6) * 70),
					float(cos(time * 0.7 + M_PI) * 70));

				float rot_val = rot_curve.get_value(time);
				static float shake_stop_time = 0.f;
				if (fmod(rot_val, 2.f) < 1.f) {
					if (!spinning) {
						recalc_speeds();
						spinning = true;
					}

					// rotate
					for (unsigned i=0; i<26; i++) fjallscene->objects[i]->prs.rotation = old_rot[i].slerp(new_rot[i], fmod(rot_val, 1.f));
				} else {
					if (spinning) {
						spinning = false;
						shake_stop_time = time;
					}

					// shake
					float t = 1.f - (time - shake_stop_time);
					if (t < 0) t = 0;

					Vector offs = axis * sin((t-1)*705) * pow(t, 1.8f) * 0.3f;
					for (unsigned i=0; i<26; i++) fjallscene->objects[i]->prs.rotation = offs;
				}

				if (time > 160) {
					srand(unsigned(player.sample_spectrum(0.4f) * 50) + unsigned(time * 2));
					for (unsigned i=0; i<26; i++) {
						fjallscene->objects[i]->get_mesh()->submeshes[0].material = (float(rand()) / RAND_MAX) > 0.75f ? light : dark;
					}
					srand(GetTickCount());
				}

				if (time > 96) fjallscene->objects[26]->prs.position = Vector(0, fmodf(time, 1) * 400 - 200, 0);

				{
					Vector forward = (fjallscene->objects[27]->prs.position - cam.prs.position).normalize();
					Vector axis = forward.cross(Vector(0, 0, 1)).normalize();
					Vector up = forward.cross(Vector(0, 1, 0).cross(forward)).normalize();
					float angle = acos(forward.dot(Vector(0, 0, 1)));

//					fjallscene->objects[27]->prs.rotation = Quaternion(axis, angle);
//					fjallscene->objects[27]->prs.rotation.normalize();
				}

				render_texture.set();
				fjallscene->draw(false);

				device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
				flashimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, 0.25f);
				device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
				render_texture.unset();

				blurtexture1.do_blur(true,	1.0f, 1.5f);
				blurtexture2.do_blur(false,	1.0f, 1.f);
				blurtexture3.do_blur(true,	1.5f, 1.f);
				blurtexture4.do_blur(false,	1.5f, 1.f);
				blurtexture5.do_blur(true,	2.0f, 1.25f);
				blurtexture6.do_blur(false,	2.0f, 1.4f);

				fjallscene->draw(false);

				intro_blurimage.draw(D3DBLEND_ONE, D3DBLEND_ONE);
				overlay.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, min(player.sample_spectrum(0.3f) * 50, 0.4f));
				overlay2.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, min(player.sample_spectrum(0.15f) * 25, 0.2f));

				if (time >= 16 && (time < 152 || time > 160)) flashimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, pow(1.f - fmod(time, 1.f), 1.5f));
				if (time_index < ARRAY_SIZE(timetable)) {
					if ((time < 75 || time > 80) && (time < 91 || time > 96)) {
						float start_time = -FLT_MAX;
						if (time > 0) start_time = timetable[time_index - 1];
						float end_time = timetable[time_index];
						float t0 = time - start_time;
						float t1 = end_time - time;
						float t = min(t0 * 1.75f, t1 * 1.75f);
						if (t > 0.001f) fadeimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, 1 - t);
					}
				}

				if (time < 8) {
					kgblogoimage.width = (3.f / 4.f) * (5.f / (time + 1));
					kgblogoimage.height = 1.f * (5.f / (time + 1));
					kgblogoimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, 1.f - (time * (1.f / 8)));
				}

				if (time > 240) {
					float t = (time - 240) / (255 - 240);
					fadeimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, t);
				}

				if (text_alpha > 0.f) text[current_text].draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, text_alpha);
			} else end.draw(D3DBLEND_SRCALPHA, D3DBLEND_ZERO, min(time-256, 264-time));

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
