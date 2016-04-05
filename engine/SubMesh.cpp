#include "SubMesh.h"

using namespace engine;

extern IDirect3DDevice9 *device;

SubMesh::SubMesh(Material *mat) : material(mat), indexbuffer(0) {}

SubMesh::~SubMesh() {
	if (indexbuffer) indexbuffer->Release();
	indexbuffer = 0;
}

void SubMesh::draw() {
	if (indices.size() == 0) return;
	if (!indexbuffer) update();

	if (material) material->set();

	device->SetIndices(indexbuffer);
	device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, min_vertex, max_vertex-min_vertex, 0, UINT(indices.size() / 3));
}

void SubMesh::set_material( Material *material ) {
	this->material = material;
}

void SubMesh::update() {
	if (!indexbuffer && FAILED(device->CreateIndexBuffer(UINT(indices.size() * sizeof(unsigned short)), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &indexbuffer, 0)))
		throw std::string("Failed to create indexbuffer");

	min_vertex = max_vertex = indices[0];
	for (unsigned i=1; i<indices.size(); i++) {
		unsigned short index = indices[i];
		min_vertex = min(min_vertex, index);
		max_vertex = max(max_vertex, index);
	}
	max_vertex++;

	void *dst;
	if (indexbuffer->Lock(0, 0, &dst, 0)) throw std::string("Failed to lock indexbuffer");
	memcpy(dst, &indices[0], sizeof(unsigned short)*indices.size());
	indexbuffer->Unlock();
}
