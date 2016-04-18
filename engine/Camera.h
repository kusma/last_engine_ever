#ifndef CAMERA_H
#define CAMERA_H

#include "Vector.h"
#include "Entity.h"

namespace engine {
	class Camera : public Entity {
	public:
		Camera(Entity *parent = 0, float fov = 60.f, float aspect = 4.f/3, float znear = 1.f, float zfar = 1000.f) : fov(fov), aspect(aspect),znear(znear), zfar(zfar), Entity(parent) {}

		void set_projection() {
			Matrix projection_matrix;
			projection_matrix.project(fov, 4.f/3.f, znear, zfar);
			device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&projection_matrix);
		}

		virtual void set() {
			set_projection();
			Matrix matrix = get_absolute_matrix();
			matrix.inverse();
			device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&matrix);
		}

		float fov;
		float aspect;
		float znear;
		float zfar;
	};

	class LookAtCamera : public Camera {
	public:
		LookAtCamera(Entity *target = 0, Entity *parent = 0, float roll = 0.f, float fov = 60.f, float aspect = 4.f/3, float znear = 1.f, float zfar = 1000.f) : target(target), roll(roll), Camera(parent, fov, aspect, znear) {}

		Matrix get_matrix() {
			Vector targ = Vector(0.f, 0.f, 0.f);
			if (target) targ = target->get_absolute_matrix().get_translation();

			Matrix mat;
			mat.look_at(prs.position, targ, roll);
			return mat;
		}

		void set() {
			set_projection();
			Matrix matrix = get_absolute_matrix();
			device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&matrix);
		}

		Entity *target;
		float roll;
	};
/*

		Camera(Vector position, Vector target = Vector(0.f, 0.f, 0.f), float roll = 0.f, float fov = 60.f, float znear = 1.f, float zfar = 1000.f) : parent(parent), position(position), target(target), roll(roll), fov(fov), znear(znear), zfar(zfar) {
		Vector position()
		}

		void set() {
		Matrix projection_matrix;
		projection_matrix.project(fov, 4.f/3.f, znear, zfar);
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&projection_matrix);

		Matrix view_matrix;
		view_matrix.look_at(position, target, roll);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&view_matrix);
		}

*/
}

#endif // CAMERA_H