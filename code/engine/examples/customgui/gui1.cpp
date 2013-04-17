#pragma once
#include "gui1.h"

cbackground::cbackground(const rect_xywh& r, const stylesheet& styles) 
	: styles(styles), rect(r)  {}

cchecklabel::cchecklabel(const checklabel& c, const stylesheet& styles_active, const stylesheet& styles_inactive, const std::function<void (bool)>& callback) 
	: styles_active(styles_active), styles_inactive(styles_inactive), checklabel(c), callback(callback) {}

ccheckbox::ccheckbox(bool set, const rect_xywh& r, const stylesheet& styles_active, const stylesheet& styles_inactive, const std::function<void (bool)>& callback) 
	: styles_active(styles_active), styles_inactive(styles_inactive), checkbox(set, r), callback(callback) {}

cslider::cslider(const scrollarea::slider& r, const stylesheet& styles) 
	: styles(styles), slider(r) {}

cscrollarea::cscrollarea(const scrollarea& r, const stylesheet& styles) 
	: styles(styles), scrollarea(r) {}

ctextbox::ctextbox(const textbox& r, const stylesheet& styles)
	: styles(styles), textbox(r) {}

text_modifier::text_modifier(bool set, const rect_xywh& r, 
							 const stylesheet& styles_active, 
							 const stylesheet& styles_inactive, 
							 const std::function<void ()>& callback,
							 const std::function<bool ()>& active_condition)
	: checkbox(set, r), styles_active(styles_active), styles_inactive(styles_inactive), callback(callback), active_condition(active_condition) {}

void cbackground::event_proc(event m) {
	rect::event_proc(m);
	styles.update_appearance(m);
}

void ccheckbox::event_proc(event m) {
	checkbox::event_proc(m);
	styles_active  .update_appearance(m);
	styles_inactive.update_appearance(m);
}

void cchecklabel::event_proc(event m) {
	checklabel::event_proc(m);
	styles_active  .update_appearance(m);
	styles_inactive.update_appearance(m);
}

void text_modifier::event_proc(event m) {
	checkbox::event_proc(m);
	styles_active  .update_appearance(m);
	styles_inactive.update_appearance(m);
}

void cslider::event_proc(event m) {
	slider::event_proc(m);
	styles.update_appearance(m);
}

void cscrollarea::event_proc(event m) {
	scrollarea::event_proc(m);
	styles.update_appearance(m);
}

void ctextbox::event_proc(event m) {
	textbox::event_proc(m);
	styles.update_appearance(m);
}

void cbackground::draw_proc(const draw_info& in) {
	draw_rect(in, styles);
	draw_children(in);
}

void ccheckbox::draw_proc(const draw_info& in) {
	draw_rect(in, set ? styles_active : styles_inactive);
	draw_children(in);
}

void cchecklabel::draw_proc(const draw_info& in) {
	draw_rect(in, set ? styles_active : styles_inactive);
	draw_label(in);
	draw_children(in);
}

void text_modifier::draw_proc(const draw_info& in) {
	set = active_condition();
	draw_rect(in, set ? styles_active : styles_inactive);
	draw_children(in);
}

void cslider::draw_proc(const draw_info& in) {
	draw_rect(in, styles);
}

void cscrollarea::draw_proc(const draw_info& in) {
	draw_rect(in, styles);
	draw_slider(in);
}

void ctextbox::draw_proc(const draw_info& in) {					
	draw_rect(in, styles);
	draw_text_ui(in);
}

void ccheckbox::on_change(bool flag) {
	//checkbox::on_change(flag);
	if(callback) callback(flag);
}

void cchecklabel::on_change(bool flag) {
	checklabel::on_change(flag);
	if(callback) callback(flag);
}

void text_modifier::on_change(bool flag) {
	//checkbox::on_change(flag);
	if(callback) callback();
}

