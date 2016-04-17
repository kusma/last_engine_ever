#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>

extern IDirect3DDevice9 *device;

#include "Object.h"

using namespace engine;

Object::Object(Mesh *mesh, Entity *parent) :  mesh(mesh), Entity(parent), visible(true) {
	pivot.identity();
}

void Object::draw() {
	if (mesh && visible) {
		Matrix absolute_matrix = get_absolute_matrix() * pivot;
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&absolute_matrix);
		mesh->draw();
	}
}
