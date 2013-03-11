#include "db.h"
#include "customgui/gui1.h"
#include <ctime>

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
	
	errlog.log_successful(true);
	graphics::errlog.log_successful(true);
	window::errlog.log_successful(true);
	
	glwindow::init();
	glwindow gl("Conspiracy", 0);
	gl.create(math::rect_xywh(20, 20, 800, 800), true);
	gl.resize_func(resizer);
	gl.set_show(gl.SHOW); 
	gl.current();
	resizer(gl);
	window::errlog.enable(false);
	
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

	gui::system sys(&gl.events);
	cbackground background(rect(rect_xywh(0, 0, gl.get_window_rect().w-20, gl.get_window_rect().h-20), material(gui::null_texture, pixel_32(6, 5, 20, 255))));
	
	text mytext(fonts + 0, white);
	ctextbox  mytextbox(textbox(rect_xywh(), material(gui::null_texture, white), &mytext));
	
	mytext.blink.blink = true;
	mytext.blink.interval_ms = 500;
	mytext.str().clear();
	mytext.kerning = false;
	mytext.select.bg_mat.color = pixel_32(128, 255, 255, 120);
	mytext.select.inactive_bg_mat.color = pixel_32(128, 255, 255, 40);
	mytext.highlight_mat = material(gui::null_texture, pixel_32(15, 15, 15, 255)); 
	mytext.highlight_current_line = false;
	mytext.highlight_during_selection = true;
	mytext.wrap_width = 0;
	mytext.align_caret_height = true;
	mytext.caret_width = 1;
	mytext.caret_mat.color = pixel_32(255,255,255,255);
	//mytext.whitelist = L"0123456789.";
	//mytext.max_characters = 15;
	mytextbox.mat.color = darkgray;
	mytextbox.scroller.vel_mult = 100.0;
	

	ctext_modifier bold_button(rect(rect_xywh (230, 75, 20, 20), textures + 2), &mytextbox, ctext_modifier::BOLDEN);
	ctext_modifier italics_button(rect(rect_xywh (260, 75, 20, 20), textures + 3), &mytextbox, ctext_modifier::ITALICSEN);
	
	ccolortickbox::labels_active = fonts + 1;
	ccolortickbox::labels_inactive = fonts + 0;
	ccolortickbox  settings(0, rect(rect_xywh(background.rc.r - 15, 0, 15, 15),material(textures+1)), settings_active, ltblue, pixel_32(180, 180, 180, 255));
	ccolortickbox  highlight(L" Highlight current line", rect(rect_xywh(20, 17, 200, 15),material(gui::null_texture)), mytext.highlight_current_line);
	ccolortickbox  kerning(L" Kerning", rect(rect_xywh(20, 47, 200, 15),material(gui::null_texture)), mytext.kerning);
	ccolortickbox  word_wrap(L" Word wrapping", rect(rect_xywh(20, 77, 200, 15),material(gui::null_texture)), word_wrapping);
	
	cslider   sl(scrollarea::slider(20, material(gui::null_texture, pixel_32(104, 104, 104, 255))));
	cslider  slh(scrollarea::slider(20, material(gui::null_texture, pixel_32(104, 104, 104, 255))));
	scrollarea  myscrtx(rect_xywh(0, 0, 10, 0), material(gui::null_texture, pixel_32(62, 62, 62, 255)), &mytextbox, &sl, scrollarea::orientation::VERTICAL);
	scrollarea myscrhtx(rect_xywh(0, 0, 0, 10), material(gui::null_texture, pixel_32(62, 62, 62, 255)), &mytextbox, &slh, scrollarea::orientation::HORIZONTAL);
	
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
				mytext.wrap_width = 0;
				mytextbox.rc.b -= 10;
			}
			else {
				mytext.wrap_width = mytextbox.rc.w();
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

			glVertexPointer(2, GL_INT, sizeof(gui::vertex), sys.rect_array.data());
			glTexCoordPointer(2, GL_FLOAT, sizeof(gui::vertex), (char*)(sys.rect_array.data()) + sizeof(int)*2);
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(gui::vertex), (char*)(sys.rect_array.data()) + sizeof(int)*2 + sizeof(float)*2);
			glDrawArrays(GL_QUADS, 0, sys.rect_array.size() * 4);

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



