#include "rayofshite.h"
#include "engine/textureproxy.h"
#include "engine/image.h"
#include "engine/mesh.h"

#define frand (float(rand()&4095)/4095.f)
#define frand2 (frand*2.f-1.f)

struct particle
{
	float speed;
	float position, iposition;
	float life, birth;
};

static engine::TextureProxy* texture_proxy;
static engine::Texture *wise;
static engine::Image* wiseImage;
static engine::Texture *sunTex;
static engine::SimpleMesh* dopeSun;
const int numRays = 200;
static particle* Rays;

static engine::Matrix identity;

void InitSuperMagicalBukakkeRayofShite()
{
	texture_proxy = new engine::TextureProxy();
	wise = texture_proxy->get_resource("wise_guy_darken.jpg");
	wiseImage = new engine::Image(wise);
	sunTex = texture_proxy->get_resource("argh.png");
	dopeSun = new engine::SimpleMesh();
	Rays = new particle[numRays];

	int numVertices = 3*numRays;
	dopeSun->vertices.resize(numVertices);

	identity.identity();

	Rays = new particle[numRays];

	float maxspeed = 0.7f;
	for (int i = 0; i < numRays; i++)
	{
		Rays[i].iposition = 0;
		Rays[i].birth = 0;
		Rays[i].speed = frand2 * maxspeed;
		Rays[i].position = 0;
		Rays[i].life = 0;
	}
	srand(1664);
}

void PlaySuperMagicalBukakkeRayofShite(float time)
{
			device->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(0, 0, 0));
			device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
			device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
//			device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);

//			device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
			device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
			device->SetRenderState(D3DRS_LIGHTING, false);

			device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
//			device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
			device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
			device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
/*			device->SetRenderState(D3DRS_ZWRITEENABLE, false);
			device->SetRenderState(D3DRS_ZENABLE, false); */
/*
			device->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);
			device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
*/
//			wiseImage->draw();

			device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
			device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
			device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

			device->SetRenderState(D3DRS_ZWRITEENABLE, true);
			device->SetRenderState(D3DRS_ZENABLE, true);
			device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

			engine::Matrix projection_matrix;
			projection_matrix.project( 80, 4.f/3.f, 1, 1000 );
			device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&projection_matrix);

			engine::Matrix view_matrix;
			view_matrix.translate( engine::Vector(3, -1, 10.f) );
			device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&view_matrix);

			engine::Matrix world_matrix;
			world_matrix.rotate( engine::Vector(0.4f, 0.3f, 0) );
			device->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&world_matrix );

			device->SetTexture(0, sunTex->texture);
			device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			device->SetRenderState(D3DRS_COLORVERTEX, TRUE);
			device->SetRenderState(D3DRS_ZWRITEENABLE, false);
			device->SetRenderState(D3DRS_ZENABLE, false);
			device->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);


			float speed = 4.f;
			for (int i = 0; i < numRays; i++)
			{
				Rays[i].position = Rays[i].iposition + (time-Rays[i].birth) * Rays[i].speed * speed;
				if (Rays[i].life < (time-Rays[i].birth)*speed)
				{
					Rays[i].birth = time;
					Rays[i].life = 0.5f + frand * 3.f;
					Rays[i].speed = -Rays[i].speed;
					Rays[i].iposition = Rays[i].position;
				}
			}

			const float pi = 3.14159f;
			srand(numRays);

			for (int ray = 0; ray < numRays; ray++)
			{
				float fray = 2.0f*pi*float(ray)/float(numRays);
				float frayNext = 2.0f*pi*float(ray+10.0f)/float(numRays);

				int raySize = 9.0f + frand2 * 5.f;

				dopeSun->vertices[ray*3+0].position.x = 0;
				dopeSun->vertices[ray*3+0].position.y = 0;
				dopeSun->vertices[ray*3+0].position.z = 0;
				dopeSun->vertices[ray*3+0].color = 0xffffffff;
				dopeSun->vertices[ray*3+0].uv.u = 0.f;
				dopeSun->vertices[ray*3+0].uv.v = 0.f;

				fray += Rays[ray].position;
				frayNext += Rays[ray].position;

				dopeSun->vertices[ray*3+1].position.x = raySize * (cosf(fray) - sinf(fray));
				dopeSun->vertices[ray*3+1].position.y = raySize * (sinf(fray) + cosf(fray));
				dopeSun->vertices[ray*3+1].position.z = 0;
				dopeSun->vertices[ray*3+1].color = 0x00ffffff;
				dopeSun->vertices[ray*3+1].uv.u = 1.0f;
				dopeSun->vertices[ray*3+1].uv.v = 0.f;


				dopeSun->vertices[ray*3+2].position.x = raySize * (cosf(frayNext) - sinf(frayNext));
				dopeSun->vertices[ray*3+2].position.y = raySize * (sinf(frayNext) + cosf(frayNext));
				dopeSun->vertices[ray*3+2].position.z = 0;
				dopeSun->vertices[ray*3+2].color = 0x00ffffff;
				dopeSun->vertices[ray*3+2].uv.u = 0.0f;
				dopeSun->vertices[ray*3+2].uv.v = 1.0f;
			}

			dopeSun->update();
			dopeSun->draw();
}

void FreeSuperMagicalBukakkeRayofShite()
{
	delete [] Rays;
	delete dopeSun;
	delete wiseImage;
	delete texture_proxy;
}

