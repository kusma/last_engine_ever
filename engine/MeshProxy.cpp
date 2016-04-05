#include "file/file.h"
#include "MeshProxy.h"

using namespace engine;

Mesh* MeshProxy::read_from_file(std::string filename) {
	file *fp = file_open(filename.c_str());
	if (!fp) throw std::string("File not found");

	char test[5];
	file_read( test, 1, 5, fp );
	if (memcmp(test, "MESH1", 5) != 0) {
		file_close(fp);
		throw std::string("Not a MESH1-file");
	}

	Mesh *temp = new Mesh;
	unsigned vertex_count;
	file_read(&vertex_count, 1, 4, fp);
	printf("vertices: %i\n", vertex_count);
	temp->vertices.resize(vertex_count);
	file_read(&temp->vertices[0], sizeof(Vertex), vertex_count, fp);

	unsigned submesh_count;
	file_read(&submesh_count, 1, 4, fp);
	printf("submeshes: %i\n", submesh_count);
	for (unsigned i=0; i<submesh_count; i++) {
		SubMesh *submesh = new SubMesh;
		char *mat = file_loadstring(fp);
		std::string material_name(mat);
		printf("material name: \"%s\"\n", material_name.c_str());
		free(mat);
		submesh->set_material(material_proxy.get_resource(material_name));
		unsigned face_count;
		file_read(&face_count, 1, 4, fp);
		printf("faces: %i\n", face_count);
		submesh->indices.resize(face_count * 3);
		file_read(&submesh->indices[0], sizeof(unsigned short), face_count * 3, fp);
		temp->submeshes.push_back(*submesh);
	}
	file_close(fp);
	return temp;
}

std::map<Mesh*, int> ResourceProxy<Mesh>::ref_count_static;
std::map<std::string, Mesh*> ResourceProxy<Mesh>::filename_map_static;
