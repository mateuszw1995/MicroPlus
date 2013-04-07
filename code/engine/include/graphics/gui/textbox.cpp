#pragma once
#include "textbox.h"
#include "text\drafter.h"

namespace db {
	namespace graphics {
	using namespace io::input;
		namespace gui {
			namespace controls {
				textbox::textbox(const rect& r, style default_style) 
					: printer(r), ui(default_style), view_caret(false), blink_reset(false) {}

				void textbox::draw_proc(const draw_info& in) {
					rect::draw_proc(in);
					guarded_redraw();
					draw_text(in.v, *this);
				}
				
				//void textbox::update_rectangles() {
				//	rect::update_rectangles();
				//	guarded_redraw();
				//}
		
				void textbox::update_proc(system& owner) {
					drag.move(scroll);

					if(drag.vel[0] != 0.f || drag.vel[1] != 0.f) {
						rect::info _inf(owner, 0);
						in = &_inf;
						on_drag();
					}
					printer::update_proc(owner);
				}
				
				point textbox::local_mouse() {
					return in->owner.events.mouse.pos + scroll - get_absolute_xy();
				}

				void textbox::on_caret_left(bool s)			{ caret_left(s);			view_caret = true; blink_reset = true;}
				void textbox::on_caret_right(bool s)		{ caret_right(s);			view_caret = true; blink_reset = true;}
				void textbox::on_caret_left_word(bool s)	{ caret_left_word(s);		view_caret = true; blink_reset = true;}
				void textbox::on_caret_right_word(bool s)	{ caret_right_word(s);		view_caret = true; blink_reset = true;}
				void textbox::on_caret_up(bool s)			{ caret_up(s);				view_caret = true; blink_reset = true;}
				void textbox::on_caret_down(bool s)			{ caret_down(s);			view_caret = true; blink_reset = true;}
				void textbox::on_caret_ctrl_up()			{ 
					guarded_redraw();
					int li = draft.get_line_visibility(get_local_clipper()).first;
					if(li == -1) return;
					auto& l = draft.lines[li];

					if(int(scroll.y) != l.get_rect().y)
					   scroll.y  = float(l.get_rect().y);
					else if(li > 0) 
						scroll.y -= float(draft.lines[li-1].get_rect().h);  
				
				}

				void textbox::on_caret_ctrl_down()			{ 
					guarded_redraw();
					int li = draft.get_line_visibility(get_local_clipper()).second;
					if(li == -1) return;
					auto& l = draft.lines[li];

					if(int(scroll.y) != l.get_rect().b() - rc.h())
					   scroll.y  = float(l.get_rect().b() - rc.h());
					else if(li < int(draft.lines.size()-1)) 
						scroll.y += float(draft.lines[li+1].get_rect().h);  
				
				}

				void textbox::on_place_caret(bool s) {  
						guarded_redraw();
						//if(!s) caret.selection_offset = 0;
						set_caret(draft.map_to_caret_pos(local_mouse()), s);
						blink_reset = true;
					//	view_caret = true; 
				}

				void textbox::on_select_word() {
						guarded_redraw();
						select_word(draft.map_to_caret_pos(local_mouse()));
						blink_reset = true;
						view_caret = true; 
				}

				void textbox::on_select_line() { 
						guarded_redraw();
						select_line(draft.map_to_caret_pos(local_mouse()));
						blink_reset = true;
						view_caret = true; 
				}

				void textbox::on_select_all()		{ select_all();							view_caret = true;						}
				void textbox::on_home(bool s)		{ home(s);								view_caret = true;						}
				void textbox::on_end(bool s)		{ end(s);								view_caret = true;						}
				void textbox::on_pagedown()			{ scroll.y += get_rect_absolute().h();												}
				void textbox::on_pageup()			{ scroll.y -= get_rect_absolute().h();												}
				void textbox::on_character()		{ character(in->owner.events.utf16);	view_caret = true; blink_reset = true;	}	
				void textbox::on_cut()				{ cut(in->owner);						view_caret = true;						}	  
				void textbox::on_bold()				{ bold();								view_caret = true;						}	  
				void textbox::on_italics()			{ italics();							view_caret = true;						}	  
				void textbox::on_copy()				{ copy(in->owner);						view_caret = true;						}	    
				void textbox::on_undo()				{ undo();								view_caret = true;						}	  	
				void textbox::on_redo()				{ redo();								view_caret = true;						}	  	
				void textbox::on_paste()			{ paste(in->owner);						view_caret = true; blink_reset = true;	}	  	
				void textbox::on_backspace(bool c)	{ backspace(c);							view_caret = true; blink_reset = true;	}
				void textbox::on_del(bool c)		{ del(c);								view_caret = true; blink_reset = true;	}
				
				void textbox::on_drag()	{ 
						set_caret(draft.map_to_caret_pos(local_mouse()), true);
						drag.drag(local_mouse(), get_local_clipper()); 
						blink_reset = true;
				}
	
				void textbox::on_focus(bool f) {
					active = f;
					blink_reset = true;
				}

				rect_wh textbox::get_content_size() {
					guarded_redraw();
					return draft.get_bbox();
				}

				void textbox::event_proc(rect::event e) {
					using namespace db::event::keys;
					auto& w = in->owner.events;
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

					case rect::event::lup:		drag.stop(); break;
					case rect::event::hover:	drag.stop(); break;
					case rect::event::loutup:	drag.stop(); break;

					default: break;
					}

					if(view_caret) {
						guarded_redraw();
						scroll += draft.view_caret(get_caret_pos(), get_local_clipper());
						view_caret = false;
					}

					if(blink_reset) {
						blink.reset();
						blink_reset = false;
					}
					return rect::event_proc(e);
				}
			}
		}
	}
}