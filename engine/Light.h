#ifndef LIGHT_H
#define LIGHT_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>

#include "Entity.h"
#include "Color.h"

namespace engine {
	class Light : public Entity {
	public:
		Light( Entity *parent = 0 );

		void set( unsigned index );

		Color ambient;
		Color diffuse;
		Color specular;

	private:
		D3DLIGHT9 light;
	};
}

#endif // LIGHT_H
