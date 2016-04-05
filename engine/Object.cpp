#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>

extern IDirect3DDevice9 *device;

#include "Object.h"

using namespace engine;

Object::Object( Mesh *mesh, Entity *parent) :  mesh(mesh), Entity(parent) {
	offset.identity();
	offset_inverse.identity();
}

Matrix Object::get_matrix() {
	Matrix pos, rot, scale;
	pos.translate(prs.position);
	rot.rotate(prs.rotation);
	scale.scale(prs.scale);
	return pos*offset_inverse*rot*offset*scale;
}

void Object::draw() {
	if (mesh) {
		Matrix absolute_matrix = get_absolute_matrix();
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&absolute_matrix);
		mesh->draw();
	}
}
