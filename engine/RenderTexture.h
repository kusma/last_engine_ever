#ifndef RENDERTEXTURE_H
#define RENDERTEXTURE_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
extern IDirect3DDevice9 *device;

#include "Texture.h"
#include "RenderTarget.h"

namespace engine {
	class RenderTexture : public Texture, public RenderTarget {
	public:
		RenderTexture( unsigned width, unsigned height, int usage = D3DUSAGE_RENDERTARGET, D3DPOOL pool = D3DPOOL_DEFAULT ) {
			if (device->CreateTexture(width, height, 0, usage, D3DFMT_A8R8G8B8, pool, &texture, 0) != D3D_OK)
				throw std::string("failed to create render-texture");
			if (texture->GetSurfaceLevel(0, &this->surface) != D3D_OK)
				throw std::string("failed to get surface from render-texture");
		}
	};
}

#endif // RENDERTEXTURE_H
