#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
extern IDirect3DDevice9 *device;

#include "Texture.h"
#include "Image.h"
#include <string>

using namespace engine;

Image::Image( Texture *texture, float size ) : texture(texture), vertex_buffer(0), left(0), top(0), width(1), height(1) {
	float w_nudge = 0.f;
	float h_nudge = 0.f;

	if (texture) {
		D3DSURFACE_DESC desc;
		texture->texture->GetLevelDesc(0, &desc);
		float w_nudge = 0.5f / desc.Width;
		float h_nudge = 0.5f / desc.Height;
	}

	if (device->CreateVertexBuffer(sizeof(Vertex2D) * 4, 0, VERTEX2D_TYPE, D3DPOOL_MANAGED, &vertex_buffer, 0) != D3D_OK)
		throw std::string("failed to create vertex buffer");

	Vertex2D *dst;
	vertex_buffer->Lock( 0, 0, (void**)&dst, 0);
	dst[0] = Vertex2D(Vector(-size, -size, 0), Uv(w_nudge, 1 + h_nudge));
	dst[1] = Vertex2D(Vector(-size, size, 0), Uv(w_nudge, h_nudge));
	dst[2] = Vertex2D(Vector(size, size, 0), Uv(1 + w_nudge, h_nudge));
	dst[3] = Vertex2D(Vector(size, -size, 0), Uv(1 + w_nudge, 1 + h_nudge));
	vertex_buffer->Unlock();
}

Image::~Image() {
	if (vertex_buffer) vertex_buffer->Release();
}

void Image::draw( unsigned srcblend, unsigned dstblend, float alpha ) {
	if (alpha < 0) alpha = 0;
	if (alpha > 1) alpha = 1;
	device->SetTexture(0, texture->texture);
	device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
	device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	device->SetRenderState(D3DRS_TEXTUREFACTOR, unsigned(alpha * 255.f) << 24);
	device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	Matrix identity;
	identity.identity();
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&identity);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&identity);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&identity);

	Matrix translate;
	Matrix scale;
	translate.translate(Vector(left, -top, 0));
	scale.scale(Vector(width, height, 1));
	translate *= scale;
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&translate);

	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device->SetRenderState(D3DRS_LIGHTING, false);

	if (srcblend == D3DBLEND_ONE && dstblend == D3DBLEND_ZERO) {
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	} else {
		device->SetRenderState(D3DRS_SRCBLEND, srcblend);
		device->SetRenderState(D3DRS_DESTBLEND, dstblend);
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	}

	device->SetFVF(VERTEX2D_TYPE);
	device->SetStreamSource(0, vertex_buffer, 0, sizeof(Vertex2D));
	device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

	device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}
