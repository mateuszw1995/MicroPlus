#pragma once
#include "stylesheet.h"
#include "rect.h"

namespace db {
	namespace graphics {
		namespace gui {
			stylesheet::style::style() 
				: color(pixel_32()), background_image(gui::null_texture), border(solid_stroke(0)) {
			}

			stylesheet::style::style(const attribute<pixel_32>& c, 
				const attribute<io::input::texture*>& b, 
				const attribute<solid_stroke>& br) 
				: color(c), background_image(b), border(br) {
					color.active = background_image.active = border.active = true;
			}

			stylesheet::style::operator material() const {
				return material(background_image.active ? background_image : null_texture, color.active ? color : pixel_32(255, 255, 255, 255));
			}

			stylesheet::stylesheet(const style& released, const style& hovered, const style& pushed) 
				: released(released), hovered(hovered), pushed(pushed), current_appearance(rect::appearance::released) {}

			void stylesheet::update_appearance(rect::event m) {
				auto app = rect::get_appearance(m);
				if(app != rect::appearance::unknown) current_appearance = app;
			}

			const stylesheet::style& stylesheet::get_style() const {
				style result = released;
				
				/* operator='s take care of copying only with active members */
				if(current_appearance >= rect::appearance::hovered) result.operator=(hovered); 
				if(current_appearance == rect::appearance::pushed ) result.operator=(pushed); 

				return result;
			}
		}
	}
}