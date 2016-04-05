#include "Mesh.h"

using namespace engine;

Mesh::Mesh() : vertexbuffer(0) {}

Mesh::~Mesh() {
	if (vertexbuffer) vertexbuffer->Release();
}

void Mesh::update() {
	if (!vertexbuffer && FAILED(device->CreateVertexBuffer(UINT(sizeof(Vertex) * vertices.size()), 0, VERTEX_TYPE, D3DPOOL_MANAGED, &vertexbuffer, 0)))
		throw std::string("Failed to create vertexbuffer");

	void *dst;
	if (vertexbuffer->Lock(0, 0, &dst, 0)) throw std::string("Failed to lock vertexbuffer");
	memcpy(dst, &vertices[0], sizeof(Vertex)*vertices.size());
	vertexbuffer->Unlock();
}

void Mesh::animate( float time ) {}

void Mesh::draw() {
	if (vertices.size() == 0) return;
	if (!vertexbuffer) update();

	device->SetFVF(VERTEX_TYPE);
	device->SetStreamSource( 0, vertexbuffer, 0, sizeof(Vertex));

//	device->DrawPrimitive( D3DPT_POINTLIST, 0, vertices.size() );
	std::list<SubMesh>::iterator i;
	for (i=submeshes.begin(); i!=submeshes.end(); i++) (*i).draw();
}

void SimpleMesh::draw()
{
	Mesh::draw();
	device->DrawPrimitive( D3DPT_TRIANGLELIST, 0, vertices.size() / 3);
}
