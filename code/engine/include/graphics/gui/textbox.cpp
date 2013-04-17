#pragma once
#include "textbox.h"
#include "text\drafter.h"

namespace db {
	namespace graphics {
	using namespace io::input;
		namespace gui {
			namespace controls {
				textbox::textbox(const rect& r, text::style default_style) 
					: rect(r), editor(default_style), view_caret(false), blink_reset(false) { 
						preserve_focus = true; 
				}

				void textbox::draw_text_ui(const draw_info& in) {
					editor.guarded_redraw();
					print.draw_text(in.v, editor, *this);
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

					print.blink.update();
				}
				
				point textbox::local_mouse() {
					return in->owner.events.mouse.pos + scroll - get_absolute_xy();
				}

				void textbox::on_caret_left(bool s)			{ editor.caret_left(s);			view_caret = true; blink_reset = true;}
				void textbox::on_caret_right(bool s)		{ editor.caret_right(s);		view_caret = true; blink_reset = true;}
				void textbox::on_caret_left_word(bool s)	{ editor.caret_left_word(s);	view_caret = true; blink_reset = true;}
				void textbox::on_caret_right_word(bool s)	{ editor.caret_right_word(s);	view_caret = true; blink_reset = true;}
				void textbox::on_caret_up(bool s)			{ editor.caret_up(s);			view_caret = true; blink_reset = true;}
				void textbox::on_caret_down(bool s)			{ editor.caret_down(s);			view_caret = true; blink_reset = true;}
				void textbox::on_caret_ctrl_up()			{ 
					editor.guarded_redraw();
					int li = editor.get_draft().get_line_visibility(get_local_clipper()).first;
					if(li == -1) return;
					auto& l = editor.get_draft().lines[li];

					if(int(scroll.y) != l.get_rect().y)
					   scroll.y  = float(l.get_rect().y);
					else if(li > 0) 
						scroll.y -= float(editor.get_draft().lines[li-1].get_rect().h);  
				}

				void textbox::on_caret_ctrl_down()			{ 
					editor.guarded_redraw();
					int li = editor.get_draft().get_line_visibility(get_local_clipper()).second;
					if(li == -1) return;
					auto& l = editor.get_draft().lines[li];

					if(int(scroll.y) != l.get_rect().b() - rc.h())
					   scroll.y  = float(l.get_rect().b() - rc.h());
					else if(li < int(editor.get_draft().lines.size()-1)) 
						scroll.y += float(editor.get_draft().lines[li+1].get_rect().h);  
				}

				void textbox::on_place_caret(bool s) {  
						editor.guarded_redraw();
						//if(!s) caret.selection_offset = 0;
						editor.set_caret(editor.get_draft().map_to_caret_pos(local_mouse()), s);
						blink_reset = true;
					//	view_caret = true; 
				}

				void textbox::on_select_word() {
						editor.guarded_redraw();
						editor.select_word(editor.get_draft().map_to_caret_pos(local_mouse()));
						blink_reset = true;
						view_caret = true; 
				}

				void textbox::on_select_line() { 
						editor.guarded_redraw();
						editor.select_line(editor.get_draft().map_to_caret_pos(local_mouse()));
						blink_reset = true;
						view_caret = true; 
				}

				void textbox::on_select_all()		{ editor.select_all();							view_caret = true;						}
				void textbox::on_home(bool s)		{ editor.home(s);								view_caret = true;						}
				void textbox::on_end(bool s)		{ editor.end(s);								view_caret = true;						}
				void textbox::on_pagedown()			{ scroll.y += get_rect_absolute().h();												}
				void textbox::on_pageup()			{ scroll.y -= get_rect_absolute().h();												}
				void textbox::on_character()		{ editor.character(in->owner.events.utf16);		view_caret = true; blink_reset = true;	}	
				void textbox::on_cut()				{ editor.cut(in->owner);						view_caret = true;						}	  
				void textbox::on_bold()				{ editor.bold();								view_caret = true;						}	  
				void textbox::on_italics()			{ editor.italics();								view_caret = true;						}	  
				void textbox::on_copy()				{ editor.copy(in->owner);						view_caret = true;						}	    
				void textbox::on_undo()				{ editor.undo();								view_caret = true;						}	  	
				void textbox::on_redo()				{ editor.redo();								view_caret = true;						}	  	
				void textbox::on_paste()			{ editor.paste(in->owner);						view_caret = true; blink_reset = true;	}	  	
				void textbox::on_backspace(bool c)	{ editor.backspace(c);							view_caret = true; blink_reset = true;	}
				void textbox::on_del(bool c)		{ editor.del(c);								view_caret = true; blink_reset = true;	}
				
				void textbox::on_drag()	{ 
						editor.set_caret(editor.get_draft().map_to_caret_pos(local_mouse()), true);
						drag.drag(local_mouse(), get_local_clipper()); 
						blink_reset = true;
				}
	
				void textbox::on_focus(bool f) {
					print.active = f;
					blink_reset = true;
				}

				rect_wh textbox::get_content_size() {
					editor.guarded_redraw();
					return editor.get_draft().get_bbox();
				}

				void textbox::handle_interface(event e) {
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
						editor.guarded_redraw();
						scroll += editor.get_draft().view_caret(editor.get_caret_pos(), get_local_clipper());
						view_caret = false;
					}

					if(blink_reset) {
						print.blink.reset();
						blink_reset = false;
					}
				}

				void textbox::event_proc(rect::event e) {
					handle_interface(e);
					rect::event_proc(e);
				}
			}
		}
	}
}