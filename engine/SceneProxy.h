#ifndef SCENE_PROXY_H
#define SCENE_PROXY_H

#include <tinyxml2.h>
#include "Scene.h"
#include "ResourceProxy.h"
#include "MeshProxy.h"

namespace engine {
	class SceneProxy : public ResourceProxy<Scene> {
	private:
		Scene* read_from_file(std::string filename);
		MeshProxy mesh_proxy;

		void read_children(Scene *scene, tinyxml2::XMLNode* node, Entity* parent);
		void read_object(Scene *scene, tinyxml2::XMLNode* node, Entity* parent);
		void read_camera(Scene *scene, tinyxml2::XMLNode* node, Entity* parent);
		void read_light(Scene *scene, tinyxml2::XMLNode* node, Entity* parent);
	};
}

#endif // SCENE_PROXY_H