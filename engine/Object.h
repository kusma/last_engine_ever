#ifndef OBJECT_H
#define OBJECT_H

#include "Entity.h"
#include "Mesh.h"

namespace engine {
	class Object : public Entity {
	public:
		Object( Mesh *mesh = 0, Entity *parent = 0 );

		void set_offset(Matrix offset) {
			this->offset = offset;
			offset_inverse = offset;
			offset_inverse.inverse();
		}

		Matrix get_matrix();

		void draw();
	private:
		Mesh *mesh;
		Matrix offset;
		Matrix offset_inverse;
	};
}

#endif // OBJECT_H
