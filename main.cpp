#define BPM 150

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
#include "dt2003/common.h"

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

#include "rayofshite.h"

using namespace engine;

void setcam(Syncreader* sync, LookAtCamera& cam, Entity& lookat) {
	cam.prs.position.x = sync->get(tCamPosX);
	cam.prs.position.y = sync->get(tCamPosY);
	cam.prs.position.z = sync->get(tCamPosZ);

	lookat.prs.position.x = sync->get(tCamTarX);
	lookat.prs.position.y = sync->get(tCamTarY);
	lookat.prs.position.z = sync->get(tCamTarZ);

	if(sync->getbool(tCamRelative)) {
		lookat.prs.position.x += cam.prs.position.x;
		lookat.prs.position.y += cam.prs.position.y;
		lookat.prs.position.z += cam.prs.position.z;
	}
	
	cam.roll = (sync->get(tCamRoll) / 0x400) * M_PI*2;

}

float flash_time = -1000031337.f;
float flash_speed = 2.f;

int main(int argc, char *argv[]) {
	try {
		D3DWin win(THE_DEMOTITLE, 640, 480, D3DFMT_X8R8G8B8, true);
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

		engine::Texture *load_tex = texture_proxy.get_resource("load.jpg");
		engine::Image load_image(load_tex);
		device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);
		load_image.draw(D3DBLEND_ONE, D3DBLEND_ONE);
		if (device->Present(0, 0, 0, 0) == D3DERR_DEVICELOST) throw std::string("Lost device.");

		BassPlayer player(win.get_window());
		player.load(std::string("test.ogg"));

		Syncreader* sync;
		sync = new Syncreader();
		sync->load();
		win.setstuff(&player,sync);

		engine::Texture *flashtex = texture_proxy.get_resource("white.png");
		engine::Image flashimage(flashtex);

		engine::Texture *fadetex = texture_proxy.get_resource("black.png");
		engine::Image fadeimage(fadetex);

		engine::Texture *texture = texture_proxy.get_resource("lobstarrs.png");
		engine::Image image(texture);

		engine::Texture *attack_texture = texture_proxy.get_resource("attack.png");
		engine::Image attack(attack_texture);

		engine::Texture *mars_texture = texture_proxy.get_resource("mars.png");
		engine::Image mars_image(mars_texture);

		engine::Texture *nm_ski_texture = texture_proxy.get_resource("nm_ski.jpg");
		engine::Image nm_ski(nm_ski_texture);

		engine::Texture *dof1_texture = texture_proxy.get_resource("dof1.jpg");
		engine::Image dof1_image(dof1_texture);

		engine::Texture *dof2_texture = texture_proxy.get_resource("dof2.jpg");
		engine::Image dof2_image(dof2_texture);

		engine::Texture *end_texture = texture_proxy.get_resource("end.jpg");
		engine::Image end_image(end_texture);
		
		engine::Texture *lobstarrsimg = texture_proxy.get_resource("lobstarrs.jpg");
		engine::Image lobstarrsimg_image(lobstarrsimg);

		Texture *wiseguy1_texture = texture_proxy.get_resource("wise_guy.jpg");
		Texture *wiseguy2_texture = texture_proxy.get_resource("wise_guy2.jpg");
		Texture *wiseguy3_texture = texture_proxy.get_resource("wise_guy3.jpg");
		Texture *wiseguy4_texture = texture_proxy.get_resource("wise_guy4.jpg");
		Image wiseguy1(wiseguy1_texture);
		Image wiseguy2(wiseguy2_texture);
		Image wiseguy3(wiseguy3_texture);
		Image wiseguy4(wiseguy4_texture);

		Light light1, light2;

		engine::RenderTexture render_texture(256, 256);
		engine::BlurTexture blurtexture(&render_texture);
		engine::BlurTexture blurtexture2(&blurtexture);
		engine::Image dof_blurimage(&blurtexture2);
		engine::BlurTexture blurtexture3(&blurtexture2);
		engine::BlurTexture blurtexture4(&blurtexture3);
		engine::Image intro_blurimage(&blurtexture4);

		engine::BlurTexture blurtexture5(&blurtexture4);
		engine::BlurTexture blurtexture6(&blurtexture5);
		engine::Image blurimage(&blurtexture6);
		engine::Entity lookat;
		engine::LookAtCamera cam(&lookat);
		engine::LookAtCamera cam2;

		engine::Scene *fjallscene = scene_proxy.get_resource("intro_scene.scene");
		fjallscene->add(&cam);
		fjallscene->add(&cam2);

		engine::Scene *raimoball = scene_proxy.get_resource("raimoball.scene");
		raimoball->add(&cam);
		raimoball->add(&cam2);
		raimoball->add(&light1);
		raimoball->add(&light2);

		engine::Scene *spikeball = scene_proxy.get_resource("spikeball.scene");
		spikeball->add(&cam);
		spikeball->add(&light1);
		spikeball->add(&light2);

		engine::Scene *space = scene_proxy.get_resource("space.scene");
		space->add(&cam);

		engine::Scene *lauri = scene_proxy.get_resource("lauri.scene");
		lauri->add(&cam);
		lauri->add(&light1);

		engine::Scene *demon = scene_proxy.get_resource("demon.scene");
		demon->add(&cam);
		demon->add(&light1);

		Grid g;
		InitSuperMagicalBukakkeRayofShite();

		Timer fps_timer;
		player.play();
		bool done = false;

		HWND tracker;
		int oldrow = -1;
		int irow;
		int part;
		while (!done) {
			//			static float time = 0;
			float time = player.get_time() * (float(BPM) / 60);
			sync->update(time * 8.0f);
			irow = time * 8;
			if ((irow != oldrow) && (tracker = FindWindow(THE_TRACKERTITLE,THE_TRACKERTITLE))) {
				SendMessage(tracker,CM_MOVEPOS,irow,0);
				oldrow = irow;
			}

			player.update_spectrum();
			device->BeginScene();

			part = sync->get(tPart);

			if (part < 2) {
				
				cam.fov = 110 + player.sample_spectrum(0.6f) * 500;
				//cam.prs.position = engine::Vector( sin(time*0.5f)*120, sin(time*0.5f)*50, cos(time*0.5f)*120 );

				setcam(sync,cam,lookat);

				//cam2.fov = 90 + player.sample_spectrum(0.6f) * 500;
				//cam2.prs.position = engine::Vector( sin(time*0.30f)*70, cos(time*0.7f)*100, sin(-time*0.7f)*100 );

//				if (true) fjallscene->draw(unsigned(time / 4) & 1);

				render_texture.set();
				if(0 < part) {
					fjallscene->objects[0]->prs.rotation = Quaternion( Vector( time*0.25f, sin(-time*0.5f), -time ) );
				}
				fjallscene->draw(sync->get(tCamSelector)); //unsigned(time / 4) & 1);
				render_texture.unset();

				blurtexture.do_blur(true,	1, 1.0f);
				blurtexture2.do_blur(false,	1, 1.0f);
				blurtexture3.do_blur(true,	2, player.sample_spectrum(0.5f) * 5000);
				blurtexture4.do_blur(false,	2, player.sample_spectrum(0.5f) * 5000);
				intro_blurimage.draw(D3DBLEND_ONE, D3DBLEND_ZERO);
				
				if (0 < part) {
					attack.draw(D3DBLEND_DESTCOLOR, D3DBLEND_ZERO);
				}

			} else if (part < 5) {
				cam.fov = 110 + player.sample_spectrum(0.6f) * 500;
				//cam.prs.position = Vector( sin(time*0.5f)*130, sin(time*0.5f)*70, cos(time*0.5f)*130 );
				setcam(sync,cam,lookat);
 				light1.prs.position = Vector( sin(time*0.7f)*120*3, sin(time*0.35f)*70*3, cos(time*0.2f)*100*3 );
				light2.prs.position = Vector( sin(-time*0.75f)*120*3, -sin(time*0.5f)*70*3, cos(time*0.25f)*100*3 );
				raimoball->objects[0]->prs.rotation = Quaternion( Vector( time*0.05f, sin(-time*0.1f), -time*0.2f ) );
				raimoball->draw();
				render_texture.set();
 				raimoball->draw();
 				render_texture.unset();
 				blurtexture.do_blur(true,	1, 1.0f);
				blurtexture2.do_blur(false,	1, 1.0f);
 				blurtexture3.do_blur(true,	2, player.sample_spectrum(0.5f) * 100);
				blurtexture4.do_blur(false,	2, player.sample_spectrum(0.5f) * 200);
 				intro_blurimage.draw(D3DBLEND_ONE, D3DBLEND_ONE);

				if(2 < part) {
					wiseguy1.draw(D3DBLEND_DESTCOLOR, D3DBLEND_ZERO);
					wiseguy1.draw(D3DBLEND_DESTCOLOR, D3DBLEND_ONE);
				}
			}
			else if (part < 6) {
				lobstarrsimg_image.draw();
			} else if (part < 10) {

				//cam.prs.position = engine::Vector( sin(time*0.30f)*40, cos(time*0.7f)*50, sin(-time*0.7f)*50 );
				setcam(sync,cam,lookat);
				cam.fov = 100 + player.sample_spectrum(0.6f) * 500;
				light1.prs.position = Vector( sin(time*0.7f)*120*3, sin(time*0.35f)*70*3, cos(time*0.2f)*100*3 );
				light2.prs.position = Vector( sin(-time*0.75f)*120*3, -sin(time*0.5f)*70*3, cos(time*0.25f)*100*3 );

				spikeball->objects[0]->prs.rotation = Quaternion( Vector( -time*0.07f, sin(time*0.3f), -time*0.2f ) );
				spikeball->objects[1]->prs.rotation = Quaternion( Vector(0, 1, 0), time * (sync->get(tKwlInner) / 64.0f))
					* Quaternion( Vector(1, 0, 0), time * (sync->get(tKwlInnerR) / 64.0f));
				spikeball->objects[2]->prs.rotation = Quaternion( Vector(0, 1, 0), time * (sync->get(tKwlOuter) / 64.0f))
					* Quaternion( Vector(0, 0, 1), time * (sync->get(tKwlOuterR) / 64.0f));

				spikeball->draw();

				render_texture.set();
				spikeball->draw();
				render_texture.unset();

				blurtexture.do_blur(true,	1, 1.0f);
				blurtexture2.do_blur(false,	1, 1.0f);
				blurtexture3.do_blur(true,	2, 1.0f);
				blurtexture4.do_blur(false,	2, player.sample_spectrum(0.5f) * 5000);

/*				blurtexture5.do_blur(false,	3, 1.0f);
				blurtexture6.do_blur(true,	3, 1.5f); */
/*				g.reset();
				g.wave(sin(time)*0.5f, cos(time)*0.5f, 0.2f, 0.2f);
				g.wave(sin(time-1)*0.5f, cos(-time)*0.5f, 0.2f, 0.2f);
				g.update();
				g.draw(&blurtexture4, D3DBLEND_ONE, D3DBLEND_ONE); */
				intro_blurimage.draw(D3DBLEND_ONE, D3DBLEND_ONE);
			}
			else if (part < 15) {
				time *= 0.1f;
				cam.prs.position = engine::Vector( sin(time*0.30f)*80, cos(time*0.7f)*100, sin(-time*0.7f)*90 );
				cam.fov = 100;

				space->draw();
				PlaySuperMagicalBukakkeRayofShite(time);

				render_texture.set();
				space->draw();
				PlaySuperMagicalBukakkeRayofShite(time);
				render_texture.unset();

				blurtexture.do_blur(true,	1, 1.0f);
				blurtexture2.do_blur(false,	1, 1.0f);
				blurtexture3.do_blur(true,	2, 1.0f);
				blurtexture4.do_blur(false,	2, player.sample_spectrum(0.5f) * 100);
				intro_blurimage.draw(D3DBLEND_ONE, D3DBLEND_ONE);
				if(sync->getbool(tMarsShow)) {
					mars_image.draw(D3DBLEND_DESTCOLOR, D3DBLEND_ZERO);
				} else {
					fadeimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, 0.5f);
				}
			}
			else if (part < 20) {
				cam.prs.position = engine::Vector( sin(time*0.30f)*40, cos(time*0.7f)*50, sin(time*0.7f)*50 );
				cam.fov = 100;

				if (part < 17) {
					float fog_start = 10;
					float fog_end = 50;

					device->SetRenderState(D3DRS_FOGCOLOR, 0);
					device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
					device->SetRenderState(D3DRS_FOGSTART, *((DWORD*) (&fog_start)));
					device->SetRenderState(D3DRS_FOGEND, *((DWORD*) (&fog_end)));
					device->SetRenderState(D3DRS_FOGENABLE, true);

					lauri->draw();
					device->SetRenderState(D3DRS_FOGSTART, *((DWORD*) (&fog_end)));
					device->SetRenderState(D3DRS_FOGEND, *((DWORD*) (&fog_start)));

					render_texture.set();
					lauri->draw();
					render_texture.unset();

					device->SetRenderState(D3DRS_FOGENABLE, false);

					blurtexture.do_blur(true,	1, 1.0f);
					blurtexture2.do_blur(false,	1, 1.0f);
					dof_blurimage.draw(D3DBLEND_ONE, D3DBLEND_ONE);

					if (part < 16) dof1_image.draw(D3DBLEND_ONE, D3DBLEND_ONE);
					else dof2_image.draw(D3DBLEND_ONE, D3DBLEND_ONE);
				} else {
					lauri->draw();
					render_texture.set();
					lauri->draw();
					render_texture.unset();
					blurtexture.do_blur(true,	1, 1.0f);
					blurtexture2.do_blur(false,	1, 1.0f);
					blurtexture3.do_blur(true,	2, 1.0f);
					blurtexture4.do_blur(false,	2, player.sample_spectrum(0.5f) * 12000);
					intro_blurimage.draw(D3DBLEND_ONE, D3DBLEND_ONE);
				}
			} else if (false && (part < 25)) {
				cam.prs.position = engine::Vector( sin(time*0.30f)*40, cos(time*0.7f)*50, sin(time*0.7f)*50 );
				cam.fov = 100;
				demon->objects[0]->prs.rotation = Quaternion(Vector(1,0,0), M_PI/2);
				demon->draw();
				render_texture.set();
				demon->draw();
				render_texture.unset();
				blurtexture.do_blur(true,	1, 1.0f);
				blurtexture2.do_blur(false,	1, 1.0f);
				blurtexture3.do_blur(true,	2, 1.0f);
				blurtexture4.do_blur(false,	2, player.sample_spectrum(0.5f) * 100);
				intro_blurimage.draw(D3DBLEND_ONE, D3DBLEND_ONE);
			} else {
				device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);
				end_image.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, player.sample_spectrum(0.5f) * 500);
			}

/*
			blurtexture5.do_blur(false,	2, 1.0f);
			blurtexture6.do_blur(true,	2, 1.0f);
			blurimage.draw(D3DBLEND_ONE, D3DBLEND_ONE);
*/
//			image.draw(D3DBLEND_ZERO, D3DBLEND_SRCCOLOR);
//			image.draw(D3DBLEND_ONE, D3DBLEND_SRCCOLOR);
//			image.draw(D3DBLEND_ONE, D3DBLEND_ONE);

			if(sync->get_trig(tFlash)) {
				//flash_time = time;
			}

			if (1 - ((time - flash_time) / flash_speed) > 0) {
				flashimage.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, 1 - ((time - flash_time) / flash_speed));
			}

//			attack.draw(D3DBLEND_SRCALPHA, D3DBLEND_ONE, 0.5f+sin(time)*0.5f);

			device->EndScene();
			if (device->Present(0, 0, 0, 0) == D3DERR_DEVICELOST) throw std::string("Lost device.");

			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (msg.message == WM_QUIT) done = true;
				if (msg.message == WM_KEYDOWN) {
					flash_time = time;
				}
			}
			if (GetAsyncKeyState(VK_ESCAPE)) done = true;

#ifndef NDEBUG
			double time__ = fps_timer.get_time();
			fps_timer.reset();
			printf("FPS: %f\r", 1.f / time__);
#endif
		}
		FreeSuperMagicalBukakkeRayofShite();
	} catch (std::string reason) {
		MessageBox( NULL, reason.c_str(), NULL, MB_OK );
		return 1;
	}

	return 0;
}
