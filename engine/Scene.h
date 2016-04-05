#ifndef SCENE_H
#define SCENE_H

#include "Object.h"
#include "Camera.h"
#include "Light.h"
#include <list>

namespace engine {
	class Scene {
	public:
		Scene() : ambient(0xFFFFFF) {}
										
		void draw(unsigned camera = 0, bool clear = true);

		void add(Object* object) {
			objects.push_back(object);
		}

		void add(Light* light) {
			lights.push_back(light);
		}

		void add(Camera* camera) {
			cameras.push_back(camera);
		}

		void set_ambient(float r, float g, float b) {
			this->ambient = (unsigned(r * 255) << 16) | (unsigned(g * 255) << 8) | unsigned(b * 255);
		}

		std::vector<Object*> objects;
		std::vector<Camera*> cameras;
		std::vector<Light*> lights;
		unsigned ambient;
	private:
	};
}

#endif // SCENE_H
