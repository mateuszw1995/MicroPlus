//#define include_DWM
#include "..\..\db\db.h"

int db::main() {
	using namespace db;
	using namespace math;
	using namespace window;

	config cfg(L"config.txt");

	glwindow gl("MicroPlus",	int(cfg["limit_resize"]) > 0);
	gl.create(math::rect_xywh(		cfg["window_x"], 
									cfg["window_y"], 
									cfg["resolution_w"],
									cfg["resolution_h"]), 
								int(cfg["doublebuffer"]) > 0, 
								int(cfg["windowborder"]) > 0 ? (int(cfg["resizable"]) > 0 ? glwindow::ALL : glwindow::ALL &~ glwindow::RESIZABLE) : 0, 
									cfg["bpp"]);
	
	gl.set_minimum_size(rect_wh(cfg["min_resw"], cfg["min_resh"]));
	gl.set_maximum_size(rect_wh(cfg["max_resw"], cfg["max_resh"]));

	event::message msg;

	bool quit = false;
	gl.set_show(gl.SHOW);
	
	float val = 0.f;
	float r = 0.f;

	misc::animator fadein (&val, 0.f, 1.f, 500, misc::animator::EXPONENTIAL);
	misc::animator mover (&r, 400.0f, 0.0f, 500, misc::animator::EXPONENTIAL);
	misc::animation anim(1), anim2(1);
	anim.animators.push_back(&fadein);
	anim2.animators.push_back(&mover);
	
	anim.start();
	anim2.start();

	fpstimer tm;
	db::graphics::init(&tm);
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

	while(!quit) {
		while(gl.poll_events(msg)) {
			if(msg == event::close) {
				quit = true;
			}
		}
		
		anim.animate();
		anim2.animate();
		rect_xywh rc = gl.get_window_rect();
		rc.x = r;
		gl.set_window_rect(rc);

		glClearColor(val, val, val, val);
		glClear(GL_COLOR_BUFFER_BIT);

		gl.swap_buffers();
	}
	
	return 0;
}