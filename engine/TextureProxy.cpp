#include "file/file.h"
#include "TextureProxy.h"

#include <d3dx9.h>

using namespace engine;
extern IDirect3DDevice9 *device;

Texture* TextureProxy::read_from_file(std::string filename) {
	file *fp = file_open(filename.c_str());
	if (!fp) throw std::string("file not found: ") + filename;

	Texture *temp = new Texture;
	if (FAILED(D3DXCreateTextureFromFileInMemory(device, fp->data, fp->size, &temp->texture))) throw std::string("failed to load ")+filename;
	file_close(fp);

	return temp;
}

std::map<Texture*, int> ResourceProxy<Texture>::ref_count_static;
std::map<std::string, Texture*> ResourceProxy<Texture>::filename_map_static;
