#include "tinyxml.h"
#include "file/file.h"
#include "MaterialProxy.h"

using namespace engine;

inline float read_float(TiXmlElement *elem, const char *name, float default_val) {
	const char *ret = elem->Attribute(name);
	if (!ret) return default_val;
	return float(atof(ret));
}

static void read_color(TiXmlNode* node, Color* target) {
	TiXmlElement *temp = node->ToElement();
	if (temp) *target = Color(read_float(temp, "r", 1), read_float(temp, "g", 1), read_float(temp, "b", 1), read_float(temp, "a", 1));
}

Material* MaterialProxy::read_from_file(std::string filename) {
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

	Material *temp = new Material;
	TiXmlNode* node = doc.RootElement();
	TiXmlNode* curr = node->FirstChild();
	while (curr) {
		if (strcmp(curr->Value(), "emissive") == 0) read_color(curr, &temp->emissive);
		if (strcmp(curr->Value(), "ambient") == 0) read_color(curr, &temp->ambient);
		if (strcmp(curr->Value(), "diffuse") == 0) read_color(curr, &temp->diffuse);
		if (strcmp(curr->Value(), "specular") == 0) {
			TiXmlElement *temp_node = node->ToElement();
			if (temp_node) {
				temp->shinyness = read_float(temp_node, "shinyness", 0);
				temp->specular_power = read_float(temp_node, "power", 1);
			}
		}

		if (strcmp(curr->Value(), "blend") == 0) {
			TiXmlElement *temp_node = curr->ToElement();
			if (temp_node)  {
				const char *str = temp_node->Attribute("srcblend");

				if (str) {
					unsigned blend = D3DBLEND_ONE;
					if (strcmp(str, "zero") == 0) blend = D3DBLEND_ZERO;
					else if (strcmp(str, "one") == 0) blend = D3DBLEND_ONE;
					else if (strcmp(str, "srccolor") == 0) blend = D3DBLEND_SRCCOLOR;
					else if (strcmp(str, "invsrccolor") == 0) blend = D3DBLEND_INVSRCCOLOR;
					else if (strcmp(str, "srcalpha") == 0) blend = D3DBLEND_SRCALPHA;
					else if (strcmp(str, "invsrcalpha") == 0) blend = D3DBLEND_INVSRCALPHA;
					else if (strcmp(str, "destalpha") == 0) blend = D3DBLEND_DESTALPHA;
					else if (strcmp(str, "invdestalpha") == 0) blend = D3DBLEND_INVDESTALPHA;
					else if (strcmp(str, "destcolor") == 0) blend = D3DBLEND_DESTCOLOR;
					else if (strcmp(str, "invdestcolor") == 0) blend = D3DBLEND_INVDESTCOLOR;
					else if (strcmp(str, "srcalphasat") == 0) blend = D3DBLEND_SRCALPHASAT;
					else if (strcmp(str, "bothsrcalpha") == 0) blend = D3DBLEND_BOTHSRCALPHA;
					else if (strcmp(str, "bothinvsrcalpha") == 0) blend = D3DBLEND_BOTHINVSRCALPHA;
					else if (strcmp(str, "blendfactor") == 0) blend = D3DBLEND_BLENDFACTOR;
					else if (strcmp(str, "invblendfactor") == 0) blend = D3DBLEND_INVBLENDFACTOR;
					else throw std::string("invalid srcblend");
					temp->srcblend = blend;
				}

				str = temp_node->Attribute("dstblend");
				if (str) {
					unsigned blend = D3DBLEND_ZERO;
					if (strcmp(str, "zero") == 0) blend = D3DBLEND_ZERO;
					else if (strcmp(str, "one") == 0) blend = D3DBLEND_ONE;
					else if (strcmp(str, "srccolor") == 0) blend = D3DBLEND_SRCCOLOR;
					else if (strcmp(str, "invsrccolor") == 0) blend = D3DBLEND_INVSRCCOLOR;
					else if (strcmp(str, "srcalpha") == 0) blend = D3DBLEND_SRCALPHA;
					else if (strcmp(str, "invsrcalpha") == 0) blend = D3DBLEND_INVSRCALPHA;
					else if (strcmp(str, "destalpha") == 0) blend = D3DBLEND_DESTALPHA;
					else if (strcmp(str, "invdestalpha") == 0) blend = D3DBLEND_INVDESTALPHA;
					else if (strcmp(str, "destcolor") == 0) blend = D3DBLEND_DESTCOLOR;
					else if (strcmp(str, "invdestcolor") == 0) blend = D3DBLEND_INVDESTCOLOR;
					else if (strcmp(str, "srcalphasat") == 0) blend = D3DBLEND_SRCALPHASAT;
					else if (strcmp(str, "bothsrcalpha") == 0) blend = D3DBLEND_BOTHSRCALPHA;
					else if (strcmp(str, "bothinvsrcalpha") == 0) blend = D3DBLEND_BOTHINVSRCALPHA;
					else if (strcmp(str, "blendfactor") == 0) blend = D3DBLEND_BLENDFACTOR;
					else if (strcmp(str, "invblendfactor") == 0) blend = D3DBLEND_INVBLENDFACTOR;
					else throw std::string("invalid srcblend");
					temp->dstblend = blend;
				}
			}
		}

		if (strcmp(curr->Value(), "texture") == 0) {
			TiXmlElement *temp_node = curr->ToElement();
			if (temp_node)  {
				const char *str = temp_node->Attribute("file");
				if (str) {
					std::string texture_filename(str);
					temp->texture = texture_proxy.get_resource(texture_filename);
				}
			}
		}

		if (strcmp(curr->Value(), "envmap") == 0) {
			TiXmlElement *temp_node = curr->ToElement();
			if (temp_node)  {
				const char *str = temp_node->Attribute("file");
				if (str) {
					std::string texture_filename(str);
					temp->envmap = texture_proxy.get_resource(texture_filename);
				}
			}
		}
		curr = node->IterateChildren(curr);
	}
	return temp;
}

std::map<Material*, int> ResourceProxy<Material>::ref_count_static;
std::map<std::string, Material*> ResourceProxy<Material>::filename_map_static;
