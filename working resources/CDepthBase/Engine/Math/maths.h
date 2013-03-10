#pragma once
#undef max
#undef min
#include "../Container/vector.h"

namespace math {

	template<class T>
	struct var {
		static const T PI_OVER_180;
		static const T _180_OVER_PI;
		static const T PI_OVER_360;
		static const T PI;
	};

	struct RectXYWH {
		int x,y,w,h;
	};

	struct RectLTRB {
		int l,t,r,b;
	};

	struct RectXYWHf : public RectXYWH {
		bool flip;
	};

	struct RectWH {
		int w,h;
	};

	namespace sort {
		template <class A, class B> void dual_quick(A* array, B* array_2, int start, int end);
		template <class A, class B> void dual_heap(A* arr, B* arr2, unsigned int N);
		template <class A> void heap(A* arr, unsigned int N); 
	}

	inline void clip_product(const RectLTRB* b, const RectLTRB* s, RectLTRB* r);
	template <class A> A* binary_search(A* arr, A val, unsigned int N); 

	template <class A> void swap(A& a, A& b);
	template <class A> const A& max(const A& a, const A& b);
	template <class A> const A& min(const A& a, const A& b);
	template <class A> A abs(const A& a);
	template <class A> A clamp(const A& _min, const A& _max, const A& a);
	template <class A> A deg2rad(A);
	template <class A> A rad2deg(A);

	RectWH rectangle2Dpack(RectXYWHf* arr, int n, bool sortarea, int stride, int nrec, int maxl); 

	namespace kernel {
		template <class R, class K> 
		void Gaussian1D(R radius, K* kernel_out, int delta_width);
	};

}

#include "../Math/Transformation.h"

namespace math {
	
	template <class A, class T>
	A lerp(A a, A b, T t) {
		return a*(1.0-t) + b*t;
	}

	template <class T> 
	T power(T a, int p) {
		static T aa;
		aa = a;
		if(!p) return 1;
		if(p < 0) {
			a = 1.0/a;
			p = -p;
		}
		while(--p)
			a *= aa;
		return a;
	}
	
	template <class T>
	T factorial(T a) {
		static T b;
		if(!a) return 1;
		b = 1;
		while(--a) b *= (a+1);
		return b;
	}
	
	
	template <class T>
	T deBoorCoxRecursion(int n, int i, T t) {

	}

	template <class T>
	T binomial(T n, T k) {
		return factorial(n) / (factorial(k) * factorial(n-k));
	}

	template <class T, class B>
	B bernstein(T n, T i, B t) {
		return binomial(n, i)*power(t, i)*power(1.0-t, n-i); 
	}

	template <class T = double>
	struct Curve3D {
		_vectorm<Vec3D<T> > points;
		inline _vectorm<Vec3D<T> >& Points() {return points;}

		virtual Vec3D<T> Interpolate(T t) = 0;
		Vec3D<T> alignInterpolate(T t) {
			return Interpolate(t)-points[0];
		}
	};


	template <class T = double>
	struct NURBS : public Curve3D<T> {
		Vec3D<T> Interpolate(T t);
	};

	template <class T = double>
	struct Bezier : public Curve3D<T> {
		Vec3D<T> Interpolate(T t);
	};	
	
	template <class T = double>
	struct BSpline : public Curve3D<T> {
		int degree;
		BSpline(int degree = 2) : degree(degree) {};

		_vectorm<T> knots;

		void Update();
		Vec3D<T> Interpolate(T t);
	};
}

#include "maths.hpp"