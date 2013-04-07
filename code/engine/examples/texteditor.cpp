#include "db.h"
#include "customgui/gui1.h"
#include <ctime>

/* wordwrapping/kerning musza setowac need_redraw
	jakis dziwny bug przy wrappingu kilku linii, szeroki newline na koncu sie robi
	poprawic word wrapping albo sprawdzic czy dobrze ustawiona wartosc
*/

int resizer(glwindow& gl) {
	gl.current();
	glViewport(0,0,gl.get_window_rect().w,gl.get_window_rect().h);
	
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, gl.get_window_rect().w, gl.get_window_rect().h, 0, 0, 1 );
	glMatrixMode(GL_MODELVIEW);
	return 0;
}

int main() {
	srand((unsigned int)(time(NULL)));

	db::error errlog(0, 0);

	errlog.open("main_log.txt");
	graphics::errlog.open("graphics.txt");
	window::errlog.open("window.txt");
	
	//errlog.log_successful(true);
	//graphics::errlog.log_successful(true);
	//window::errlog.log_successful(true);
	
	glwindow::init();
	glwindow gl("Conspiracy", 0);
	gl.create(math::rect_xywh(20, 20, 800, 800), true);
	gl.resize_func(resizer);
	gl.set_show(gl.SHOW); 
	gl.current();
	resizer(gl);
	window::errlog.enable(true);
	
	fpstimer fps;
	graphics::init(&fps);
	fps.set_max_fps(9999999);
#define IMAGES 5
	image images[IMAGES];
	texture textures[IMAGES];

	images[0].create(4, 4, 1);
	images[0].fill(255);
	errs(images[1].from_file("resources/texteditor/settings.png", 2),		 "loading resources/texteditor/settings.png");
	errs(images[2].from_file("resources/texteditor/bold.png", 2),			 "loading resources/texteditor/bold.png");
	errs(images[3].from_file("resources/texteditor/italics.png", 2),		 "loading resources/texteditor/italics.png");
	errs(images[4].from_file("resources/texteditor/middlescroll.png", 2),  "loading resources/texteditor/middlescroll.png");
	gui::null_texture = textures + 0;

#define FONTS 4
	font_file fontf[FONTS];
	font      fonts[FONTS];
	font_in fin; 
	errs(fin.init(), "freetype initialization");

	wchar_t* str = L" qvxQVXaąbcćdeęfghijklłmnńoóprsśtuwyzźżAĄBCĆDEĘFGHIJKLŁMNŃOÓPRSŚTUWYZŹŻ0123456789.!@#$%^&*()_+-=[];'\\,./{}:\"|<>?\n";
	
	errs(fontf[0].open(fin, "resources/texteditor/arial.ttf",   12, str), "resources/texteditor/arial.ttf");
	errs(fontf[1].open(fin, "resources/texteditor/arialbd.ttf", 12, str), "resources/texteditor/arialbd.ttf");
	errs(fontf[2].open(fin, "resources/texteditor/ariali.ttf",  12, str), "resources/texteditor/ariali.ttf");
	errs(fontf[3].open(fin, "resources/texteditor/arialbi.ttf", 12, str), "resources/texteditor/arialbi.ttf");
	errs(fin.deinit(), "freetype deinitialization");
	
	fonts->set_styles(fonts + 1, fonts + 2, fonts + 3);
	glEnable(GL_TEXTURE_2D);
	
	io::input::atlas atl;
	atl.quick_add(images, textures, IMAGES, fontf, fonts, FONTS);
	errs(atl.pack(), "atlas packing");
	atl.create_image(4, false);
	atl.build(false, false);
	atl.img.destroy(); // free memory bo i tak juz jest na gpu

	gui::null_texture->translate_uv(pointf(2, 2));
	gui::null_texture->scale_uv(0.000000001f, 0.000000001f);

	bool settings_active = false;
	bool word_wrapping = true;

	gui::system sys(gl.events);
	cbackground background(rect(rect_xywh(0, 0, gl.get_window_rect().w-20, gl.get_window_rect().h-20), material(pixel_32(6, 5, 20, 255))));
	
	ctextbox  mytextbox(textbox(rect(rect_xywh(), material(darkgray)), style(fonts + 0, white)));
	
	/* ui settings */
	// mytextbox.whitelist = L"0123456789.";
	// mytextbox.max_characters = 15;

	/* structural settings */
	mytextbox.draft.wrap_width = 0;
	mytextbox.draft.kerning = false;
	
	/* visual settings */
	mytextbox.blink.blink = true;
	mytextbox.blink.interval_ms = 500;
	mytextbox.selection_bg_mat.color = pixel_32(128, 255, 255, 120);
	mytextbox.selection_inactive_bg_mat.color = pixel_32(128, 255, 255, 40);
	mytextbox.highlight_mat = material(pixel_32(15, 15, 15, 255)); 
	mytextbox.caret_mat.color = pixel_32(255, 255, 255, 255);
	mytextbox.highlight_current_line = true;
	mytextbox.highlight_during_selection = true;
	mytextbox.align_caret_height = true;
	mytextbox.caret_width = 1;;
	mytextbox.drag.vel_mult = 100.0;
	
	ctext_modifier bold_button(rect(rect_xywh (230, 75, 20, 20), textures + 2), &mytextbox, ctext_modifier::BOLDEN);
	ctext_modifier italics_button(rect(rect_xywh (260, 75, 20, 20), textures + 3), &mytextbox, ctext_modifier::ITALICSEN);
	
	gui::style   active(fonts + 1, ltblue);
	gui::style inactive(fonts + 0, gray);
	ctickbox  settings(			rect(rect_xywh(20, 47, 200, 15)), material(textures + 1, ltblue), material(textures + 1, gray), settings_active);
	clabel_tickbox  highlight(	rect(rect_xywh(20, 17, 200, 15)), L"Highlight current line", active, inactive, mytextbox.highlight_current_line);
	clabel_tickbox  kerning(	rect(rect_xywh(20, 47, 200, 15)), L"Kerning", active, inactive, mytextbox.draft.kerning);
	clabel_tickbox  word_wrap(	rect(rect_xywh(20, 77, 200, 15)), L"Word wrapping", active, inactive, word_wrapping);
	
	cslider   sl(scrollarea::slider(20, pixel_32(104, 104, 104, 255)));
	cslider  slh(scrollarea::slider(20, pixel_32(104, 104, 104, 255)));
	scrollarea  myscrtx(rect_xywh(0, 0, 10, 0), pixel_32(62, 62, 62, 255), &mytextbox, &sl, scrollarea::orientation::VERTICAL);
	scrollarea myscrhtx(rect_xywh(0, 0, 0, 10), pixel_32(62, 62, 62, 255), &mytextbox, &slh, scrollarea::orientation::HORIZONTAL);
	//background.scroll = point(10, 10);
	//background.snap_scroll_to_content = false;
	background.children.push_back(&bold_button);
	background.children.push_back(&italics_button);
	background.children.push_back(&highlight);
	background.children.push_back(&kerning);
	background.children.push_back(&word_wrap);
	background.children.push_back(&settings);
	background.children.push_back(&myscrtx);
	background.children.push_back(&myscrhtx);
	background.children.push_back(&mytextbox);

	sys.windows.push_back(&background);
	sys.middlescroll.speed_mult = 90.0f;
	sys.middlescroll.mat = material(textures + 4, pixel_32(255, 255, 255, 180));
	sys.set_focus(&mytextbox);

	glDisable(GL_DEPTH_TEST); 
	glEnableClientState(GL_VERTEX_ARRAY);  
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);  
	glEnableClientState(GL_COLOR_ARRAY);  
	
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

	glClearColor(62/255.f, 62/255.f, 62/255.f, 1.0f);

	event::message msg;
	atl.nearest();
	while(true) {

		if(gl.poll_events(msg)) {
			if(msg == event::close) {
				gl.set_show(gl.MINIMIZE);
			}
			if(msg == event::minimize) {
				gl.set_show(gl.MINIMIZE);
			}
			if(msg == event::maximize) {
				gl.set_show(gl.MAXIMIZE);
			}
			if(msg == event::key::down) {
				if(gl.events.key == event::keys::ESC) 
					break;
			}

			sys.update_rectangles();
			sys.poll_events();
		}

		if((fps.render() && gl.get_window_rect().good())) { 
			gl.current();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glLoadIdentity();
			
			background.rc = rect_xywh(0, 0, gl.get_window_rect().w, gl.get_window_rect().h);
			mytextbox.rc = background.rc;
			mytextbox.rc.l += 10;
			mytextbox.rc.t += settings_active ? 100 : 17;
			mytextbox.rc.r -= 10;
			if(!word_wrapping) {
				mytextbox.draft.wrap_width = 0;
				mytextbox.rc.b -= 10;
			}
			else {
				mytextbox.draft.wrap_width = mytextbox.rc.w();
			}
			settings.rc = rect_xywh(background.rc.r - 15, 0, 15, 15);
			myscrtx.align();
			myscrhtx.align();
			mytextbox.stroke.right.width = !myscrtx.is_needed();
			mytextbox.stroke.bottom.width = !myscrhtx.is_needed();

			sys.update_rectangles();
			sys.call_updaters();
			sys.update_rectangles();
			sys.update_array();
			gui::text::quick_print(sys.quad_array, L"AAAA KURWAAA\nAAAAAAAAAAAAAAAAAAAAAAAA XDDDDDDDDDDDDDDD", gui::style(fonts+3, pixel_32(255,0,0,195)), point(20, 20));

			glVertexPointer(2, GL_INT, sizeof(gui::vertex), sys.quad_array.data());
			glTexCoordPointer(2, GL_FLOAT, sizeof(gui::vertex), (char*)(sys.quad_array.data()) + sizeof(int)*2);
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(gui::vertex), (char*)(sys.quad_array.data()) + sizeof(int)*2 + sizeof(float)*2);
			glDrawArrays(GL_QUADS, 0, sys.quad_array.size() * 4);

			window::errlog.log_successful(false);
			if(!gl.swap_buffers()) break;
			window::errlog.log_successful(true);

			fps.reset();
		}

		fps.loop();
	}

	window::errlog.close();
	graphics::errlog.close();
	return 0;
}



