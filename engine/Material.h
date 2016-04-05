#ifndef MATERIAL_H
#define MATERIAL_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>

#include "Color.h"
#include "Texture.h"

namespace engine {

	class SubMesh;
	class Material {
	public:
		Material();
		~Material();
		void set( bool ambient = true, bool diffuse = true );
		void update();

		Color emissive;
		Color ambient;
		Color diffuse;

		Color specular;
		float specular_power;
		float shinyness;

		Texture *texture;
		Texture *envmap;

		bool doublesided;
		bool lit;

		unsigned srcblend;
		unsigned dstblend;

		IDirect3DStateBlock9 *state;
	};

	extern Material default_material;
}

#endif // MATERIAL_H
