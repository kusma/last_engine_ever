#ifndef RADIAL_BLUR_TEXTURE_H
#define RADIAL_BLUR_TEXTURE_H

#include "RenderTexture.h"


namespace engine {
	class RadialBlurTexture : public RenderTexture {
	public:
		RadialBlurTexture( Texture *texture, unsigned width = 256, unsigned height = 256 );
		~RadialBlurTexture();

		void do_blur( float rotation, float distance, float alpha );
	private:
		Texture *blur_texture[4];
		IDirect3DVertexBuffer9 *vertex_buffer;
	};
}

#endif // RADIAL_BLUR_TEXTURE_H