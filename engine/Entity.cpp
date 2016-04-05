#include "Entity.h"
#include <stdio.h>

using namespace engine;

Entity::Entity( Entity *parent ) : parent(parent), prs(engine::Vector(0.f, 0.f, 0.f), engine::Quaternion(1.f, 0.f, 0.f, 0.f), engine::Vector(1.f, 1.f, 1.f)) {}

Matrix Entity::get_absolute_matrix() {
	Matrix absolute_matrix = get_matrix();

	Entity *current = this;
	while (current->parent) {
		current = current->parent;
		absolute_matrix = current->get_matrix() * absolute_matrix;
	}

	return absolute_matrix;
}

Matrix Entity::get_matrix() {
	Matrix temp;
	temp.prs(prs);
	return temp;
}

