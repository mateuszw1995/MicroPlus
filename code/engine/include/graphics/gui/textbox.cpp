#pragma once
#include "text.h"

namespace db {
	namespace graphics {
	using namespace io::input;
		namespace gui {
			namespace controls {
				textbox::textbox(const math::rect_xywh& rc, const material& m, text* t) :
					rect(rc, m), view_caret(false), blink_reset(false) {
						print = t;
				}
		
				void textbox::update_proc(system& owner) {
					scroller.move(pen);

					if(scroller.vel[0] != 0.f || scroller.vel[1] != 0.f) {
						rect::info _inf(owner, 0);
						in = &_inf;
						on_drag();
					}
					rect::update_proc(owner);
				}

				void textbox::on_caret_left(bool s)			{ print->caret_left(s);			view_caret = true; blink_reset = true;}
				void textbox::on_caret_right(bool s)		{ print->caret_right(s);		view_caret = true; blink_reset = true;}
				void textbox::on_caret_left_word(bool s)	{ print->caret_left_word(s);	view_caret = true; blink_reset = true;}
				void textbox::on_caret_right_word(bool s)	{ print->caret_right_word(s);	view_caret = true; blink_reset = true;}
				void textbox::on_caret_up(bool s)			{ print->caret_up(s);			view_caret = true; blink_reset = true;}
				void textbox::on_caret_down(bool s)			{ print->caret_down(s);			view_caret = true; blink_reset = true;}
				void textbox::on_caret_ctrl_up()			{ 
					if(pen.y != float(print->lines[print->get_first_line_visible()].get_rect().y - print->lines[0].get_rect().y)) {
					   pen.y  = float(print->lines[print->get_first_line_visible()].get_rect().y - print->lines[0].get_rect().y);
					}
					else if(print->get_first_line_visible() > 0) 
						pen.y -= float(print->lines[print->get_first_line_visible()-1].get_rect().h);  
				
				}

				void textbox::on_caret_ctrl_down()			{ 
					if(pen.y != float(print->lines[print->get_last_line_visible()].get_rect().b() - print->lines[0].get_rect().y - get_rect_absolute().h())) {
					   pen.y  = float(print->lines[print->get_last_line_visible()].get_rect().b() - print->lines[0].get_rect().y - get_rect_absolute().h());
					}
					else if(print->get_last_line_visible() < print->lines.size()-1) 
						pen.y += float(print->lines[print->get_last_line_visible()+1].get_rect().h);  
				
				}

				void textbox::on_place_caret(bool s) {  
						if(!s) print->select.offset = 0;
						print->set_caret(print->map_mouse(in->owner.events->mouse.pos, true), s);
						blink_reset = true;
					//	view_caret = true; 
				}

				void textbox::on_select_word() {  
						print->select_word(print->map_mouse(in->owner.events->mouse.pos, false));
						blink_reset = true;
						view_caret = true; 
				}

				void textbox::on_select_line() {  
						print->select_line(print->map_mouse(in->owner.events->mouse.pos, false));
						blink_reset = true;
						view_caret = true; 
				}

				void textbox::on_select_all()		{ print->select_all();						view_caret = true;						}
				void textbox::on_home(bool s)		{ print->home(s);							view_caret = true;						}
				void textbox::on_end(bool s)		{ print->end(s);							view_caret = true;						}
				void textbox::on_pagedown()			{ pen.y += get_rect_absolute().h();													}
				void textbox::on_pageup()			{ pen.y -= get_rect_absolute().h();													}
				void textbox::on_character()		{ print->character(in->owner.events->utf16);	view_caret = true; blink_reset = true;	}	
				void textbox::on_cut()				{ print->cut(in->owner);				view_caret = true;						}	  
				void textbox::on_bold()				{ print->bold();							view_caret = true;						}	  
				void textbox::on_italics()			{ print->italics();							view_caret = true;						}	  
				void textbox::on_copy()				{ print->copy(in->owner);				view_caret = true;						}	    
				void textbox::on_undo()				{ print->undo();							view_caret = true;						}	  	
				void textbox::on_redo()				{ print->redo();							view_caret = true;						}	  	
				void textbox::on_paste()			{ print->paste(in->owner);				view_caret = true; blink_reset = true;	}	  	
				void textbox::on_backspace(bool c)	{ print->backspace(c);						view_caret = true; blink_reset = true;	}
				void textbox::on_del(bool c)		{ print->del(c);							view_caret = true; blink_reset = true;	}
				
				void textbox::on_drag()	{ 
						print->set_caret(print->map_mouse(in->owner.events->mouse.pos, true), true);
						scroller.drag(in->owner.events->mouse.pos, get_clipped_rect()); 
						blink_reset = true;
				}
	
				void textbox::on_focus(bool f) {
					if(print) print->active = f;
					blink_reset = true;
				}

				void textbox::event_proc(rect::event e) {
					using namespace db::event::keys;
					auto& w = *in->owner.events;
					auto* k = w.keys;
					bool s = k[SHIFT], c = k[CTRL];

					switch(e) {
					case rect::event::keydown:
						switch(w.key) {
						case LEFT:	
							if(c) on_caret_left_word(s); 
							else  on_caret_left(s); 
							break;
						case RIGHT: 
							if(c) on_caret_right_word(s); 
							else  on_caret_right(s); 
							break;
						case UP: 
							if(c) on_caret_ctrl_up();
							else  on_caret_up(s);
							break;
						case DOWN: 
							if(c) on_caret_ctrl_down();
							else  on_caret_down(s);
							break;
						case BACKSPACE: on_backspace(c); break;
						case DEL: on_del(c); break;
						case HOME: on_home(s); break;
						case END: on_end(s); break;
						case PAGEUP: on_pageup(); break;
						case PAGEDOWN: on_pagedown(); break;
						default: break;
						}
						break;

					case rect::event::character:
						if(c && !k[RALT])
							switch(w.key) {
							case A: on_select_all(); break;
							case B: on_bold();		 break;
							case I: on_italics();	 break;
							case C: on_copy();		 break;
							case Z: k[LSHIFT] ? on_redo() : on_undo(); break;
							case Y: on_redo();		 break;
							case X: on_cut();		 break;
							case V: on_paste();		 break;
							default: break;
						}
						else if(w.key != BACKSPACE) on_character();
						
						break;

					case rect::event::ldown: on_place_caret(false); break;
					case rect::event::rdown: break;
					case rect::event::lpressed: on_place_caret(true); break;
					case rect::event::loutdrag: on_drag(); break;
					case rect::event::ldoubleclick: on_select_word(); break;
					case rect::event::ltripleclick: on_select_line(); break;

					case rect::event::lup:		scroller.stop(); break;
					case rect::event::hover:	scroller.stop(); break;
					case rect::event::loutup:	scroller.stop(); break;

					default: break;
					}

					if(view_caret) {
						print->view_caret(*this);
						view_caret = false;
					}

					if(blink_reset) {
						print->blink.reset();
						blink_reset = false;
					}
					return rect::event_proc(e);
				}
			}
		}
	}
}