#ifndef TEXTURE_PROXY_H
#define TEXTURE_PROXY_H

#include "Texture.h"
#include "ResourceProxy.h"

namespace engine{
	class TextureProxy : public ResourceProxy<Texture> {
	private:
		Texture* read_from_file(std::string filename);
	};
}

#endif // TEXTURE_PROXY_H
