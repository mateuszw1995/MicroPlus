#include "..\..\db\db.h"

using namespace db::window;
int db::main() {
	glwindow gl("GL Window", false);
	gl.create(math::rect_xywh(40, 40, 400, 400), 1, glwindow::ALL, 24); 
	gl.set_show(gl.SHOW);
	
	event::message msg;
	bool quit = false;
	while(!quit) {
		while(gl.poll_events(msg)) {
			if(msg == event::close) {
				quit = true;
			}
		}
		
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glBegin(GL_TRIANGLES);
		glVertex2f(1.0, 0.0);
		glVertex2f(1.0, 1.0);
		glVertex2f(0.0, 1.0);
		glEnd();

		gl.swap_buffers();
	}
	
	return 0;
}