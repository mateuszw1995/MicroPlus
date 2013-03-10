#pragma once
#include <cmath>

namespace rawMatrix {
	
	template <class M, class B>						extern inline void mult(M* mat1out, const B* const mat2);
	template <class M, class B, class C>			extern inline void mult(const M* const mat1, const B* const mat2, C* const matout);
	template <class M, class B>						extern inline void mult(const M* mat1, const B* const mat2, bool saveresult);
	template <class M, class A>						extern inline void rotY(A angle, M* matout);
	template <class M, class A>						extern inline void rotX(A angle, M* matout);
	template <class M, class A>						extern inline void rotZ(A angle, M* matout);
	template <class M, class X, class Y, class Z>	extern inline void trans(X x, Y y, Z z, M* matout);
	template <class M, class X, class Y, class Z>	extern inline void scale(X x, Y y, Z z, M* matout);
	template <class M>								extern inline void identity(M* matout);
	template <class M, class P, class W>			extern inline void multiply_point(const M* const mat, P* const point, const W w);
	template <class M, class P, class Po, class W>  extern inline void multiply_point(const M* const mat, const P* const point, Po* const out, const W w);
};

template <typename T> struct Quaternion; 
template <typename T> struct Vec3D; 

template <class M = float>
struct Matrix{

	static M _mul[16];
	static M LAST_RESULT[16];

	M MATRIX[16];
	Matrix();
	template <class B>
	Matrix(const Quaternion<B>&);
	Matrix(const Vec3D<M>& axis, M angle);
	Matrix(M yaw, M pitch, M roll);
	
	template <class B>
	Matrix& operator=(const Quaternion<B>&);

	inline void identity		();
	
	template <class T>
	inline void	scale			(Vec3D<T>);
	template <class T>
	inline void	trans			(Vec3D<T>);
	template <class T>
	inline void	xrot			(T);
	template <class T>
	inline void	yrot			(T);
	template <class T>
	inline void	zrot			(T);
	template <class T>
	inline void	frustum			(T, T, T, T, T, T);
	template <class T>
	inline void	ortho			(T, T, T, T, T, T);
	template <class T>
	inline void	perspective		(T, T, T, T);
	template <class T>
	inline void	lookat			(Vec3D<T>, Vec3D<T>, Vec3D<T>);
	
	template <class B>
	Matrix operator*(const Matrix<B>&) const;
	template <class B>
	Matrix& operator*=(const Matrix<B>&);
	template <class B>
	Matrix operator*(const Quaternion<B>&) const;
	template <class B>
	Matrix& operator*=(const Quaternion<B>&);

	Matrix& transpose();
};

namespace rawVector {
	template <class T> inline void normalize3D(T* vec3f);
	template <class T> inline void normalize2D(T* vec2f);
	template <class A, class B, class R> inline void cross(A* a, B* b, R* out);
	template <class A, class B> inline double dot(A* u, B* v);
	template <class A, class R> inline void normal(A* v, R* out);
};

template <class T = float>
struct Vec3D {
	T x,y,z;

	Vec3D(T x = 0.0, T y = 0.0, T z = 0.0);
	
	template <class B>
	Vec3D(const Vec3D<B>& v) : x(v.x), y(v.y), z(v.z) {};
	template <class B>
	Vec3D& operator=(const Vec3D<B>& v) {x = v.x, y = v.y, z = v.z; return *this;}

	T magnitude() const;
	T magnitude_sq() const;
	Vec3D& normalize();
	
	template <class B>
	Vec3D cross(const Vec3D<B>& b) const;
	
	template <class B>
	T dot(const Vec3D<B>& b);
	
	Vec3D operator-() const;
	
	template <class B>
	Vec3D operator-(const Vec3D<B>&) const;
	template <class B>
	Vec3D operator-=(const Vec3D<B>&);
	template <class B>
	Vec3D operator+(const Vec3D<B>&) const;
	template <class B>
	Vec3D& operator+=(const Vec3D<B>&);

	template <class B>
	Vec3D operator*(B) const;
	template <class B>
	Vec3D& operator*=(B);
	template <class B>
	Vec3D operator*(const Vec3D<B>&) const;
	template <class B>
	Vec3D& operator*=(const Vec3D<B>&);
	template <class B>
	Vec3D operator*(const Matrix<B>&) const;
	template <class B>
	Vec3D& operator*=(const Matrix<B>&);
	template <class B>
	Vec3D operator*(const Quaternion<B>&) const;
	template <class B>
	Vec3D& operator*=(const Quaternion<B>&);
};

template <class T = double>
struct Quaternion {
	T x,y,z,w;
	
	Quaternion();
	Quaternion(T x, T y, T z, T w);
	template <class B, class A>
	Quaternion(const Vec3D<B>& axis, A angle);
	Quaternion(T yaw, T pitch, T roll);
	template <class B>
	Quaternion(const Matrix<B>& matrix);
	
	template <class B>
	Quaternion operator*(const Quaternion<B>&) const;
	template <class B>
	Quaternion& operator*=(const Quaternion<B>&);
	template <class B>
	Quaternion operator+(const Quaternion<B>&) const;
	template <class B>
	Quaternion& operator+=(const Quaternion<B>&);
	
	template <class B>
	T dot(const Quaternion<B>& b) const;

	template <class B, class A>
	Quaternion slerp(Quaternion<B>, A t) const;

	template <class B, class A>
	Quaternion nlerp(Quaternion<B>, A t) const;

	Quaternion conjugate() const;
	Quaternion operator-() const;
	
	void scale(T scalar);
	Quaternion operator*(T scalar) const;
	Quaternion& operator*=(T scalar); 

	T magnitude() const;
	T magnitude_sq() const;
	Quaternion& normalize();
	
	Vec3D<T> getXAxis();
	Vec3D<T> getYAxis();
	Vec3D<T> getZAxis();
	
	Vec3D<T> getYawPitchRoll();
	Vec3D<T> getAxisAngle(T& angle);
};

template <class T = float, class Q = double>
struct Transformation {
	Quaternion<Q> rotation;
	Vec3D<T> position, scale;
	Matrix<T> matrix;

	Transformation();

	void UpdatePosition();
	void Scale();
	void UpdateMatrix();
};


#include "Transformation.hpp"