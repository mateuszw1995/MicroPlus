#include "db\db.h"
//#include "microplus\microplus.h"

struct crect : public db::graphics::gui::rect {
	void event_proc(event m) { /* event listener */
		if(m == event::lclick) {
			// twoj kod
		}
	}
	crect(const rect& r) : rect(r) {}
};

int resizer(db::window::glwindow& gl) {
	gl.current();
	glViewport(0,0,gl.get_window_rect().w,gl.get_window_rect().h);
	
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, gl.get_window_rect().w, gl.get_window_rect().h, 0, 0, 1 );
	glMatrixMode(GL_MODELVIEW);
	return 0;
}

int db::main() {
	using namespace db;
	using namespace graphics;
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
	gl.resize_func(resizer);
	resizer(gl);

	event::message msg;
	gui::null_texture = new graphics::io::input::texture;
	bool quit = false;
	gl.set_show(gl.SHOW);
	float val = 0.f;
	float r = 0.f;
	misc::animator fadein (&val, 0.f, 1.f, 500, misc::animator::EXPONENTIAL);
	misc::animator fadeout(&val, 1.f, 0.f, 2000, misc::animator::LOGARITHMIC);
	misc::animator mover (&r, 400.0f, 0.0f, 500, misc::animator::EXPONENTIAL);
	//misc::animator faderout(&r, 0.9f, 0.1f, 400, misc::animator::LOGARITHMIC);
	misc::animation anim(1), anim2(1);
	anim.animators.push_back(&fadein);
	//anim.animators.push_back(&fadeout);
	anim2.animators.push_back(&mover);
	//anim2.animators.push_back(&faderout);
	anim.start();
	anim2.start();
	fpstimer tm;
	db::graphics::init(&tm);
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	gl.transparency(true);
	
	db::graphics::gui::system sys;
	crect myrect(gui::rect(rect_xywh(10, 10, 100, 100), gui::material(gui::null_texture, pixel_32(0, 255, 0, 255))));
	sys.windows.push_back(&myrect);
	
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
	while(!quit) {
		while(gl.poll_events(msg)) {
			if(msg == event::close) {
				quit = true;
			}
			sys.update_rectangles();
			sys.poll_events(gl);
		}
		
		anim.animate();
		anim2.animate();
		rect_xywh rc = gl.get_window_rect();
		rc.x = r;
		gl.set_window_rect(rc);

		glClearColor(val, val, val, val);
		glClear(GL_COLOR_BUFFER_BIT);
		
			sys.update_rectangles();
			sys.call_updaters(gl);
			sys.update_rectangles();
			sys.update_array(gl);
		sys.bind();			
		glDrawArrays(GL_QUADS, 0, sys.rect_array.size() * 4);

		gl.swap_buffers();
	}
	
	return 0;
}