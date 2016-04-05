#ifndef SUBMESH_H
#define SUBMESH_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
#include <vector>

#include "Material.h"

namespace engine {

	class SubMesh {
	public:
		SubMesh(Material *mat = &default_material);
		~SubMesh();

		void draw();
		void set_material( Material *material );
		void update();

		std::vector<unsigned short> indices;

		Material *material;
	private:
		unsigned short min_vertex;
		unsigned short max_vertex;
		IDirect3DIndexBuffer9 *indexbuffer;
	};

}

#endif // SUBMESH_H
