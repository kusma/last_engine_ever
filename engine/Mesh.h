#ifndef MESH_H
#define MESH_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
extern IDirect3DDevice9 *device;

#include <vector>
#include <list>

#include "Vertex.h"
#include "SubMesh.h"

namespace engine {

	class Mesh {
	public:
		Mesh();
		~Mesh();

		void update();
		virtual void animate(float time);
		virtual void draw();

		std::vector<Vertex> vertices;
//		std::vector<SubMesh> submeshes;
		std::list<SubMesh> submeshes;

	private:
		IDirect3DVertexBuffer9 *vertexbuffer;
	};

	// works without submeshes & indexbuffers
	class SimpleMesh : public Mesh
	{
	public:
		void draw();
	};
}

#endif // MESH_H
