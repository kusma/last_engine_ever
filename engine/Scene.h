#ifndef SCENE_H
#define SCENE_H

#include "Object.h"
#include "Camera.h"
#include "Light.h"
#include <list>

namespace engine {
	class Scene {
	public:
		Scene() : ambient(0.1f, 0.1f, 0.1f), fog(false), fog_color(0,0,0), fog_near(0), fog_far(1000) {}
										
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

		void set_ambient(Color color) {
			ambient = color;
		}

		std::vector<Object*> objects;
		std::vector<Camera*> cameras;
		std::vector<Light*> lights;

		Color ambient;

		bool fog;
		Color fog_color;
		float fog_near;
		float fog_far;
	};
}

#endif // SCENE_H
