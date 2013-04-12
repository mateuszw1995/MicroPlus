#pragma once
#include "gui1.h"

dynamic_border::dynamic_border() : stroke(1, material(gui::null_texture, ltblue), rect::solid_stroke::OUTSIDE) {}


cbackground::cbackground(const rect& r) : rect(r) {}

void cbackground::event_proc(event m) {
	if(m == event::wheel || m == event::mdown) rect::event_proc(m);
}

void cbackground::draw_proc(const draw_info& in) {
	rect::draw_proc(in);
	stroke.draw(*this, in);
}

ctickbox::ctickbox(const rect& r, material active, material inactive, bool& set) : rect(r), active(active), inactive(inactive), set(set) {
	mat = set ? active : inactive;
	stroke.set_width(1);
	stroke.set_material(set ? ltblue : gray);
}

void ctickbox::event_proc(event m) {
	if(m == rect::event::lclick) {
		set = !set;
		mat = set ? active : inactive;
	}
	
	if(poll_border(m) == 0) {
		stroke.set_width(1);
		stroke.set_material(set ? ltblue : gray);
	}
	else {
		stroke.set_material(white);
	}

	if(m == event::wheel || m == event::mdown) rect::event_proc(m);
}

void ctickbox::draw_proc(const draw_info& in) {
	rect::draw_proc(in);
	stroke.draw(*this, in);
}

clabel_tickbox::clabel_tickbox(const rect& r, std::wstring label, style act, style inact, bool& set) 
	: rect(r), set(set), active(formatted_text(label.c_str(), act)), inactive(formatted_text(label.c_str(), inact)) {
		stroke.set_width(1);
		stroke.set_material(set ? ltblue : gray);
		update_rc();
		snap_scroll_to_content = false;
		scroll = point(4, 4);
		clip = false;
		print.clip = false;
		children.push_back(&print);
}

void clabel_tickbox::event_proc(event m) {
	if(m == rect::event::lclick) {
		set = !set;
		update_rc();
	}
	
	if(poll_border(m) == 0) {
		stroke.set_width(1);
		stroke.set_material(set ? ltblue : gray);
	}
	else {
		stroke.set_material(white);
	}

	if(m == event::wheel || m == event::mdown) rect::event_proc(m);
}

void clabel_tickbox::update_rc() {
	print.str = set ? active : inactive;
	print.draft.draw(print.str);
	print.update_str = false;
	rc.w(print.draft.get_bbox().w + 4);
	rc.h(print.draft.get_bbox().h + 2);
}

void clabel_tickbox::draw_proc(const draw_info& in) {
	rect::draw_children(in);
	stroke.draw(*this, in);
}

ctext_modifier::ctext_modifier(const rect& r, textbox* mytext, type _type) : _type(_type), mytext(mytext), rect(r) {
}

void ctext_modifier::event_proc(event m) {
	poll_border(m);
	if(m == event::lclick) {
		if(_type == BOLDEN)
			mytext->on_bold();
		else
			mytext->on_italics();
		mytext->need_redraw();
	}
	if(m == event::wheel || m == event::mdown) rect::event_proc(m);
}

void ctext_modifier::draw_proc(const draw_info& in) {
	rect::draw_proc(in);
	auto& t = *mytext;
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