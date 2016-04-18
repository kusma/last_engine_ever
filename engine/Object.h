#ifndef OBJECT_H
#define OBJECT_H

#include "Entity.h"
#include "Mesh.h"

namespace engine {
	class Object : public Entity {
	public:
		Object( Mesh *mesh = 0, Entity *parent = 0 );

		void draw();

		void set_pivot_matrix(Matrix pivot) { this->pivot = pivot; }
		void set_visible(bool visible) { this->visible = visible; }

		Matrix get_pivot_matrix() { return pivot; }
		bool get_visible() { return visible; }
		Mesh *get_mesh() { return mesh; }

	private:
		bool visible;
		Mesh *mesh;
		Matrix pivot;
	};
}

#endif // OBJECT_H
