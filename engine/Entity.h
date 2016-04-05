#ifndef ENTITY_H
#define ENTITY_H

#include "Prs.h"
#include "Matrix.h"

namespace engine {
	class Entity {
	public:
		Entity( Entity *parent = 0 );

		Matrix get_absolute_matrix();
		virtual Matrix get_matrix();

		void set_prs(const Prs &prs) {
			this->prs = prs;
		}

		Prs prs;
		Entity *parent;
	};
}

#endif // ENTITY_H
