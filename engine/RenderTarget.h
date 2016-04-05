#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
#include <string>

namespace engine {
	class RenderTarget {
	public:
		RenderTarget( IDirect3DSurface9 *surface=0 ) : surface(surface), prev_surface(0), index(0) {}

		~RenderTarget() {
			if (surface) surface->Release();
		}

		void set( unsigned index = 0 ) {
			if (prev_surface) throw std::string("RenderTarget already set");
			if (surface) {
				this->index = index;
				device->GetRenderTarget(index, &prev_surface);
				device->SetRenderTarget(index, surface);
			}
		}

		IDirect3DSurface9* get_surface() { return surface; }

		void unset() {
			if (prev_surface) {
				device->SetRenderTarget(index, prev_surface);
				prev_surface->Release();
				prev_surface = 0;
			}
		}

	protected:
		unsigned index;
		IDirect3DSurface9 *surface;
		IDirect3DSurface9 *prev_surface;
	};
}

#endif // RENDERTARGET_H
