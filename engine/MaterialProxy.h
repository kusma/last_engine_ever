#ifndef MATERIAL_PROXY_H
#define MATERIAL_PROXY_H

#include "Material.h"
#include "ResourceProxy.h"
#include "TextureProxy.h"

namespace engine{
	class MaterialProxy : public ResourceProxy<Material> {
	private:
		Material* read_from_file(std::string filename);
		TextureProxy texture_proxy;
	};
}

#endif // MATERIAL_PROXY_H
