#ifndef MESH_PROXY_H
#define MESH_PROXY_H

#include "Mesh.h"
#include "ResourceProxy.h"
#include "MaterialProxy.h"

namespace engine {
	class MeshProxy : public ResourceProxy<Mesh> {
	private:
		Mesh* read_from_file(std::string filename);
		MaterialProxy material_proxy;
	};
}

#endif // MESH_PROXY_H
