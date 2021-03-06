#include "Scene.h"

using namespace engine;

void Scene::draw(unsigned camera, bool clear) {
	if (clear) device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);
	else if (clear) device->Clear(0, 0, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);

	device->SetRenderState(D3DRS_AMBIENT, ambient);
	device->SetRenderState(D3DRS_LIGHTING, lights.size() > 0);

	if (camera < cameras.size()) cameras[camera]->set();

	unsigned i = 0;
	std::vector<Light*>::const_iterator light_iter;
	for (light_iter=lights.begin(); light_iter!=lights.end(); light_iter++) {
		(*light_iter)->set(i++);
	}

	for (; i<8; i++) {
		device->LightEnable(i, false);
	}

	std::vector<Object*>::const_iterator object_iter;
	for (object_iter=objects.begin(); object_iter!=objects.end(); object_iter++) {
//		device->SetRenderState(D3DRS_LIGHTING, lights.size() > 0);
		(*object_iter)->draw();
	}
}
