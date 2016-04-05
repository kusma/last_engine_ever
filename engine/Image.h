#ifndef IMAGE_H
#define IMAGE_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>

#include "Vertex2D.h"
#include "Matrix.h"

namespace engine {
	class Image {
	public:
		Image::Image( Texture *texture, float size = 1.0f );
		~Image();

		float left, top, width, height;

		void draw(unsigned srcblend=D3DBLEND_ONE, unsigned dstblend=D3DBLEND_ZERO, float alpha = 1.f);
		Texture* get_texture() { return texture; }
		IDirect3DVertexBuffer9* get_vertex_buffer() { return vertex_buffer; }

	private:
		Texture *texture;
		IDirect3DVertexBuffer9 *vertex_buffer;
	};
}

#endif // IMAGE_H
