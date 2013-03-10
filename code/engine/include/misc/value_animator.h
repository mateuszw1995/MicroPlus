#pragma once
#include "../window/window.h"

namespace db {
	namespace misc {
		class animator {
			window::timer tm;
			float *val, init_val, diff, miliseconds;
		public:
			struct method {
				float (*increasing_func)(float), left_x, diff_x, left_y, diff_y;
				method(float left_x, float right_x, float (*increasing_func)(float)); 
				
				static float linear(float);
				static float quadratic(float);
				static float sinusoidal(float);
				static float hyperbolic(float);
				static float logarithmic(float);
				static float exponential(float);
			} how;
			static method LINEAR, QUADRATIC, SINUSOIDAL, HYPERBOLIC, LOGARITHMIC, EXPONENTIAL;
			animator(float* val, float init_val, float desired_val, float miliseconds, method how);
			
			void start();
			bool animate();
		};

		struct animation {
			std::vector<animator*> animators;

			int loops, current_loop;
			unsigned current;
			animation(int loops = -1);

			void start();
			bool animate();
		};
	}
}

