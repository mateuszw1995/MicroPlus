#pragma once

//#include "CMatrix.h"
//#include <cmath>
#include <cstring>

template <class M>
M Matrix<M>::_mul[16];
template <class M>
M Matrix<M>::LAST_RESULT[16];

template <class M, class B>
void rawMatrix::mult(M *m, const B* const mult){ 
	static int i,j,k;

	memset(Matrix<M>::LAST_RESULT, 0, 16*sizeof(M));
	i = j = k = 0;

	for(i = 0; i < 4; ++i) 
		for( j = 0; j < 4; ++j)
			for( k = 0; k < 4; ++k) 
				Matrix<M>::LAST_RESULT[4*i+j] += M(mult[4*i+k] * m[4*k+j]);

	memcpy(m, Matrix<M>::LAST_RESULT, 16*sizeof(M));

}

template <class M, class B>
void rawMatrix::mult(const M *m, const B* const mult, bool){ 
	memset(Matrix<M>::LAST_RESULT, 0, 16*sizeof(M));
	mult(m, mult, Matrix<M>::LAST_RESULT>);
}

template <class M, class B, class C>
void rawMatrix::mult(const M * const a, const B* const b, C* const out){ 
	static int i,j,k;

	memset(out, 0, 16*sizeof(M));
	i = j = k = 0;

	for(i = 0; i < 4; ++i) 
		for( j = 0; j < 4; ++j)
			for( k = 0; k < 4; ++k) 
				out[4*i+j] += b[4*i+k] * a[4*k+j];

}

template <class M, class A>
void rawMatrix::rotY(A yrot, M* m){
	yrot *= math::var<M>::PI_OVER_180;  

	memset(Matrix<M>::_mul, 0, 15*sizeof(M)); 
	Matrix<M>::_mul[0] = cos(yrot);
	Matrix<M>::_mul[2] = sin(yrot);
	Matrix<M>::_mul[5] = 1;
	Matrix<M>::_mul[8] = -sin(yrot);
	Matrix<M>::_mul[10] = cos(yrot);
	Matrix<M>::_mul[15] = 1;
	mult(m, Matrix<M>::_mul);
}


template <class M, class A>
void rawMatrix::rotX(A xrot, M* m){
	xrot *= math::var<M>::PI_OVER_180;  

	memset(Matrix<M>::_mul, 0, 15*sizeof(M)); 
	Matrix<M>::_mul[0] = 1;
	Matrix<M>::_mul[5] = cos(xrot);
	Matrix<M>::_mul[6] = -sin(xrot);
	Matrix<M>::_mul[9] = sin(xrot);
	Matrix<M>::_mul[10] = cos(xrot);
	Matrix<M>::_mul[15] = 1;
	mult(m, Matrix<M>::_mul);
}


template <class M, class A>
void rawMatrix::rotZ(A zrot, M* m){
	zrot *= math::var<M>::PI_OVER_180;  

	memset(Matrix<M>::_mul, 0, 15*sizeof(M)); 
	Matrix<M>::_mul[0] = cos(zrot);
	Matrix<M>::_mul[1] = -sin(zrot);
	Matrix<M>::_mul[4] = sin(zrot);
	Matrix<M>::_mul[5] = cos(zrot);
	Matrix<M>::_mul[10] = 1;
	Matrix<M>::_mul[15] = 1;
	mult(m, Matrix<M>::_mul);
}

template <class M, class X, class Y, class Z>
void rawMatrix::trans(X x, Y y, Z z, M* m){

	memset(Matrix<M>::_mul, 0, 15*sizeof(M)); 
	Matrix<M>::_mul[0] = 1;
	Matrix<M>::_mul[5] = 1;
	Matrix<M>::_mul[10] = 1;
	Matrix<M>::_mul[12] = x;
	Matrix<M>::_mul[13] = y;
	Matrix<M>::_mul[14] = z;
	Matrix<M>::_mul[15] = 1;
	mult(m, Matrix<M>::_mul);
}

template <class M, class X, class Y, class Z>
void rawMatrix::scale(X x, Y y, Z z, M* m){
	memset(Matrix<M>::_mul, 0, 15*sizeof(M)); 
	Matrix<M>::_mul[0] = x;
	Matrix<M>::_mul[5] = y;
	Matrix<M>::_mul[10] = z;
	Matrix<M>::_mul[15] = 1;
	mult(m, Matrix<M>::_mul);
}

template <class M>
void rawMatrix::identity(M* m){
	memset(m, 0, 15*sizeof(M)); 
	m[0] = 1;
	m[5] = 1;
	m[10] = 1;
	m[15] = 1;
}

template <class M, class P, class W>
void rawMatrix::multiply_point(const M* m, P* const p, const W w = 1.0) {
	static P pt[3];
	pt[0] = p[0] * m[0] + p[1] * m[4] +p[2] * m[8] + w * m[12];
	pt[1] = p[0] * m[1] + p[1] * m[5] + p[2] * m[9] + w * m[13];
	pt[2] = p[0] * m[2] + p[1] * m[6] + p[2] * m[10] + w * m[14];
	memcpy(p, pt, 3*sizeof(P));
}

template <class M, class P, class Po, class W>
void rawMatrix::multiply_point(const M* m, const P* const p, Po* const out, const W w = 1.0) {
	out[0] = p[0] * m[0] + p[1] * m[4] + p[2] * m[8] + w * m[12];
	out[1] = p[0] * m[1] + p[1] * m[5] + p[2] * m[9] + w * m[13];
	out[2] = p[0] * m[2] + p[1] * m[6] + p[2] * m[10] + w * m[14];
}


template <class M>
Matrix<M>& Matrix<M>::transpose() {
	static Matrix<M> m;
	m = *this;
	for(int i = 0; i < 4; ++i) 
		for(int j = 0; j < 4; ++j)
			MATRIX[4*i+j] = m.MATRIX[4*j+i];

	return *this;
}

template <class M>
template <class B>
Matrix<M> Matrix<M>::operator*(const Matrix<B>& b) const {
	static Matrix m;
	rawMatrix::mult(MATRIX, b.MATRIX, m.MATRIX);
	return m;
}
template <class M>
template <class B>
Matrix<M>& Matrix<M>::operator*=(const Matrix<B>& b) {
	rawMatrix::mult(MATRIX, b.MATRIX);
	return *this;
}

template <class M>
template <class B>
Matrix<M> Matrix<M>::operator*(const Quaternion<B>& b) const {
	return operator*(Matrix<M>(b));
}
template <class M>
template <class B>
Matrix<M>& Matrix<M>::operator*=(const Quaternion<B>& b) {
	return operator*=(Matrix<M>(b));
}


template <class M>
Matrix<M>::Matrix() {
	identity();
}

template <class M>
Matrix<M>::Matrix(const Vec3D<M>& a, M angle) {
	static M sint, cost, _1cost, ux, uy, uz;
	static Vec3D<M> ax;
	angle = math::deg2rad(angle);
	sint = sin(angle), cost = cos(angle), _1cost = 1.0-cost;
	ax = a;
	ax.normalize();
	ux = ax.x, uy = ax.y, uz = ax.z;

	identity();
	MATRIX[0] = cost+ux*ux*_1cost;
	MATRIX[1] = ux*uy*_1cost-uz*sint;
	MATRIX[2] = ux*uz*_1cost+uy*sint;
	MATRIX[4] = uy*ux*_1cost+uz*sint;
	MATRIX[5] = cost+uy*uy*_1cost;
	MATRIX[6] = uy*uz*_1cost-ux*sint;
	MATRIX[8] = uz*ux*_1cost-uy*sint;
	MATRIX[9] = uz*uy*_1cost+ux*sint;
	MATRIX[10] = cost+uz*uz*_1cost;
}


template <class M>
Matrix<M>::Matrix(M yaw, M pitch, M roll) {
	static M sp, cp, sy, cy, sr, cr;
	identity();
	pitch = math::deg2rad(pitch);
	yaw = math::deg2rad(yaw);
	roll = math::deg2rad(roll);
	sy = sin(roll); // flight to tait-bryan
	sr = sin(pitch);
	sp = sin(yaw);	
	cy = cos(roll);
	cr = cos(pitch);
	cp = cos(yaw);
	
	MATRIX[0] = cp*cy;
	MATRIX[1] = cr*sy+sr*sp*sy;
	MATRIX[2] = sr*sy-cr*sp*cy;
	MATRIX[4] = -cp*sy;
	MATRIX[5] = cr*cy-sr*sp*sy;
	MATRIX[6] = sr*cy+cr*sp*sy;
	MATRIX[8] = sp;
	MATRIX[9] = -sr*cp;
	MATRIX[10] = cr*cp;

}


template <class M>
template <class B>
Matrix<M>::Matrix(const Quaternion<B>& q) {
	operator=(q);
}

template <class M>
template <class B>
Matrix<M>& Matrix<M>::operator=(const Quaternion<B>& q) {
	const M& q1 = q.x, &q2 = q.y, &q3 = q.z, &q4 = q.w;
	static float q1q2, q1q3, q2q3, q1q4, q4q4, q3q4, q2q4;
	identity();
	q1q2 = q1*q2;  
	q1q3 = q1*q3; 
	q2q3 = q2*q3; 
	q1q4 = q1*q4; 
	q4q4 = q4*q4; 
	q3q4 = q3*q4; 
	q2q4 = q2*q4;
	MATRIX   [0] = -1.0+2.0*(q1*q1 + q4q4);
	MATRIX   [1] =		2.0*(q1q2  - q3q4); 
	MATRIX   [2] =		2.0*(q1q3  + q2q4);
	MATRIX   [4] =		2.0*(q1q2  + q3q4); 
	MATRIX   [5] = -1.0+2.0*(q2*q2 + q4q4);
	MATRIX   [6] =		2.0*(q2q3  - q1q4); 
	MATRIX   [8] =		2.0*(q1q3  - q2q4); 
	MATRIX   [9] =		2.0*(q1q4  + q2q3);
	MATRIX   [10]= -1.0+2.0*(q3*q3 + q4q4);

	return *this;
}

template <class M>
void Matrix<M>::identity(){
	rawMatrix::identity(MATRIX);
}

template <class M>
template <class T>
void Matrix<M>::xrot(T r){
	rawMatrix::rotX(r, MATRIX);
}
template <class M>
template <class T>
void Matrix<M>::yrot(T r){
	rawMatrix::rotY(r, MATRIX);
}
template <class M>
template <class T>
void Matrix<M>::zrot(T r){
	rawMatrix::rotZ(r, MATRIX);
}
template <class M>
template <class T>
void Matrix<M>::trans(Vec3D<T> v){
	rawMatrix::trans(v.x, v.y, v.z, MATRIX);
}
template <class M>
template <class T>
void Matrix<M>::scale(Vec3D<T> v){
	rawMatrix::scale(v.x, v.y, v.z, MATRIX);
}
template <class M>
template <class T>
void Matrix<M>::frustum(T  	left, 
	T  	right, 
	T  	bottom, 
	T  	top, 
	T  	nearVal, 
	T  	farVal){
		static T m[16],
			_2n,     
			_1over_rml,
			_1over_fmn,
			_1over_tmb;

		_2n = 2.0 * nearVal;
		_1over_rml = 1.0 / (right - left);
		_1over_fmn = 1.0 / (farVal - nearVal);
		_1over_tmb = 1.0 / (top - bottom);

		m[0] = _2n * _1over_rml;
		m[1] = 0.0;
		m[2] = 0.0;
		m[3] = 0.0;

		m[4] = 0.0;
		m[5] = _2n * _1over_tmb;
		m[6] = 0.0;
		m[7] = 0.0;

		m[8] = (right + left) * _1over_rml;
		m[9] = (top + bottom) * _1over_tmb;
		m[10] = (-(farVal + nearVal)) * _1over_fmn;
		m[11] = -1.0;

		m[12] = 0.0;
		m[13] = 0.0;
		m[14] = -(_2n * farVal * _1over_fmn);
		m[15] = 0.0;

		rawMatrix::mult(MATRIX, m);
}
template <class M>
template <class T>
void Matrix<M>::ortho(T left, T right, T bottom, T top, T zNear, T zFar) {
	static T o[16] = {
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		0,0,0,1
	};
	o[0] = 2.0/(right-left);
	o[12] = -(right+left)/(right-left);
	o[5] = 2.0/(top-bottom);
	o[13] = -(top+bottom)/(top-bottom);
	o[10] = -2.0/(zFar-zNear);
	o[14] = -(zFar+zNear)/(zFar-zNear);
	rawMatrix::mult(MATRIX, o);
}
template <class M>
template <class T>
void Matrix<M>::perspective(T fovy, T aspect, T zNear, T zFar){
	static T xmin, xmax, ymin, ymax;
	static T piover360 = T(math::var<T>::PI / 360.0);
	ymax = zNear * tan(fovy * piover360);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;
	frustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

template <class M>
template <class T>
void Matrix<M>::lookat(Vec3D<T> eye, Vec3D<T> center, Vec3D<T> up) {
	using namespace rawVector;
	static T m[16];
	static Vec3D<T> x, z;

	z = (eye - center).normalize();
	x = (up * z).normalize();
	up = (z * x).normalize();

	memset(m, 0, 15*sizeof(M));

#define T(row,col)  m[col*4+row]
	T(0, 0) = x.x;
	T(0, 1) = x.y;
	T(0, 2) = x.z;
	T(1, 0) = up.x;
	T(1, 1) = up.y;
	T(1, 2) = up.z;
	T(2, 0) = z.x;
	T(2, 1) = z.y;
	T(2, 2) = z.z;
	T(3, 3) = 1.0;
#undef T

	rawMatrix::mult(MATRIX, m);
	trans(-eye);
}









template <class T>
void rawVector::normalize3D(T* v) {				
	static T len;
	len = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	len = (len ? len = 1.0/len : len = 1.0);

	v[0] *= len;						
	v[1] *= len;						
	v[2] *= len;						
}

template <class T>
void rawVector::normalize2D(T* v) {				
	static T len;
	len = sqrt(v[0]*v[0] + v[1]*v[1]);
	len = (len ? len = 1.0/len : len = 1.0);

	v[0] *= len;						
	v[1] *= len;							
}



template <class A, class B, class R> 
inline void rawVector::cross(A* a, B* b, R* out){
	out[0] = a[1]*b[2] - a[2]*b[1];
	out[1] = a[2]*b[0] - a[0]*b[2];
	out[2] = a[0]*b[1] - a[1]*b[0];
}


template <class A, class B> 
inline double rawVector::dot(A* u, B* v){
	return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
}

template <class A, class R> 
inline void rawVector::normal(A* v, R* out){
	static float cnrmv1[3],cnrmv2[3];
	cnrmv1[0] = v[0+0] - v[3+0];
	cnrmv1[1] = v[0+1] - v[3+1];
	cnrmv1[2] = v[0+2] - v[3+2];
	cnrmv2[0] = v[3+0] - v[6+0];
	cnrmv2[1] = v[3+1] - v[6+1];
	cnrmv2[2] = v[3+2] - v[6+2];
	out[0] = cnrmv1[1]*cnrmv2[2] - cnrmv1[2]*cnrmv2[1];
	out[1] = cnrmv1[2]*cnrmv2[0] - cnrmv1[0]*cnrmv2[2];
	out[2] = cnrmv1[0]*cnrmv2[1] - cnrmv1[1]*cnrmv2[0];

	normalize3D(out);						
}



template <class T>
Vec3D<T>::Vec3D(T x, T y, T z) : x(x), y(y), z(z) {}

template <class T>
template <class B>
Vec3D<T> Vec3D<T>::cross(const Vec3D<B>& b) const {
	return Vec3D(y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x);
}

template <class T>
template <class B>
T Vec3D<T>::dot(const Vec3D<B>& b) {
	return x*b.x + y*b.y, z*b.z;
}

template <class T>
T Vec3D<T>::magnitude() const {
	return sqrt(magnitude_sq());
}
template <class T>
T Vec3D<T>::magnitude_sq() const {
	return x*x + y*y + z*z;
}

template <class T>
Vec3D<T>& Vec3D<T>::normalize() {
	rawVector::normalize3D(&x);
	return *this;
}

template <class T>
Vec3D<T> Vec3D<T>::operator-() const {
	return Vec3D<T>(-x, -y, -z);
}

template <class T>
template <class B>
Vec3D<T> Vec3D<T>::operator*(B b) const {
	return Vec3D<T>(x*b, y*b, z*b);
}
template <class T>
template <class B>
Vec3D<T>& Vec3D<T>::operator*=(B b) {
	return *this = Vec3D<T>(x*b, y*b, z*b);
}
template <class T>
template <class B>
Vec3D<T> Vec3D<T>::operator*(const Vec3D<B>& b) const {
	return cross(b);
}
template <class T>
template <class B>
Vec3D<T>& Vec3D<T>::operator*=(const Vec3D<B>& b) {
	return *this = cross(b);
}
template <class T>
template <class B>
Vec3D<T> Vec3D<T>::operator+(const Vec3D<B>& b) const {
	return Vec3D<T>(x+b.x, y+b.y, z+b.z);
}
template <class T>
template <class B>
Vec3D<T> Vec3D<T>::operator-(const Vec3D<B>& b) const {
	return Vec3D<T>(x-b.x, y-b.y, z-b.z);
}
template <class T>
template <class B>
Vec3D<T> Vec3D<T>::operator-=(const Vec3D<B>& b) {
	return *this = Vec3D<T>(x-b.x, y-b.y, z-b.z);
}
template <class T>
template <class B>
Vec3D<T>& Vec3D<T>::operator+=(const Vec3D<B>& b) {
	return *this = Vec3D<T>(x+b.x, y+b.y, z+b.z);
}
template <class T>
template <class B>
Vec3D<T> Vec3D<T>::operator*(const Matrix<B>& b) const {
	static Vec3D v;
	rawMatrix::multiply_point(b.MATRIX, (T*)&x, (T*)&v.x, 1.0);
	return v;
}
template <class T>
template <class B>
Vec3D<T>& Vec3D<T>::operator*=(const Matrix<B>& b) {
	rawMatrix::multiply_point(b.MATRIX, (T*)&x, 1.0);
	return *this;
}

template <class T>
template <class B>
Vec3D<T> Vec3D<T>::operator*(const Quaternion<B>& b) const {
	static Matrix<T> m;
	m = Matrix<T>(b);
	return this->operator*(m);
}

template <class T>
template <class B>
Vec3D<T>& Vec3D<T>::operator*=(const Quaternion<B>& b) {
	static Matrix<T> m;
	m = Matrix<T>(b);
	return this->operator*=(m);
}

template <class T>
Quaternion<T>& Quaternion<T>::normalize() {
	scale(1.0/magnitude());
	return *this;
}

template <class T>
T Quaternion<T>::magnitude() const {
	return sqrt(magnitude_sq());
}

template <class T>
T Quaternion<T>::magnitude_sq() const {
	return x*x + y*y + z*z + w*w;
}


template <class T>
template <class B>
Quaternion<T> Quaternion<T>::operator*(const Quaternion<B>& b) const {
	return Quaternion<T>(
		w*b.x + x*b.w - y*b.z + z*b.y,
		w*b.y + x*b.z + y*b.w - z*b.x,
		w*b.z - x*b.y + y*b.x + z*b.w,
		w*b.w - x*b.x - y*b.y - z*b.z);
}

template <class T>
template <class B>
Quaternion<T>& Quaternion<T>::operator*=(const Quaternion<B>& b) {
	return *this = operator*(b);
}


template <class T>
template <class B>
Quaternion<T> Quaternion<T>::operator+(const Quaternion<B>& b) const {
	return Quaternion<T>(x+b.x, y+b.y, z+b.z, w+b.w);
}

template <class T>
template <class B>
Quaternion<T>& Quaternion<T>::operator+=(const Quaternion<B>& b) {
	return *this = operator*(b);
}


template <class T>
Quaternion<T>::Quaternion() : x(0.0), y(0.0), z(0.0), w(1.0) {}

template <class T>
Quaternion<T>::Quaternion(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

template <class T>
template <class B, class A>
Quaternion<T>::Quaternion(const Vec3D<B>& axis, A angle) {
	static A sinangle;
	static Vec3D<B> vec;
	angle = math::deg2rad(angle) * .5;
	sinangle = sin(angle);

	vec = axis;
	vec.normalize();
	x = vec.x*sinangle; 
	y = vec.y*sinangle; 
	z = vec.z*sinangle; 
	w = cos(angle);
}

template <class T>
Quaternion<T>::Quaternion(T yaw, T pitch, T roll) {
	static T sp, cp, sy, cy, sr, cr, add, diff;
	pitch = math::deg2rad(pitch) * 0.5;
	yaw = math::deg2rad(yaw) * 0.5;
	roll = math::deg2rad(roll) * 0.5;
	sy = sin(roll); // flight to tait-bryan
	sr = sin(pitch);
	sp = sin(yaw);	
	cy = cos(roll);
	cr = cos(pitch);
	cp = cos(yaw);
	w = cr*cp*cy+sr*sp*sy;
	x = sr*cp*cy-cr*sp*sy;
	y = cr*sp*cy+sr*cp*sy;
	z = cr*cp*sy-sr*sp*cy;
}

template <class T>
template <class B>
Quaternion<T>::Quaternion(const Matrix<B>& m) {
    T s = 0.0;
    T q[4] = {0.0};
    T trace = m.MATRIX[0] + m.MATRIX[5] + m.MATRIX[10];

    if (trace > 0.0)
    {
        s = sqrt(trace + 1.0);
        q[3] = s * 0.5;
        s = 0.5 / s;
        q[0] = (m.MATRIX[1*4+2] - m.MATRIX[2*4+1]) * s;
        q[1] = (m.MATRIX[2*4+0] - m.MATRIX[0*4+2]) * s;
        q[2] = (m.MATRIX[0*4+1] - m.MATRIX[1*4+0]) * s;
    }
    else
    {
        int nxt[3] = {1, 2, 0};
        int i = 0, j = 0, k = 0;

        if (m.MATRIX[1*4+1] > m.MATRIX[0*4+0])
            i = 1;

        if (m.MATRIX[2*4+2] > m.MATRIX[i*4+i])
            i = 2;

        j = nxt[i];
        k = nxt[j];
        s = sqrtf((m.MATRIX[i*4+i] - (m.MATRIX[j*4+j] + m.MATRIX[k*4+k])) + 1.0);

        q[i] = s * 0.5;
        s = 0.5 / s;
        q[3] = (m.MATRIX[j*4+k] - m.MATRIX[k*4+j]) * s;
        q[j] = (m.MATRIX[i*4+j] + m.MATRIX[j*4+i]) * s;
        q[k] = (m.MATRIX[i*4+k] + m.MATRIX[k*4+i]) * s;
    }

    x = q[0], y = q[1], z = q[2], w = q[3];
}

template <class T>
Quaternion<T> Quaternion<T>::conjugate() const {
	return Quaternion(-x, -y, -z, w);
}
template <class T>
Quaternion<T> Quaternion<T>::operator-() const {
	return Quaternion(-x, -y, -z, w);
}

template <class T>
void Quaternion<T>::scale(T scalar) {
	x *= scalar; y *= scalar; z *= scalar; w *= scalar;
}

template <class T>
Quaternion<T> Quaternion<T>::operator*(T scalar) const {
	return Quaternion(x*scalar, y*scalar, z*scalar, w*scalar);
}
template <class T>
Quaternion<T>& Quaternion<T>::operator*=(T scalar) {
	scale(scalar);
}

template <class T>
template <class B>
T Quaternion<T>::dot(const Quaternion<B>& b) const {
	return x*b.x + y*b.y + z*b.z + w*b.w;
}


template <class T>
template <class B, class A>
Quaternion<T> Quaternion<T>::slerp(Quaternion<B> b, A t) const {
	static T theta, sinTheta;
	theta    = acos(dot(b));
	sinTheta = sin(theta);
	return sinTheta > 0.001f ? Quaternion<T>((*this)*(sin((1.0-t)*theta)/sinTheta) + b*(sin(t*theta)/sinTheta)) : nlerp(b, t);
}

template <class T>
template <class B, class A>
Quaternion<T> Quaternion<T>::nlerp(Quaternion<B> b, A t) const {
	return Quaternion((*this)*(1.0-t) + b*t).normalize();
}


template <class T>
Vec3D<T> Quaternion<T>::getXAxis() {
	return Vec3D<T>(-1.0+2.0*(x*x + w*w), 2.0*(x*y  + z*w), 2.0*(x*z  - y*w));
}
template <class T>
Vec3D<T> Quaternion<T>::getYAxis() {
	return Vec3D<T>(2.0*(x*y  - z*w), -1.0+2.0*(y*y + w*w), 2.0*(x*w  + y*z));
}
template <class T>
Vec3D<T> Quaternion<T>::getZAxis() {
	return Vec3D<T>(2.0*(x*z  + y*w), 2.0*(y*z  - x*w), -1.0+2.0*(z*z + w*w));
}

template <class T>
Vec3D<T> Quaternion<T>::getYawPitchRoll() {
	static T q0, q1, q2, q3;
	q0 = w, q1 = x, q2 = y, q3 = z;
	return Vec3D<>(math::rad2deg(asin(2*(q0*q2-q3*q1))), math::rad2deg(atan2(2*(q0*q1 + q2*q3), 1 - 2*(q1*q1 + q2*q2))), math::rad2deg(atan2(2*(q0*q3 + q1*q2), 1 - 2*(q2*q2 + q3*q3))));
}
template <class T>
Vec3D<T> Quaternion<T>::getAxisAngle(T& angle) {
	angle = 2*math::rad2deg(acos(w));
	return Vec3D<>(x, y, z).normalize();
}

template <class T, class Q>
Transformation<T,Q>::Transformation() : scale(Vec3D<T>(1.0, 1.0, 1.0)) {
}

template <class T, class Q>
void Transformation<T,Q>::UpdateMatrix() {
	matrix = rotation;
	UpdatePosition();
	Scale();
}
template <class T, class Q>
void Transformation<T,Q>::Scale() {
	matrix.MATRIX[0] *= scale.x;
	matrix.MATRIX[5] *= scale.y;
	matrix.MATRIX[10] *= scale.z; 
}

template <class T, class Q>
void Transformation<T,Q>::UpdatePosition() {
	memcpy(matrix.MATRIX+12, &position, sizeof(Vec3D<T>));
}



	/*
	
	static A coshalf, half, sinhalf, ratioA, ratioB;
	
	coshalf = w * b.w + x * b.x + y * b.y + z * b.z;

	if (math::abs(coshalf) >= 1.0)
		return Quaternion(*this);
	
	if (coshalf < 0.0) {
		b = b.conjugate();
		coshalf = -coshalf;
	}

	half = acos(coshalf);
	sinhalf = sqrt(1.0 - coshalf*coshalf);

	if (math::abs(sinhalf) < 0.001){
		return Quaternion(x * 0.5 + b.x * 0.5,
						  y * 0.5 + b.y * 0.5,
						  z * 0.5 + b.z * 0.5,
						  w * 0.5 + b.w * 0.5);
	}

	sinhalf = 1.0/sinhalf;
	ratioA = sin((1 - t) * half) * sinhalf;
	ratioB = sin(t * half) * sinhalf; 

	return Quaternion(x * ratioA + b.x * ratioB,
					  y * ratioA + b.y * ratioB,
					  z * ratioA + b.z * ratioB,
					  w * ratioA + b.w * ratioB).normalize();*/