#pragma once
#include "value_animator.h"

namespace db {
	namespace misc {
		animator::method::method(float left_x, float right_x, float (*increasing_func)(float)) 
			: increasing_func(increasing_func), left_x(left_x), diff_x(right_x - left_x), left_y(increasing_func(left_x)), diff_y(increasing_func(right_x) - left_y) {}

		float animator::method::linear(float x) {
			return x;
		}
		
		float animator::method::sinusoidal(float x) {
			return sin(x);
		}

		float animator::method::quadratic(float x) {
			return -(x*x);
		}

		float animator::method::hyperbolic(float x) {
			return -1.f/x;
		}
		
		float animator::method::logarithmic(float x) {
			return log(x);
		}

		float animator::method::exponential(float x) {
			return -exp(-x);
		}

		animator::method 
			animator::LINEAR(0.f, 1.f, animator::method::linear), 
			animator::QUADRATIC(-1.f, 0.f, animator::method::quadratic), 
			animator::SINUSOIDAL(-3.14159265f/2., 3.14159265f/2., animator::method::sinusoidal), 
			animator::HYPERBOLIC(-2.f, -0.5f, animator::method::hyperbolic), 
			animator::LOGARITHMIC(0.05f, 5.f, animator::method::logarithmic),
			animator::EXPONENTIAL(-3.f, 1.f, animator::method::exponential); 

		animator::animator(float* val, float init_val, float desired_val, float miliseconds, method how) 
			: val(val), init_val(init_val), diff(desired_val - init_val), miliseconds(miliseconds), how(how) {
		}

		void animator::start() {
			tm.microseconds();
		}

		bool animator::animate() {
			float ms = float(tm.get_miliseconds());
			bool finished = ms >= miliseconds;
			*val = finished ? init_val + diff : init_val + diff * (how.increasing_func(how.left_x + ms / miliseconds * how.diff_x) - how.left_y) / how.diff_y;
			return finished;
		}

		animation::animation(int loops) : loops(loops), current(0) {}

		void animation::start() {
			current = current_loop = 0;
			if(!animators.empty()) animators[0]->start();
		}

		bool animation::animate() {
			if(loops > -1 && current_loop >= loops) return true;
			if(current < animators.size()) {
				if(animators[current]->animate()) {
					++current;
					if(current >= animators.size()) {
						current = 0;
						++current_loop;
					}
					animators[current]->start();
					if(loops > -1 && current_loop >= loops) return true;
				}
				return false;
			}
			else return true;
		}
	}
}

