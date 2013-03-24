#pragma once
#include "../../include/db.h"

using namespace db;
using namespace graphics;
using namespace window;
using namespace gui;
using namespace io;
using namespace input;
using namespace event;
using namespace controls;

const pixel_32 ltblue(0, 122, 204, 255);
const pixel_32 darkgray(30, 30, 30, 255);
const pixel_32 gray(50, 50, 50, 255);
const pixel_32 white(255, 255, 255, 255);

struct dynamic_border {
	rect::solid_stroke stroke;

	dynamic_border();
	int poll_border(rect::event m);
};

struct cbackground : public rect, public dynamic_border {
	cbackground(const rect& r);
	
	void event_proc(event m);
	void draw_proc(const draw_info& in);
};

struct ccolortickbox : public rect, public dynamic_border {
	static font* labels_active;
	static font* labels_inactive;
	bool& set;
	pixel_32 active, inactive;
	fstr label;
	text_printer label_p;
	rect_wh size;

	ccolortickbox(wchar_t* label_str, const rect& r, bool& set, pixel_32 active = ltblue, pixel_32 inactive = darkgray) ;
	void update_mat();
	void event_proc(event m);
	void update_proc(gui::system& in);
	void draw_proc(const draw_info& in);
};

struct ctext_modifier : public rect, public dynamic_border {
	enum type {
		ITALICSEN, BOLDEN
	} _type;
	textbox* mytext;
	
	ctext_modifier(const rect& r, textbox* mytext, type _type);
	void event_proc(event m);
	void draw_proc(const draw_info& in);
};

struct ctextbox : public textbox, public dynamic_border {
	ctextbox(const textbox& r);
	void on_focus(bool f);
	void draw_proc(const draw_info& in);
};

struct cslider : public scrollarea::slider, public dynamic_border {
	cslider(const scrollarea::slider& r);
	void event_proc(event m);
};