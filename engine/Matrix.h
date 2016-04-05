#ifndef MATRIX_H
#define MATRIX_H

#include "Vector.h"
#include "Quaternion.h"
#include "Prs.h"

namespace engine {

	class Matrix {
	private:
	public:
		Matrix() {};
		Matrix(const float m[4][4]);

		void identity();
		void rotate(const Vector &v);
		void rotate(const Quaternion &q);
		void prs(const Prs &prs);
		void scale(const Vector &v);
		void translate(const Vector &v);
		void project(float fovy, float aspect, float zn, float zf);
		void look_at(Vector position, Vector target, float roll);

		Vector get_translation();

		void transponate();
		void inverse();
		void swap_rows( const unsigned row1, const unsigned row2 );

		Vector transform_vector(const Vector &v);
		Vector rotate_vector(const Vector &v);
		float transform_z(const Vector &v);

		Matrix operator*=(const Matrix m);
		Matrix operator*(const Matrix m) const;

		float matrix[4][4];
	};

}

#endif // MATRIX_H
