#ifndef QUATERNION_H
#define QUATERNION_H

#include "Vector.h"

namespace engine {
	class Quaternion {
	public:
		Quaternion() {}

		Quaternion(float w, float x, float y, float z) : x(x), y(y), z(z), w(w) {
			fix_sign();
		}

		Quaternion(const Vector &euler) {
			Quaternion x_quat = Quaternion(cos(euler.x / 2), sin(euler.x/2), 0, 0);
			Quaternion y_quat = Quaternion(cos(euler.y / 2), 0, sin(euler.y/2), 0);
			Quaternion z_quat = Quaternion(cos(euler.z / 2), 0, 0, sin(euler.z/2));
			*this = x_quat * y_quat * z_quat;
		}

		Quaternion(const Vector &axis, float angle) {
			w = cos(angle / 2);
			float s = float(sin(angle / 2));
			x = axis.x * s;
			y = axis.y * s;
			z = axis.z * s;
			fix_sign();
		}

		void fix_sign() {
			if (w < 0) flip_sign();
		}

		Quaternion Quaternion::operator +(const Quaternion& a) const {
			return Quaternion(w + a.w, x + a.x, y + a.y, z + a.z);
		}

		const Quaternion& Quaternion::operator +=(const Quaternion& q) {
			w += q.w;
			x += q.x;
			y += q.y;
			z += q.z;
			return *this;
		}

		Quaternion Quaternion::operator -(const Quaternion& a) const {
			return Quaternion(w - a.w, x - a.x, y - a.y, z - a.z);
		}

		const Quaternion& Quaternion::operator -=(const Quaternion& q) {
			w -= q.w;
			x -= q.x;
			y -= q.y;
			z -= q.z;
			return *this;
		}

		Quaternion Quaternion::operator *(const Quaternion& a) const {
			return Quaternion(
				w*a.w - x*a.x - y*a.y - z*a.z,
				w*a.x + x*a.w + y*a.z - z*a.y,
				w*a.y + y*a.w + z*a.x - x*a.z,
				w*a.z + z*a.w + x*a.y - y*a.x
			);
		}

		Quaternion Quaternion::operator *(const float a) const {
			return Quaternion(w*a, x*a, y*a, z*a);
		}

		const Quaternion& Quaternion::operator *=(const float a) {
			w *= a;
			x *= a;
			y *= a;
			z *= a;
			return *this;
		}

		float Quaternion::norm() const {
			return w*w + x*x + y*y + z*z;
		}

		float Quaternion::abs() const {
			return float(sqrt(w*w + x*x + y*y + z*z));
		}

		Quaternion Quaternion::conjugate() const {
			return Quaternion(w, -x, -y, -z);
		}


		bool Quaternion::operator==(const Quaternion &q) const {
			return (*this - q).norm() < 1e-5;
		}

		Quaternion Quaternion::inverse() const {
			float scale = 1.f / norm();
			return Quaternion(w*scale, -x*scale, -y*scale, -z*scale);
		}

		Quaternion Quaternion::slerp(const Quaternion& q, const float t) const {
			Quaternion temp;
			float omega, cosom, sinom, scale0 = 1.0f, scale1 = 0.0f;
			float prediv = 1.f;

			cosom =	this->x*q.x + this->y*q.y + this->z*q.z + this->w*q.w;

			if (cosom < 0.f) {
				cosom = -cosom;
				temp.x = -q.x;
				temp.y = -q.y;
				temp.z = -q.z;
				temp.w = -q.w;
			} else {
				temp.x = q.x;
				temp.y = q.y;
				temp.z = q.z;
				temp.w = q.w;
			}

			omega = (float)acos(cosom);
			sinom = (float)sin(omega);
			if (fabs(sinom) > 0.0001f) {
				prediv = 1.f/sinom;
				scale0 = (float)sin((1.f-t)*omega)*prediv;
				scale1 = (float)sin(t*omega)*prediv;
			}

			temp.x = scale0*this->x + scale1*temp.x;
			temp.y = scale0*this->y + scale1*temp.y;
			temp.z = scale0*this->z + scale1*temp.z;
			temp.w = scale0*this->w + scale1*temp.w;

			return temp;
		}

		void Quaternion::flip_sign() {
			w = -w;
			x = -x;
			y = -y;
			z = -z;
		}

		void Quaternion::normalize() {
			fix_sign();
			float qnorm = norm();
			if (qnorm < 1e-5) {
				w = 1.0f; x = 0.0f; y = 0.0f; z = 0.0f;
				return;
			}
			*this *= 1.f / float(sqrt(qnorm));
		}

		Quaternion Quaternion::log() const {
			Quaternion result;
			result.w = 0.f;

			if (fabs(w) < 1.f) {
				float angle = acos(w);
				float sn = sin(angle);
				if (fabs(sn)>=0.01f) {
					float coeff = angle/sn;
					result.x = coeff*x;
					result.y = coeff*y;
					result.z = coeff*z;
					return result;
				}
			}

			result.x = x;
			result.y = y;
			result.z = z;
			return result;
		}

		Quaternion Quaternion::exp() const {
			Quaternion result;
			float angle = sqrt(x*x + y*y + z*z);
			float fsin = sin(angle);
			result.w = cos(angle);
			if(fabs(fsin) >= 1e-6) {
				float coef = fsin / angle;
				result.x = coef*x;
				result.y = coef*y;
				result.z = coef*z;
			} else {
				result.x = x;
				result.y = y;
				result.z = z;
			}
			return result;
		}

		float x, y, z, w;
	};
}

#endif // QUATERNION_H