#ifndef VERTEX_H
#define VERTEX_H

#include "Vector.h"
#include "Uv.h"
#include "Color.h"

// FVF type
#define VERTEX_TYPE (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

namespace engine {

	class Vertex {
	public:
		Vertex() {}
		Vertex( Vector position, Vector normal, Uv uv ) : position(position), normal(normal), uv(uv) {}

		Vector position;
		Vector normal;
		unsigned color;
		Uv uv;
	};

}

#endif // VERTEX_H
