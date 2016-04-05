#ifndef GRID_H
#define GRID_H

#include "Vertex.h"

#include <math.h>
#ifndef M_PI
#define M_PI 3.1415
#endif

namespace engine {
	class Grid {
	public:
		Grid(unsigned w = 640/8, unsigned h = 480/8) : w(w), h(h), vertex_buffer(0), index_buffer(0) {
			xscale = 2.f / w;
			yscale = 2.f / h;
			vertices.resize((w + 1) * (h + 1));
			if (FAILED(device->CreateVertexBuffer(sizeof(Vertex2D) * (w + 1) * (h + 1), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, VERTEX2D_TYPE, D3DPOOL_DEFAULT, &vertex_buffer, 0)))
				throw std::string("failed to create grid-vertexbuffer");

			if (FAILED(device->CreateIndexBuffer(sizeof(unsigned short) * 2 * 3 * w * h, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &index_buffer, 0)))
				throw std::string("failed to create grid-indexbuffer");

			unsigned short *index_dest;
			if (index_buffer->Lock(0, 0, (void**)&index_dest, 0) != D3D_OK)
				throw std::string("failed to click-(c)lock grid-indexbuffer");
			for (unsigned y=0; y<h; y++) {
				for (unsigned x=0; x<w; x++) {
					*index_dest++ = x + y * (w + 1);
					*index_dest++ = x + 1 + y * (w + 1);
					*index_dest++ = x + (y + 1) * (w + 1);

					*index_dest++ = x + 1 + y * (w + 1);
					*index_dest++ = x + 1 + (y + 1) * (w + 1);
					*index_dest++ = x + (y + 1) * (w + 1);
				}
			}
			index_buffer->Unlock();
			reset();
			update();
		}

		~Grid() {
			if (vertex_buffer) vertex_buffer->Release();
			if (index_buffer) index_buffer->Release();
		}

		void reset() {	
			Vertex2D *vertex_dest = &vertices[0];
			float inv_w = 2.f / (w - 1);
			float inv_h = 2.f / (h - 1);
			for (unsigned y=0; y < (h + 1); y++) {
				for (unsigned x=0; x < (w + 1); x++) {
					float u = x * inv_w;
					float v = y * inv_h;
					vertex_dest->pos = Vector(u - 1, v - 1, 0.f);
					vertex_dest->uv = Uv(u * 0.5f, 1 - v * 0.5f);
					vertex_dest->color = 0xFFFFFFFF;
					vertex_dest++;
				}
			}
		}

		void update() {
			Vertex2D *vertex_dest;
			if (IDirect3DVertexBuffer9_Lock(vertex_buffer, 0, 0, (void**)&vertex_dest, 0) != D3D_OK) throw std::string("failed to lock grid-vertexbuffer");
			memcpy(vertex_dest, &vertices[0], sizeof(Vertex2D) * (w + 1) * (h + 1));
			vertex_buffer->Unlock();			
		}

		void wave(float xpos, float ypos, float size, float power) {
			size = 1.f/size;
			const float size_squared = size*size;
/*
			xpos += 1;
			ypos += 1;
*/
			Vertex2D *v = &vertices[0];
			for (unsigned y=0; y < (h + 1); y++) {
				for (unsigned x=0; x < (w + 1); x++) {
					float cx = float(int(x) - int(w >> 1)) * xscale - xpos;
					float cy = float(int(y) - int(h >> 1)) * yscale - ypos;

					float dist = cx * cx + cy * cy;

					if ((dist * size_squared) < (M_PI * M_PI)) {
						dist = (float)sqrt(dist) * size;
						float mul = (float)(1 + cos(dist)) * power;
						v->uv.u += cx * mul;
						v->uv.v += cy * mul;
					}
					v++;
				}
			}
		}

		void draw(Texture *texture,  unsigned srcblend = D3DBLEND_ONE, unsigned dstblend = D3DBLEND_ZERO, float alpha = 1.f) {
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

			device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
			device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
			device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

			Matrix identity;
			identity.identity();
			device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&identity);
			device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&identity);
			device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&identity);

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
			device->SetIndices(index_buffer);
			device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, (w + 1) * (h + 1), 0, w * h * 2);
		}

	private:
		unsigned w, h;
		float xscale, yscale;
		std::vector<Vertex2D> vertices;
		IDirect3DVertexBuffer9 *vertex_buffer;
		IDirect3DIndexBuffer9 *index_buffer;
	};
}

#endif // GRID_H