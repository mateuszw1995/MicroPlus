#pragma once
template<class T>
const T math::var<T>::PI_OVER_180 = (T)0.017453292519943295769236907684886;
template<class T>
const T math::var<T>::PI_OVER_360 = (T)0.0087266462599716478846184538424431;
template<class T>
const T math::var<T>::PI = (T)3.1415926535897932384626433832795;
template<class T>
const T math::var<T>::_180_OVER_PI = (T)57.295779513082320876798154814105;

inline void math::clip_product(const RectLTRB* b, const RectLTRB* s, RectLTRB* r){
	if((b->l > s->r || b->r < s->l || b->t > s->b || b->b < s->t))
		r->b = r->l = r->r = r->t = 0;
	else {
		r->l =	max(b->l, s->l); 
		r->t = max(b->t, s->t);
		r->r = min(b->r, s->r); 
		r->b = min(b->b, s->b);
	}
}


template <class A, class B>
void math::sort::dual_quick(A* array, B* array_2, int start, int end){ // qsort pirated by glorsors

	static A t;
	static B t2;
    if(start < end){
        int l=start+1, r=end;
		A p = array[start];
        while(l<r){
            if(array[l] <= p)
                l++;
            else if(array[r] >= p)
                r--;
            else{
                swap(array[l],array[r],t);
                swap(array_2[l],array_2[r],t2);
			}
        }
        if(array[l] < p){
            swap(array[l],array[start],t );
            swap(array_2[l],array_2[start],t2);
            l--;
        }
        else{
            l--;
            swap(array[l],array[start],t);
            swap(array_2[l],array_2[start],t2);
        }
        dual_quick(array, array_2, start, l);
        dual_quick(array, array_2, r, end);
    }
}

template <class A, class B>
 void math::sort::dual_heap(A* arr, B* arr2, unsigned int N)  
  {  
      unsigned int n = N, i = n/2, parent, child;  
      A t;  
	  B t2;
    
      for (;;) { /* Loops until arr is sorted */  
          if (i > 0) { /* First stage - Sorting the heap */  
              i--;           /* Save its index to i */  
              t = arr[i];
			  t2 = arr2[i];/* Save parent value to t */  
          } else {     /* Second stage - Extracting elements in-place */  
               n--;           /* Make the new heap smaller */  
               if (n == 0) return; /* When the heap is empty, we are done */  
               t = arr[n];
			   t2 = arr2[n];/* Save last value (it will be overwritten) */  
               arr[n] = arr[0];
			   arr2[n] = arr2[0];
			   /* Save largest value at the end of arr */  
           }  
     
           parent = i; /* We will start pushing down t from parent */  
           child = i*2 + 1; /* parent's left child */  
     
           /* Sift operation - pushing the value of t down the heap */  
           while (child < n) {  
               if (child + 1 < n  &&  arr[child + 1] > arr[child]) {  
                   child++; /* Choose the largest child */  
               }  
               if (arr[child] > t) { /* If any child is bigger than the parent */  
                   arr[parent] = arr[child];
				   arr2[parent] = arr2[child];
				   /* Move the largest child up */  
                   parent = child; /* Move parent pointer to this child */  
                   child = parent*2 + 1; /* Find the next child */  
               } else {  
                   break; /* t's place is found */  
               }  
           }  
           arr[parent] = t;
		   arr2[parent] = t2;/* We save t in the heap */  
       }  
   }  
 
template <class A>
 void math::sort::heap(A* arr, unsigned int N)
 {
      unsigned int n = N, i = n/2, parent, child;
      A t;
  
      for (;;) { /* Loops until arr is sorted */
          if (i > 0) { /* First stage - Sorting the heap */
              i--;           /* Save its index to i */
              t = arr[i];    /* Save parent value to t */
          } else {     /* Second stage - Extracting elements in-place */
              n--;           /* Make the new heap smaller */
              if (n == 0) return; /* When the heap is empty, we are done */
              t = arr[n];    /* Save last value (it will be overwritten) */
              arr[n] = arr[0]; /* Save largest value at the end of arr */
          }
  
          parent = i; /* We will start pushing down t from parent */
          child = i*2 + 1; /* parent's left child */
  
          /* Sift operation - pushing the value of t down the heap */
          while (child < n) {
              if (child + 1 < n  &&  arr[child + 1] > arr[child]) {
                  child++; /* Choose the largest child */
              }
              if (arr[child] > t) { /* If any child is bigger than the parent */
                  arr[parent] = arr[child]; /* Move the largest child up */
                  parent = child; /* Move parent pointer to this child */
                  child = parent*2 + 1; /* Find the next child */
              } else {
                  break; /* t's place is found */
              }
          }
          arr[parent] = t; /* We save t in the heap */
      }
  }

 template <class A> 
 A* math::binary_search(A* arr, A val, unsigned int N) {
	 int low = 0, high = N - 1, m = 0;
	 while (low <= high)
		 if (val == arr[m = low + (high - low)/2])
			 return arr+m;
		 else
			 val < arr[m] ? (high = m - 1) : (low = m + 1);
	 return 0;
 }


 
template <class A> void math::swap(A& a, A& b) {
	static A t;
	t = a;
	a = b;
	b = t;
}

template <class A> const A& math::max(const A& a, const A& b) {
	return a > b ? a : b;
}
template <class A> const A& math::min(const A& a, const A& b) {
	return a < b ? a : b;
}
template <class A> A math::abs(const A& a) {
	return a < 0 ? -a : a;
}

template <class A> A math::clamp(const A& _min, const A& _max, const A& a) {
	return a < _min ? _min : (a > _max ? _max : a);
}


template <class A> A math::deg2rad(A d) {
	return d*var<A>::PI_OVER_180;
}
template <class A> A math::rad2deg(A r) {
	return r*var<A>::_180_OVER_PI;
}


template <class T>
Vec3D<T> math::Bezier<T>::Interpolate(T t) {
	static Vec3D<T> v;
	static int n, i;
	n = points.size()-1;
	v = Vec3D<T>();
	
	for(i = 0; i <= n; ++i)
		v += points[i] * bernstein(n, i, t);

	return v;
}


template <class T>
Vec3D<T> math::BSpline<T>::Interpolate(T t) {

}

template <class T>
void math::BSpline<T>::Update() {
	knots.clear();
	
}

template <class R, class K>
void math::kernel::Gaussian1D(R radius, K* kernel_out, int delta_width) {
	double* kernel = new double[delta_width];
	double* weight_ptr = kernel;

	for(double i = 0; i < delta_width; ++i)
		*weight_ptr++ = (1.0/(sqrt(2.0*math::var<double>::PI)*radius))/
		double(exp((i*i)/double(2.0*radius*radius)));

	double sum = 0.0;

	for(int i = 0; i < delta_width; ++i)
		sum+=kernel[i];

	double coeff = 1.0/(sum*1.5);

	for(int i = 0; i < delta_width; ++i)
		kernel_out[i] = K(kernel[i]); 

	delete [] kernel;
}

//
//template<class T>
////NURBS<T>::Point::Point(T weight, Vec3D<T> pos) : weight(weight), pos(pos) {}
//
//template<class T>
//_vectorm<math::NURBS<T>::Point>& math::NURBS<T>::Points() {
//	return points;
//}
//
//template<class T>
//void math::NURBS<T>::Interpolate(T t) {
//}