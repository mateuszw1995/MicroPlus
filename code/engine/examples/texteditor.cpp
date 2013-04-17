#include "db.h"
#include "customgui/gui1.h"
#include <ctime>

void resizer(glwindow& gl) {
	gl.current();
	glViewport(0, 0, gl.get_window_rect().w, gl.get_window_rect().h);
	
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho(0, gl.get_window_rect().w, gl.get_window_rect().h, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
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
	gl.create(math::rect_xywh(20, 20, 794, 800), true);
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

	gui::system sys(gl.events);
	
	stylesheet sliderbox;
	sliderbox.released.color = gray3;
	sliderbox.hovered.color = gray4;
	sliderbox.pushed.color = white;

	stylesheet scrollareabox;
	scrollareabox.released.color = gray2;

	stylesheet labelbox_inactive;
	labelbox_inactive.released.border = solid_stroke(1, gray1);
	labelbox_inactive.hovered.border = solid_stroke(1, ltblue);
	labelbox_inactive.pushed.border = solid_stroke(2, ltblue);
	
	stylesheet labelbox_active;
	labelbox_active.released.border = solid_stroke(1, ltblue);
	labelbox_active.hovered.border = solid_stroke(2, ltblue);
	labelbox_active.pushed.border = solid_stroke(3, ltblue);

	stylesheet checkbox_active(stylesheet::style  (ltblue, gui::null_texture, solid_stroke(0)));
	checkbox_active.hovered.border = solid_stroke(1, ltblue);
	checkbox_active.pushed.border = solid_stroke (2, ltblue);
	stylesheet checkbox_inactive(checkbox_active);
	checkbox_inactive.released.color = white;

	stylesheet bold_button_active(checkbox_active);
	stylesheet bold_button_inactive(checkbox_inactive);
	stylesheet italics_button_active(checkbox_active);
	stylesheet italics_button_inactive(checkbox_inactive);
	bold_button_active.released.background_image    = bold_button_inactive.released.background_image    = textures + 2;
	italics_button_active.released.background_image = italics_button_inactive.released.background_image = textures + 3;

	stylesheet bgcol(stylesheet::style(darkblue, gui::null_texture, solid_stroke(0)));
	stylesheet textfield(stylesheet::style(gray1, gui::null_texture, solid_stroke(1, ltblue)));

	cbackground background(rect_ltrb(0, 0, gl.get_window_rect().w-20, gl.get_window_rect().h-20), bgcol);
	
	ctextbox  mytextbox(textbox(rect_xywh(rect_ltrb(10, 17, gl.get_window_rect().w-20, gl.get_window_rect().h-20)), text::style(fonts + 0, white)), textfield);
	/* ui settings */
	// mytextbox.whitelist = L"0123456789.";
	// mytextbox.max_characters = 15;

	/* structural settings */
	mytextbox.editor.draft().wrap_width = 0;
	mytextbox.editor.draft().kerning = false;
	
	/* visual settings */
	mytextbox.print.blink.blink = true;
	mytextbox.print.blink.interval_ms = 500;
	mytextbox.print.selection_bg_mat.color = pixel_32(128, 255, 255, 120);
	mytextbox.print.selection_inactive_bg_mat.color = pixel_32(128, 255, 255, 40);
	mytextbox.print.highlight_mat = material(pixel_32(15, 15, 15, 255)); 
	mytextbox.print.caret_mat.color = pixel_32(255, 255, 255, 255);
	mytextbox.print.highlight_current_line = true;
	mytextbox.print.highlight_during_selection = true;
	mytextbox.print.align_caret_height = true;
	mytextbox.print.caret_width = 1;;
	mytextbox.drag.vel_mult = 100.0;
	
	text_modifier bold_button(false, rect_xywh (230, 75, 20, 20), bold_button_active, bold_button_inactive, 
		std::bind(&ctextbox::on_bold, &mytextbox),
		std::bind(&text::ui::get_bold_status, &mytextbox.editor)
		);

	text_modifier italics_button(false, rect_xywh (260, 75, 20, 20), italics_button_active, italics_button_inactive, 
		std::bind(&ctextbox::on_italics, &mytextbox),
		std::bind(&text::ui::get_italics_status, &mytextbox.editor)
		);

	
	cslider   sl(scrollarea::slider(20), sliderbox);
	cslider  slh(scrollarea::slider(20), sliderbox);
	cscrollarea  myscrtx(scrollarea (rect_xywh(0, 0, 10, 0), &mytextbox, &sl, scrollarea::orientation::VERTICAL), scrollareabox);
	cscrollarea myscrhtx(scrollarea (rect_xywh(0, 0, 0, 10), &mytextbox, &slh, scrollarea::orientation::HORIZONTAL), scrollareabox);

	text::style   active(fonts + 1, ltblue);
	text::style inactive(fonts + 0, gray1);

	ccheckbox  settings(false, rect_xywh(20, 47, 200, 15), checkbox_active, checkbox_inactive,
		[&mytextbox](bool set){ mytextbox.rc.t = set ? 100 : 17; }
	);

	settings.styles_active.released.background_image = textures + 1; 
	settings.styles_inactive.released.background_image = textures + 1; 

	cchecklabel  highlight(checklabel(mytextbox.print.highlight_current_line, rect_xywh(20, 17, 200, 15), L"Highlight current line", active, inactive), labelbox_active, labelbox_inactive,
		[&mytextbox](bool set){mytextbox.print.highlight_current_line = set;}
	);
	cchecklabel  kerning(checklabel  (mytextbox.editor.get_draft().kerning, rect_xywh(20, 47, 200, 15), L"Kerning", active, inactive), labelbox_active, labelbox_inactive,
		[&mytextbox](bool set){mytextbox.editor.draft().kerning = set;}
	);
	cchecklabel  word_wrap(checklabel(true, rect_xywh(20, 77, 200, 15), L"Word wrapping", active, inactive), labelbox_active, labelbox_inactive,
		[&mytextbox, &background, &myscrhtx](bool set) mutable {mytextbox.editor.draft().wrap_width = set ? mytextbox.rc.w() : 0;
								if(set) mytextbox.rc.b = background.rc.b;
								else    mytextbox.rc.b = background.rc.b-10;
								myscrhtx.align();
			}
	);

	//highlight.scroll = point(-5, -5);
	//kerning.scroll = point(5, 5);
	highlight.snap_scroll_to_content = false;
	kerning.snap_scroll_to_content = false;

	
	//background.scroll = point(-10, -10);
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

	gl.resize = [&](glwindow& gl) mutable {
		resizer(gl); 
		background.rc = rect_xywh(0, 0, gl.get_window_rect().w, gl.get_window_rect().h);
		//mytextbox.rc = background.rc;
		mytextbox.rc.r = background.rc.r - 10;
		//mytextbox.rc.b = background.rc.b;
		
		settings.rc = rect_xywh(background.rc.r - 15, 0, 15, 15);
		word_wrap.callback(word_wrap.get_state());

		myscrtx.align();
		myscrhtx.align();
	};

	gl.resize(gl);
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

			mytextbox.styles.released.border.value.right.width = !myscrtx.is_needed();
			mytextbox.styles.released.border.value.bottom.width = !myscrhtx.is_needed();

			sys.update_rectangles();
			sys.call_updaters();
			sys.update_rectangles();
			sys.update_array();
			// gui::text::quick_print(sys.quad_array, L"AAAA KURWAAA\nAAAAAAAAAAAAAAAAAAAAAAAA XDDDDDDDDDDDDDDD", text::style(fonts+3, pixel_32(255,0,0,195)), point(20, 20));

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
		int chujnia = gl.get_window_rect().w;
	}

	window::errlog.close();
	graphics::errlog.close();
	return 0;
}



