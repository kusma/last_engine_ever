#ifndef BLUR_TEXTURE_H
#define BLUR_TEXTURE_H

#include "RenderTexture.h"

namespace engine {
	class BlurTexture : public RenderTexture {
	public:
		BlurTexture( Texture *texture, unsigned width = 256, unsigned height = 256 );
		~BlurTexture();

		void do_blur( bool horisontal, float step = 1.f, float brightness = 1.f );
	private:
		float offset_u;
		float offset_v;
		Texture *blur_texture;
		IDirect3DVertexBuffer9 *vertex_buffer;
	};
}

#endif // BLUR_TEXTURE_H