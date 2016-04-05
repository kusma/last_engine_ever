#include "tinyxml.h"
#include "file/file.h"
#include "SceneProxy.h"

using namespace engine;

static SceneProxy *scene_proxy;

static float read_float(TiXmlElement *elem, const char *name, float default_val) {
	const char *ret = elem->Attribute(name);
	if (!ret) return default_val;
	return float(atof(ret));
}

static void read_position(TiXmlNode* node, Vector* target) {
	TiXmlElement *temp = node->ToElement();
	if (!temp) return;
	*target = Vector(read_float(temp, "x", 0), read_float(temp, "y", 0), read_float(temp, "z", 0));
}

static void read_rotation(TiXmlNode* node, Quaternion* target) {
	TiXmlElement *temp = node->ToElement();
	if (!temp) return;
	*target = Quaternion(read_float(temp, "w", 1), read_float(temp, "x", 0), read_float(temp, "y", 0), read_float(temp, "z", 0));
}

static void read_scale(TiXmlNode* node, Vector* target) {
	TiXmlElement *temp = node->ToElement();
	if (!temp) return;
	*target = Vector(read_float(temp, "x", 1), read_float(temp, "y", 1), read_float(temp, "z", 1));
}

static void read_color(TiXmlNode* node, Color* target) {
	TiXmlElement *temp = node->ToElement();
	if (!temp) return;
	*target = Color(read_float(temp, "r", 1), read_float(temp, "g", 1), read_float(temp, "b", 1), read_float(temp, "a", 1));
}

void SceneProxy::read_object(Scene *scene, TiXmlNode* node, Entity* parent) {
	TiXmlElement *temp = node->ToElement();
	if (!temp) return;
	// Who gives a shit about some small memleak as long as it's not vidmem? Lets allocate and forget :)
	Object *temp_obj = new Object(mesh_proxy.get_resource(temp->Attribute("mesh")), parent);
	scene->add(temp_obj);
	read_children(scene, node, temp_obj);
}

void SceneProxy::read_camera(Scene *scene, TiXmlNode* node, Entity* parent) {
	TiXmlElement *temp = node->ToElement();
	if (!temp) return;
	// Who gives a shit about some small memleak as long as it's not vidmem? Lets allocate and forget :)
	Camera *temp_cam = new Camera(parent);
	scene->add(temp_cam);
	read_children(scene, node, temp_cam);
}

void SceneProxy::read_light(Scene *scene, TiXmlNode* node, Entity* parent) {
	TiXmlElement *temp = node->ToElement();
	if (!temp) return;
	Light *temp_light = new Light(parent);
	scene->add(temp_light);

	TiXmlNode* curr = node->FirstChild();
	while (curr) {
		if (strcmp(curr->Value(), "diffuse") == 0) read_color(curr, &temp_light->diffuse);
		if (strcmp(curr->Value(), "ambient") == 0) read_color(curr, &temp_light->ambient);
		if (strcmp(curr->Value(), "specular") == 0) read_color(curr, &temp_light->specular);
		curr = node->IterateChildren(curr);
	}	

	read_children(scene, node, temp_light);
}

void SceneProxy::read_children(Scene *scene, TiXmlNode* node, Entity* parent) {
	TiXmlNode* curr = node->FirstChild();
	while (curr) {
		// nodes
		if (strcmp(curr->Value(), "object") == 0) read_object(scene, curr, parent);
		else if (strcmp(curr->Value(), "light") == 0) read_light(scene, curr, parent);
		else if (parent) {
			// node properties
			if (strcmp(curr->Value(), "position") == 0) read_position(curr, &parent->prs.position);
			else if (strcmp(curr->Value(), "rotation") == 0) read_rotation(curr, &parent->prs.rotation);
			else if (strcmp(curr->Value(), "scale") == 0) read_scale(curr, &parent->prs.scale);
		} else {
			// scene properties
			if (strcmp(curr->Value(), "ambient") == 0) {
				Color temp;
				read_color(curr, &temp);
				temp.r = max(min(temp.r, 1), 0);
				temp.g = max(min(temp.g, 1), 0);
				temp.b = max(min(temp.b, 1), 0);
				scene->set_ambient(temp.r, temp.g, temp.b);
			}
		}
		curr = node->IterateChildren(curr);
	}	
}

Scene* SceneProxy::read_from_file(std::string filename) {
	file *fp = file_open(filename.c_str());
	if (!fp) throw std::string("File not found");

	char *string = new char[fp->size+1];
	file_read(string, 1, fp->size, fp);
	string[fp->size] = '\0';

	TiXmlDocument doc;
	doc.Parse(string);
	if (doc.Error()) {
		char temp[256];
		sprintf(temp, "line %i: %s", doc.ErrorRow(), doc.ErrorDesc());
		throw std::string(temp);
	}

	Scene *temp = new Scene;
	scene_proxy = this;
	read_children(temp, doc.RootElement(), 0);

	return temp;
}

std::map<Scene*, int> ResourceProxy<Scene>::ref_count_static;
std::map<std::string, Scene*> ResourceProxy<Scene>::filename_map_static;
