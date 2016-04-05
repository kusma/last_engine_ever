#ifndef VERTEX2D_H
#define VERTEX2D_H

#include "Vector.h"
#include "Uv.h"

#define VERTEX2D_TYPE (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

namespace engine {
	class Vertex2D {
	public:
		Vertex2D( Vector pos = Vector(0, 0, 0), Uv uv = Uv(0, 0), unsigned color = 0xFFFFFF ) : pos(pos), color(color), uv(uv) {}

		Vector pos;
		unsigned color;
		Uv uv;
	};
};

#endif // VERTEX2D_H
