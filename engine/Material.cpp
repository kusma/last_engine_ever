#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
#include <stdio.h>
extern IDirect3DDevice9 *device;

#include "Material.h"

using namespace engine;

// this is the default material
Material engine::default_material;

Material::Material() :	emissive(Color(0,0,0,0)),
						ambient(Color(0.1f,0.1f,0.1f)),
						diffuse(Color(1,1,1,1)),
						specular(Color(0,0,0,0)),
						specular_power(1),
						shinyness(0),
						texture(0),
						envmap(0),
						doublesided(false),
						lit(true),
						state(0),
						srcblend(D3DBLEND_ONE),
						dstblend(D3DBLEND_ZERO) {
}

Material::~Material() {
	if (state) state->Release();
}

void Material::update() {
	if (state) state->Release();
	device->BeginStateBlock();

	if (srcblend == D3DBLEND_ONE && dstblend == D3DBLEND_ZERO) {
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	} else {
		device->SetRenderState(D3DRS_SRCBLEND, srcblend);
		device->SetRenderState(D3DRS_DESTBLEND, dstblend);
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	}

	if (doublesided) device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	else device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	if (!lit) device->SetRenderState(D3DRS_LIGHTING, false);

	device->SetRenderState(D3DRS_SPECULARENABLE, ((specular.r != 0.f) || (specular.g != 0.f) || (specular.b != 0.f)));

	device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	device->SetRenderState(D3DRS_ZWRITEENABLE, true);

	device->SetRenderState(D3DRS_NORMALIZENORMALS, false);

	device->SetRenderState(D3DRS_COLORVERTEX, false);
	device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);


	unsigned ts = 0;
	if (texture) {
		device->SetTextureStageState(ts, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
		device->SetTextureStageState(ts, D3DTSS_COLOROP, D3DTOP_MODULATE);
		device->SetTextureStageState(ts, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		device->SetSamplerState(ts, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		device->SetSamplerState(ts, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		device->SetTexture(ts, texture->texture);
		ts++;
	}

	if (envmap) {
		device->SetTextureStageState(ts, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
		device->SetTextureStageState(ts, D3DTSS_COLOROP, D3DTOP_ADD);
		device->SetTextureStageState(ts, D3DTSS_ALPHAOP, D3DTOP_ADD);
		device->SetSamplerState(ts, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		device->SetSamplerState(ts, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		device->SetTexture(ts, envmap->texture);
		ts++;
	}

	// disable the last texturestage
	device->SetTextureStageState(ts, D3DTSS_COLOROP, D3DTOP_DISABLE);
	device->SetTextureStageState(ts, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	device->SetTexture(ts, 0);

	device->EndStateBlock(&state);
}

void Material::set( bool ambient, bool diffuse ) {

	D3DMATERIAL9 mat;
	memset(&mat, 0, sizeof(D3DMATERIAL9));

	if (ambient) {
		mat.Ambient.r = this->ambient.r;
		mat.Ambient.g = this->ambient.g;
		mat.Ambient.b = this->ambient.b;

		mat.Emissive.r = emissive.r;
		mat.Emissive.g = emissive.g;
		mat.Emissive.b = emissive.b;
	}

	if (diffuse) {
		mat.Diffuse.r = this->diffuse.r;
		mat.Diffuse.g = this->diffuse.g;
		mat.Diffuse.b = this->diffuse.b;

		mat.Specular.r = specular.r * shinyness;
		mat.Specular.g = specular.g * shinyness;
		mat.Specular.b = specular.b * shinyness;

		mat.Power = specular_power;
		device->SetRenderState(D3DRS_SPECULARENABLE, shinyness != 0.f);
	} else {
		device->SetRenderState(D3DRS_SPECULARENABLE, false);
	}

	device->SetMaterial( &mat );
	if (!state) update();
	state->Apply();
}
