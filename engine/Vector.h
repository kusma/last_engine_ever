#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

namespace engine {

	class Vector {
	public:
		Vector() {}
		Vector(float x, float y, float z) : x(x), y(y), z(z) {}
		Vector(float v[3]) : x(v[0]), y(v[1]), z(v[2]) {}

		Vector operator+(const Vector v) const {
			return Vector( x+v.x,  y+v.y, z+v.z );
		}

		Vector &operator+=(const Vector v) {
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}

		Vector operator-(const Vector v) const {
			return Vector( x-v.x,  y-v.y, z-v.z );
		}

		Vector &operator-=(const Vector v) {
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}

		Vector operator-() const {
			return Vector( -x, -y, -z );
		}

		Vector operator*(const float f) const {
			return Vector( x*f, y*f, z*f );	
		}

		Vector &operator*=(const float f) {
			x *= f;
			y *= f;
			z *= f;
			return *this;
		}

		float magnitude() const {
			return (float)sqrt(x*x+y*y+z*z);
		}

		
		float dot(const Vector &v) const {
			return x * v.x + y * v.y + z * v.z;
		}

		Vector cross(const Vector &v) const {
			return Vector(	-(y * v.z - z * v.y),
							-(z * v.x - x * v.z),
							-(x * v.y - y * v.x)
						);
		}

		Vector &normalize() {
			*this *= 1.f / magnitude();
			return *this;
		}

		float x, y, z;
	};

}
#endif // VECTOR_H
