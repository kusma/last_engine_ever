#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>

#include "BlurTexture.h"
#include "Vertex2D.h"
#include "Matrix.h"
#include <string>

using namespace engine;
extern IDirect3DDevice9 *device;

BlurTexture::BlurTexture( Texture *texture, unsigned width, unsigned height ) : RenderTexture(width, height), blur_texture(texture), vertex_buffer(0) {
	if (!texture) throw std::string("blur with no texture? how useful is that, booy?");

	D3DSURFACE_DESC desc;
	blur_texture->texture->GetLevelDesc(0, &desc);

	float w_nudge = 0.5f / desc.Width;
	float h_nudge = 0.5f / desc.Height;

	offset_u = w_nudge*2;
	offset_v = h_nudge*2;

//	if (device->CreateVertexBuffer(sizeof(Vertex2D)*4*(1+BLUR_SIZE), D3DUSAGE_DYNAMIC, VERTEX2D_TYPE, D3DPOOL_DEFAULT, &vertex_buffer, 0) != D3D_OK)
	if (device->CreateVertexBuffer(sizeof(Vertex2D)*4, 0, VERTEX2D_TYPE, D3DPOOL_MANAGED, &vertex_buffer, 0) != D3D_OK)
		throw std::string("failed to create vertex buffer");

	Vertex2D *dst;
	vertex_buffer->Lock( 0, 0, (void**)&dst, 0);
	dst[0] = Vertex2D(Vector(-1, -1, 0), Uv(w_nudge, 1 + h_nudge));
	dst[1] = Vertex2D(Vector(-1,  1, 0), Uv(w_nudge, h_nudge));
	dst[2] = Vertex2D(Vector( 1,  1, 0), Uv(1 + w_nudge, h_nudge));
	dst[3] = Vertex2D(Vector( 1, -1, 0), Uv(1 + w_nudge, 1 + h_nudge));
	vertex_buffer->Unlock();
}

BlurTexture::~BlurTexture() {
	if (vertex_buffer) vertex_buffer->Release();
}


#define BLUR_SIZE 2

void BlurTexture::do_blur( bool horisontal, float step, float brightness ) {
	set();
	for (unsigned i=0; i<2; i++) {
		device->SetTexture(i, blur_texture->texture);
		device->SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
		device->SetTextureStageState(i, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		device->SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, 0);
		device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}
	device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	Matrix identity;
	identity.identity();
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&identity);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&identity);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&identity);

	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device->SetRenderState(D3DRS_LIGHTING, false);


	float alpha = brightness / (0.5f + (BLUR_SIZE * 2));
	unsigned color = 0xFFFFFF | ((int)(alpha * 128.f) << 24);
	device->SetRenderState(D3DRS_TEXTUREFACTOR, color);
	device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
	device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

	device->SetFVF(VERTEX2D_TYPE);
	device->SetStreamSource(0, vertex_buffer, 0, sizeof(Vertex2D));

	device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_LERP);
	device->SetTextureStageState(1, D3DTSS_COLORARG0, D3DTA_TFACTOR);
	device->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	device->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	device->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
	device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	device->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);

	color = 0x7F7F7F | ((int)(2*alpha * 255.f) << 24);
	device->SetRenderState(D3DRS_TEXTUREFACTOR, color);

	for (unsigned i=1; i<(BLUR_SIZE+1); i++) {
		Matrix mat;
		mat.identity();

		float trans;
		if (horisontal) trans = i*2*offset_u*step+0.5f*offset_u-offset_u;
		else trans = i*2*offset_v*step+0.5f*offset_v-offset_v;

		if (horisontal) mat.matrix[2][0] = trans;
		else mat.matrix[2][1] = trans;

		device->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
		device->SetTransform(D3DTS_TEXTURE0, (D3DMATRIX*)&mat);

		if (horisontal) mat.matrix[2][0] = -trans;
		else mat.matrix[2][1] = -trans;

		device->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
		device->SetTransform(D3DTS_TEXTURE1, (D3DMATRIX*)&mat);
		device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	}

	unset();
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	device->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	device->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
}
