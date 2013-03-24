#pragma once
#include "gui1.h"

dynamic_border::dynamic_border() : stroke(1, material(null_texture, ltblue), rect::solid_stroke::OUTSIDE) {}

int dynamic_border::poll_border(rect::event m) {
	if(m == rect::event::hout || m == rect::event::lup || m == rect::event::loutup) {
		stroke.set_width(0);
		return 0;
	}
	if(m == rect::event::hover) {
		stroke.set_width(1);
		return 1;
	}
	if(m == rect::event::lpressed || m == rect::event::ldown || m == rect::event::ldoubleclick || m == rect::event::ltripleclick ) {
		stroke.set_width(2);
		return 2;
	}
	return 0;
}

cbackground::cbackground(const rect& r) : rect(r) {}

void cbackground::event_proc(event m) {
	if(m == event::wheel || m == event::mdown) rect::event_proc(m);
}

void cbackground::draw_proc(const draw_info& in) {
	rect::draw_proc(in);
	stroke.draw(*this, in);
}

void ccolortickbox::update_mat() {
	mat.color = inactive;
	for(size_t  i = 0; i < label.size(); ++i)
		label[i].set(set ? labels_active : labels_inactive, set ? active : white);
}

ccolortickbox::ccolortickbox(wchar_t* label_str, const rect& r, bool& set, pixel_32 active, pixel_32 inactive) 
	: rect(r), set(set), active(active), inactive(inactive), label_p(&label) {
		if(label_str) label = formatted_text(label_str, labels_inactive, pixel_32());
		update_mat();
		stroke.set_width(1);
		stroke.set_material(material(null_texture, inactive));
		clip = false;
		if(label_str) print = &label_p;
}

void ccolortickbox::event_proc(event m) {
	auto& ms = in->owner.events->mouse;
	if(m == rect::event::lclick) {
		set = !set;
		update_mat();
	}
	if(poll_border(m) == 0) {
		stroke.set_width(1);
		stroke.set_material(material(null_texture, inactive));
	}
	else stroke.set_material(material(null_texture, active));

	if(m == event::wheel || m == event::mdown) rect::event_proc(m);
}

void ccolortickbox::update_proc(gui::system& in) {
	if(!print) return;
	rc.r = max(200, label_p.get_bbox().w());
	rc.h(label_p.get_bbox().h());
}

void ccolortickbox::draw_proc(const draw_info& in) {
	//rect::draw_proc(in);
	draw_rect(in);
	draw_children(in);
	draw_text(in);
	stroke.draw(*this, in);
}
font* ccolortickbox::labels_active = 0;
font* ccolortickbox::labels_inactive = 0;

ctext_modifier::ctext_modifier(const rect& r, textbox* mytext, type _type) : _type(_type), mytext(mytext), rect(r) {
}

void ctext_modifier::event_proc(event m) {
	poll_border(m);
	if(m == event::lclick) {
		if(_type == BOLDEN)
			mytext->on_bold();
		else
			mytext->on_italics();
	}
	if(m == event::wheel || m == event::mdown) rect::event_proc(m);
}

void ctext_modifier::draw_proc(const draw_info& in) {
	rect::draw_proc(in);
	auto& t = *mytext->editor;
	stroke.draw(*this, in);
	if(_type == BOLDEN)
		mat.color = t.get_bold_status() ? ltblue : white;
	else 
		mat.color = t.get_italics_status() ? ltblue : white;
}

ctextbox::ctextbox(const textbox& r) : textbox(r) {}

void ctextbox::on_focus(bool f) {
	//if(f)
	//	scroll_to_view();
	return textbox::on_focus(f);
}

void ctextbox::draw_proc(const draw_info& in) {
	textbox::draw_proc(in);
	stroke.draw(*this, in);
}

cslider::cslider(const scrollarea::slider& r) : slider(r.min_side, r.mat) {}

void cslider::event_proc(event m) {
	if(m == event::hover && mat.color.r == 104) {
		mat.color = pixel_32(180, 180, 180, 255);
	}
	if(m == event::hout && mat.color.r == 180) {			
		mat.color = pixel_32(104, 104, 104, 255);
	}
	if(m == event::ldown || m == event::ldoubleclick || m == event::ltripleclick) {			
		mat.color = white;
	}
	if(m == event::lup) {
		mat.color = pixel_32(180, 180, 180, 255);
	}
	if(m == event::loutup) {		
		mat.color = pixel_32(104, 104, 104, 255);
	}

	slider::event_proc(m);
}