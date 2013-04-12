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

struct theme : public appearance_rect {
	rect::solid_stroke stroke;

	theme(const appearance_rect&);
	void on_released();
	void on_hovered();
	void on_pushed();				

	void draw_proc(const draw_info&);
};

struct cbackground : public theme {
	cbackground(const rect& r);
	
	void event_proc(event m);
	void draw_proc(const draw_info& in);
};

template <class functor>
struct ctickbox : public theme {
	functor callback;

	bool& set;
	material active, inactive;
	ctickbox(const functor& callback, material active, material inactive, bool& set) 
		: callback(callback), set(set), active(active), inactive(inactive) {
	}

	void event_proc(event m);
	void draw_proc(const draw_info& in);
};

template <class functor>
struct clabel_tickbox : public theme {
	functor callback;

	bool& set;
	clabel_tickbox(const rect& r, std::wstring label, style active, style inactive, bool& set);
	void event_proc(event m);
	void draw_proc(const draw_info& in);
private:
	void update_rc();
	text::text_rect print;
	fstr active, inactive;
};

struct ctextbox : public textbox {
	ctextbox(const textbox& r);
	void on_focus(bool f);
	void draw_proc(const draw_info& in);
};

struct cslider : public scrollarea::slider {
	cslider(const scrollarea::slider& r);
	void event_proc(event m);
};