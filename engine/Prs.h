#ifndef PRS_H
#define PRS_H

#include "Vector.h"
#include "Quaternion.h"

namespace engine {
	class Prs {
	public:
		Prs(Vector position = Vector(0.f, 0.f, 0.f), Quaternion rotation = Quaternion(1.f, 0.f, 0.f, 0.f), Vector scale = Vector(1.f, 1.f, 1.f)) : position(position), rotation(rotation), scale(scale) {}
		Vector position;
		Quaternion rotation;
		Vector scale;
	};
}

#endif // PRS_H
