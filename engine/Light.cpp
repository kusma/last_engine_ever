#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>

extern IDirect3DDevice9 *device;

#include "Light.h"

using namespace engine;

Light::Light(Entity *parent) : Entity(parent) {
	ambient = Color(0.05f, 0.05f, 0.05f);
	diffuse = Color(1.f, 1.f, 1.f);
	specular = Color(0.f, 0.f, 0.f);
}

void Light::set( unsigned index ) {
	Matrix absolute_matrix = get_absolute_matrix();
	Vector pos = absolute_matrix.get_translation();

	memset(&light, 0, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_POINT;

	light.Position.x = pos.x;
	light.Position.y = pos.y;
	light.Position.z = pos.z;

	light.Ambient.r = ambient.r;
	light.Ambient.g = ambient.g;
	light.Ambient.b = ambient.b;

	light.Diffuse.r = diffuse.r;
	light.Diffuse.g = diffuse.g;
	light.Diffuse.b = diffuse.b;

	light.Range = float(sqrt(FLT_MAX));

	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	device->SetLight(index, &light);
	device->LightEnable(index, true);
}
