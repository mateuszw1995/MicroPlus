#pragma once
#include "../../include/db.h"
#include <functional>

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
const pixel_32 gray1(50, 50, 50, 255);
const pixel_32 gray2(62, 62, 62, 255);
const pixel_32 gray3(104, 104, 104, 255);
const pixel_32 gray4(180, 180, 180, 255);
const pixel_32 white(255, 255, 255, 255);
const pixel_32 darkblue(6, 5, 20, 255);

struct cbackground : public rect {
	stylesheet styles;
	cbackground(const rect_xywh& r, const stylesheet& styles);
	
	virtual void draw_proc(const draw_info& in) override;
	virtual void event_proc(event) override;
};

struct ccheckbox : public controls::checkbox {
	stylesheet styles_inactive, styles_active;
	std::function<void (bool)> callback;
	
	ccheckbox(bool set, const rect_xywh& r, 
		const stylesheet& styles_active, 
		const stylesheet& styles_inactive, 
		const std::function<void (bool)>& = std::function<void (bool)>());
	
	virtual void on_change(bool) override;
	virtual void draw_proc(const draw_info& in) override;
	virtual void event_proc(event) override;
};

struct text_modifier : public controls::checkbox {
	stylesheet styles_inactive, styles_active;
	std::function<void ()> callback;
	std::function<bool ()> active_condition;
	
	text_modifier(bool set, const rect_xywh& r, 
		const stylesheet& styles_active, 
		const stylesheet& styles_inactive, 
		const std::function<void ()>& callback,
		const std::function<bool ()>& active_condition);

	virtual void draw_proc(const draw_info& in) override;
	virtual void event_proc(event) override;
	virtual void on_change(bool) override;

	protected:
		using checkbox::set;
};


struct cchecklabel : public controls::checklabel {
	stylesheet styles_inactive, styles_active;
	std::function<void (bool)> callback;

	cchecklabel(const checklabel&, 
		const stylesheet& styles_active, 
		const stylesheet& styles_inactive, const std::function<void (bool)>& callback = std::function<void (bool)>());

	virtual void on_change(bool) override;
	virtual void draw_proc(const draw_info& in) override;
	virtual void event_proc(event) override;
};

struct ctextbox : public textbox {
	stylesheet styles;
	ctextbox(const textbox& r, const stylesheet&);

	virtual void draw_proc(const draw_info& in) override;
	virtual void event_proc(event) override;
};

struct cslider : public scrollarea::slider {
	stylesheet styles;

	cslider(const scrollarea::slider& r, const stylesheet&);

	virtual void draw_proc(const draw_info& in) override;
	virtual void event_proc(event m) override;
};

struct cscrollarea : public scrollarea {
	stylesheet styles;

	cscrollarea(const scrollarea& r, const stylesheet&);

	virtual void draw_proc(const draw_info& in) override;
	virtual void event_proc(event m) override;
};
