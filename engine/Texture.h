#ifndef TEXTURE_H
#define TEXTURE_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>

namespace engine {
	class Texture {
	public:
		Texture() : texture(0) {}

		~Texture() {
			if (texture) texture->Release();
		}

		IDirect3DTexture9 *texture;
	};
}

#endif // TEXTURE_H
