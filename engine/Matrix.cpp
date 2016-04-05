#include "Matrix.h"

#include <memory.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979f
#endif
#include <algorithm>



using namespace engine;

Matrix::Matrix(const float m[4][4]) {
	matrix[0][0] = m[0][0];	matrix[0][1] = m[0][1];	matrix[0][2] = m[0][2];	matrix[0][3] = m[0][3];
	matrix[1][0] = m[1][0];	matrix[1][1] = m[1][1];	matrix[1][2] = m[1][2];	matrix[1][3] = m[1][3];
	matrix[2][0] = m[2][0];	matrix[2][1] = m[2][1];	matrix[2][2] = m[2][2];	matrix[2][3] = m[2][3];
	matrix[3][0] = m[3][0];	matrix[3][1] = m[3][1];	matrix[3][2] = m[3][2];	matrix[3][3] = m[3][3];
}

void Matrix::identity() {
	matrix[0][0] = 1;	matrix[0][1] = 0;	matrix[0][2] = 0;	matrix[0][3] = 0;
	matrix[1][0] = 0;	matrix[1][1] = 1;	matrix[1][2] = 0;	matrix[1][3] = 0;
	matrix[2][0] = 0;	matrix[2][1] = 0;	matrix[2][2] = 1;	matrix[2][3] = 0;
	matrix[3][0] = 0;	matrix[3][1] = 0;	matrix[3][2] = 0;	matrix[3][3] = 1;	
}

void Matrix::rotate(const Vector &v) {
	float cx = float(cos(v.x));
	float cy = float(cos(v.y));
	float cz = float(cos(v.z));
	float sx = float(sin(v.x));
	float sy = float(sin(v.y));
	float sz = float(sin(v.z));
	matrix[0][0] = cy*cz;			matrix[0][1] = cy*sz;			matrix[0][2] = -sy;		matrix[0][3] = 0;
	matrix[1][0] = sx*sy*cz-cx*sz;	matrix[1][1] = sx*sy*sz+cx*cz;	matrix[1][2] = sx*cy;	matrix[1][3] = 0;
	matrix[2][0] = cx*sy*cz+sx*sz;	matrix[2][1] = cx*sy*sz-sx*cz;	matrix[2][2] = cx*cy;	matrix[2][3] = 0;
	matrix[3][0] = 0;				matrix[3][1] = 0;				matrix[3][2] = 0;		matrix[3][3] = 1;
}

void Matrix::rotate(const Quaternion &q) {
	float xs = q.x*2.f, ys = q.y*2.f, zs = q.z*2.f;
	float wx = q.w*xs, wy = q.w*ys, wz = q.w*zs;
	float xx = q.x*xs, xy = q.x*ys, xz = q.x*zs;
	float yy = q.y*ys, yz = q.y*zs, zz = q.z*zs;
	matrix[0][0] = 1.f - (yy + zz); matrix[1][0] = xy - wz;         matrix[2][0] = xz + wy;
	matrix[0][1] = xy + wz;         matrix[1][1] = 1.f - (xx + zz); matrix[2][1] = yz - wx;
	matrix[0][2] = xz - wy;         matrix[1][2] = yz + wx;         matrix[2][2] = 1.f - (xx + yy);

	matrix[0][3] = matrix[1][3] = matrix[2][3] = 0.f;
	matrix[3][0] = matrix[3][1] = matrix[3][2] = 0.f;
	matrix[3][3] = 1.f;
}

void Matrix::prs(const Prs &prs) {
	Matrix pos, rot, scale;
	pos.translate(prs.position);
	rot.rotate(prs.rotation);
	scale.scale(prs.scale);
	*this = pos * rot * scale;
}

void Matrix::scale(const Vector &v) {
	matrix[0][0] = v.x;	matrix[0][1] = 0;	matrix[0][2] = 0;	matrix[0][3] = 0;
	matrix[1][0] = 0;	matrix[1][1] = v.y;	matrix[1][2] = 0;	matrix[1][3] = 0;
	matrix[2][0] = 0;	matrix[2][1] = 0;	matrix[2][2] = v.z;	matrix[2][3] = 0;
	matrix[3][0] = 0;	matrix[3][1] = 0;	matrix[3][2] = 0;	matrix[3][3] = 1;
}

void Matrix::translate(const Vector &v) {
	matrix[0][0] = 1;	matrix[0][1] = 0;	matrix[0][2] = 0;	matrix[0][3] = 0;
	matrix[1][0] = 0;	matrix[1][1] = 1;	matrix[1][2] = 0;	matrix[1][3] = 0;
	matrix[2][0] = 0;	matrix[2][1] = 0;	matrix[2][2] = 1;	matrix[2][3] = 0;
	matrix[3][0] = v.x;	matrix[3][1] = v.y;	matrix[3][2] = v.z;	matrix[3][3] = 1;
}

void Matrix::project(float fovy, float aspect, float zn, float zf) {
	aspect = 1.f/aspect;
	fovy *= (M_PI/180);

	float h = (float)(1.0/tan(fovy*0.5f));
	float q = zf/(zf-zn);

	matrix[0][0] = h*aspect;	matrix[0][1] = 0;	matrix[0][2] = 0;		matrix[0][3] = 0;
	matrix[1][0] = 0;			matrix[1][1] = h;	matrix[1][2] = 0;		matrix[1][3] = 0;
	matrix[2][0] = 0;			matrix[2][1] = 0;	matrix[2][2] = q;		matrix[2][3] = 1;
	matrix[3][0] = 0;			matrix[3][1] = 0;	matrix[3][2] = -q*zn;	matrix[3][3] = 0;
}

void Matrix::look_at(Vector position, Vector target, float roll) {
	Vector forward = (target - position).normalize();
	Vector up = forward.cross_product( Vector(0,1,0).cross_product( forward ) ).normalize();
	Vector right = up.cross_product( forward ).normalize();

	matrix[0][0] = right.x;	matrix[0][1] = up.x;	matrix[0][2] = forward.x;	matrix[0][3] = 0;
	matrix[1][0] = right.y;	matrix[1][1] = up.y;	matrix[1][2] = forward.y;	matrix[1][3] = 0;
	matrix[2][0] = right.z;	matrix[2][1] = up.z;	matrix[2][2] = forward.z;	matrix[2][3] = 0;
	matrix[3][0] = 0;		matrix[3][1] = 0;		matrix[3][2] = 0;			matrix[3][3] = 1;

	Matrix rot;
	rot.rotate(Vector(0,0,roll));
	*this = rot * *this;

	Matrix trans;
	trans.translate(Vector(0,0,0)-position);
	*this *= trans;
}

Vector Matrix::get_translation() {
	return Vector( matrix[3][0], matrix[3][1], matrix[3][2] );
}

void Matrix::transponate() {
	for (int i=0; i<4; i++) {
		std::swap( matrix[i][0], matrix[0][i] );
		std::swap( matrix[i][1], matrix[1][i] );
		std::swap( matrix[i][2], matrix[2][i] );
		std::swap( matrix[i][3], matrix[3][i] );
	}
}

void Matrix::swap_rows(const unsigned row1, const unsigned row2) {
	std::swap( matrix[row1][0], matrix[row2][0] );
	std::swap( matrix[row1][1], matrix[row2][1] );
	std::swap( matrix[row1][2], matrix[row2][2] );
	std::swap( matrix[row1][3], matrix[row2][3] );
}

Vector Matrix::transform_vector(const Vector &v) {
	return Vector(
		v.x * matrix[0][0] + v.y * matrix[1][0] + v.z * matrix[2][0] + matrix[3][0],
		v.x * matrix[0][1] + v.y * matrix[1][1] + v.z * matrix[2][1] + matrix[3][1],
		v.x * matrix[0][2] + v.y * matrix[1][2] + v.z * matrix[2][2] + matrix[3][2]
	);
}

Vector Matrix::rotate_vector(const Vector &v) {
	return Vector(
		v.x * matrix[0][0] + v.y * matrix[1][0] + v.z * matrix[2][0],
		v.x * matrix[0][1] + v.y * matrix[1][1] + v.z * matrix[2][1],
		v.x * matrix[0][2] + v.y * matrix[1][2] + v.z * matrix[2][2]
	);
}

float Matrix::transform_z(const Vector &v) {
	return v.x * matrix[0][2] + v.y * matrix[1][2] + v.z * matrix[2][2] + matrix[3][2];
}

void Matrix::inverse() {
	int maxrow; 
	float maxval; 
	float val; 

	Matrix temp;
	temp.identity();

	maxval = matrix[0][0]; 
	maxrow = 0; 

	for(int i=0; i<4; i++){
		maxval = matrix[i][i]; 
		maxrow = i;
		for(int j=i+1; j<4; j++){
			val = matrix[j][i]; 
			if(fabs(val) > fabs(maxval)){
				maxval = val; 
				maxrow = j; 
			} 
		} 

		if(maxrow != i){
			swap_rows(i, maxrow); 
			temp.swap_rows(i, maxrow); 
		} 

		for(int k=0; k<4; k++){
			float div = 1.f / maxval;
			matrix[i][k] *= div;
			temp.matrix[i][k] *= div;
		} 

		for(int j=i+1; j<4; j++){
			val = matrix[j][i];
			for(k=0; k<4; k++){
				matrix[j][k] -= matrix[i][k] * val;
				temp.matrix[j][k] -= temp.matrix[i][k] * val;
			}
		}
	}

	for(int i=3; i>=0; i--) { 
		for(int j=i-1; j>=0; j--) { 
			val = matrix[j][i]; 
			for(int k=0; k<4; k++) { 
				matrix[j][k] -= matrix[i][k] * val; 
				temp.matrix[j][k] -= temp.matrix[i][k] * val; 
			} 
		} 
	} 

	memcpy( matrix, temp.matrix, 16*sizeof(float)); 
}

Matrix Matrix::operator*=(const Matrix m) {
	for (unsigned i=0; i<4; i++) {
		float d0 = matrix[0][i], d1 = matrix[1][i], d2 = matrix[2][i], d3 = matrix[3][i];
		matrix[0][i] = d0 * m.matrix[0][0] + d1 * m.matrix[0][1] + d2 * m.matrix[0][2] + d3 * m.matrix[0][3];
		matrix[1][i] = d0 * m.matrix[1][0] + d1 * m.matrix[1][1] + d2 * m.matrix[1][2] + d3 * m.matrix[1][3];
		matrix[2][i] = d0 * m.matrix[2][0] + d1 * m.matrix[2][1] + d2 * m.matrix[2][2] + d3 * m.matrix[2][3];
		matrix[3][i] = d0 * m.matrix[3][0] + d1 * m.matrix[3][1] + d2 * m.matrix[3][2] + d3 * m.matrix[3][3];
	}
	return *this;
}

Matrix Matrix::operator*(const Matrix m) const {
	float temp[4][4];
	for (unsigned i=0; i<4; i++) {
		float d0 = matrix[0][i], d1 = matrix[1][i], d2 = matrix[2][i], d3 = matrix[3][i];
		temp[0][i] = d0 * m.matrix[0][0] + d1 * m.matrix[0][1] + d2 * m.matrix[0][2] + d3 * m.matrix[0][3];
		temp[1][i] = d0 * m.matrix[1][0] + d1 * m.matrix[1][1] + d2 * m.matrix[1][2] + d3 * m.matrix[1][3];
		temp[2][i] = d0 * m.matrix[2][0] + d1 * m.matrix[2][1] + d2 * m.matrix[2][2] + d3 * m.matrix[2][3];
		temp[3][i] = d0 * m.matrix[3][0] + d1 * m.matrix[3][1] + d2 * m.matrix[3][2] + d3 * m.matrix[3][3];
	}
	return Matrix(temp);
}
