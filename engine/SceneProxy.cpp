#include <tinyxml2.h>
#include "file/file.h"
#include "SceneProxy.h"

using namespace engine;
using namespace tinyxml2;

static SceneProxy *scene_proxy;

static float read_float(XMLElement *elem, const char *name, float default_val) {
	const char *ret = elem->Attribute(name);
	if (!ret) return default_val;
	return float(atof(ret));
}

static void read_position(XMLNode* node, Vector* target) {
	XMLElement *temp = node->ToElement();
	if (!temp) return;
	*target = Vector(read_float(temp, "x", 0), read_float(temp, "y", 0), read_float(temp, "z", 0));
}

static void read_rotation(XMLNode* node, Quaternion* target) {
	XMLElement *temp = node->ToElement();
	if (!temp) return;
	*target = Quaternion(read_float(temp, "w", 1), read_float(temp, "x", 0), read_float(temp, "y", 0), read_float(temp, "z", 0));
}

static void read_scale(XMLNode* node, Vector* target) {
	XMLElement *temp = node->ToElement();
	if (!temp) return;
	*target = Vector(read_float(temp, "x", 1), read_float(temp, "y", 1), read_float(temp, "z", 1));
}

static void read_color(XMLNode* node, Color* target) {
	XMLElement *temp = node->ToElement();
	if (!temp) return;
	*target = Color(read_float(temp, "r", 1), read_float(temp, "g", 1), read_float(temp, "b", 1), read_float(temp, "a", 1));
}

static void read_prs(XMLNode* node, Prs* target) {
	XMLNode* curr = node->FirstChild();
	while (curr) {
		if (strcmp(curr->Value(), "position") == 0) read_position(curr, &target->position);
		else if (strcmp(curr->Value(), "rotation") == 0) read_rotation(curr, &target->rotation);
		else if (strcmp(curr->Value(), "scale") == 0) read_scale(curr, &target->scale);
		curr = curr->NextSibling();
	}
}

void SceneProxy::read_object(Scene *scene, XMLNode* node, Entity* parent) {
	XMLElement *temp = node->ToElement();
	if (!temp) return;

	Mesh *temp_mesh = 0;
	if (strcmp(temp->Attribute("mesh"),"") != 0) temp_mesh = mesh_proxy.get_resource(temp->Attribute("mesh"));

	// Who gives a shit about some small memleak as long as it's not vidmem? Lets allocate and forget :)
	Object *temp_obj = new Object(temp_mesh, parent);
	scene->add(temp_obj);

	XMLNode* curr = node->FirstChild();
	while (curr) {
		if (strcmp(curr->Value(), "pivot") == 0) {
			XMLNode* curr = node->FirstChild();
			while (curr) {
				if (strcmp(curr->Value(), "pivot") == 0) {
					Prs pivot;
					read_prs(curr, &pivot);
					Matrix pivot_matrix;
					pivot_matrix.prs(pivot);
//					pivot_matrix.inverse();
					temp_obj->set_pivot_matrix(pivot_matrix);
				}
				curr = curr->NextSibling();
			}
		}
		curr = curr->NextSibling();
	}

	read_children(scene, node, temp_obj);
}

void SceneProxy::read_camera(Scene *scene, XMLNode* node, Entity* parent) {
	XMLElement *temp = node->ToElement();
	if (!temp) return;
	// Who gives a shit about some small memleak as long as it's not vidmem? Lets allocate and forget :)
	Camera *temp_cam = new Camera(parent);
	scene->add(temp_cam);
	read_children(scene, node, temp_cam);
}

void SceneProxy::read_light(Scene *scene, XMLNode* node, Entity* parent) {
	XMLElement *temp = node->ToElement();
	if (!temp) return;
	Light *temp_light = new Light(parent);
	scene->add(temp_light);

	XMLNode* curr = node->FirstChild();
	while (curr) {
		if (strcmp(curr->Value(), "diffuse") == 0) read_color(curr, &temp_light->diffuse);
		if (strcmp(curr->Value(), "ambient") == 0) read_color(curr, &temp_light->ambient);
		if (strcmp(curr->Value(), "specular") == 0) read_color(curr, &temp_light->specular);
		curr = curr->NextSibling();
	}	

	read_children(scene, node, temp_light);
}

void SceneProxy::read_children(Scene *scene, XMLNode* node, Entity* parent) {
	read_prs(node, &parent->prs);

	XMLNode* curr = node->FirstChild();
	while (curr) {
		// nodes
		if (strcmp(curr->Value(), "object") == 0) read_object(scene, curr, parent);
		else if (strcmp(curr->Value(), "light") == 0) read_light(scene, curr, parent);
		else if (parent) {
			// node properties
		} else {
			// scene properties
			if (strcmp(curr->Value(), "ambient") == 0) {
				Color temp;
				read_color(curr, &temp);
				scene->set_ambient(temp);
			}
		}
		curr = curr->NextSibling();
	}
}

Scene* SceneProxy::read_from_file(std::string filename) {
	file *fp = file_open(filename.c_str());
	if (!fp) throw std::string("File not found");

	char *string = new char[fp->size+1];
	file_read(string, 1, fp->size, fp);
	string[fp->size] = '\0';

	tinyxml2::XMLDocument doc;
	doc.Parse(string);
	if (doc.Error())
		throw std::string(doc.GetErrorStr1());

	Scene *temp = new Scene;
	scene_proxy = this;
	read_children(temp, doc.RootElement(), 0);

	return temp;
}

std::map<Scene*, int> ResourceProxy<Scene>::ref_count_static;
std::map<std::string, Scene*> ResourceProxy<Scene>::filename_map_static;
