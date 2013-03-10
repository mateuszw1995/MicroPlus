#pragma once
#include "../../Window/GLWindow.h"

namespace DB {
	namespace Window {
		namespace details {
			int min_w = 0, min_h = 0, max_w = 0, max_h = 0, width = 0, height = 0;
			bool fullscreen = false;
			int clck = 0, poll_inter = 0;
			EventObject* evptr = 0;
		}

		namespace Event {
			namespace Mouse {
				bool state[2];
				int x = 0, y = 0, xrel = 0, yrel = 0;
				_drorigin drag_origin = {0,0};
			}
			bool active = false;
			namespace Key {
				int key = 0;
				bool state[256];
				char utf8 = 0;
			}
		}
		using namespace details;

		EventObject*& EventReceiver() {
			return evptr;
		}

		int getWidth() {
			return width;
		}
		int getHeight() {
			return height;
		}

		bool isFullscreen() {
			return fullscreen;
		}

		FPSTimer::FPSTimer() : secs(0.0), sumframes(0.0), maxfps(1.0/1000.0){}


		void FPSTimer::SetMaxFPS(double fps) {
			fps = fps < 1.0 ? 1.0 : fps; 
			maxfps = 1.0/fps;
		}
		double FPSTimer::GetMaxFPS() {
			return 1.0/maxfps;
		}
		double FPSTimer::GetFrameRate() {
			return 1.0/FrameSpeed();
		}
		double FPSTimer::GetLoopRate() {
			return 1.0/LoopSpeed();
		}
		void FPSTimer::Loop() {
			secs = Seconds();
			sumframes += secs;
		}
		bool FPSTimer::Render() {
			return sumframes > maxfps;
		}
		void FPSTimer::Reset() {
			sumframes = 0.0;
		}
		double FPSTimer::FrameSpeed() {
			return sumframes;
		}
		double FPSTimer::LoopSpeed() {
			return secs;
		}
	}
}
#ifdef WIN_32
#include "../../Window/Win32Api.hpp"
#endif