#pragma once
#include "stroke.h"
#include "rect.h"

namespace db {
	namespace graphics {
		namespace gui {
			struct stylesheet {

				template <class T>
				struct attribute {
					bool active;
					T value;

					attribute(const T& value) : value(value), active(false) {}

					void set(const T& v) {
						value = v;
						active = true;
					}

					attribute& operator=(const T& v) { 
						set(v); 
						return *this; 
					}

					attribute& operator=(const attribute& attr) { 
						if(attr.active) { 
							value = attr.value;
							active = true; 
						} 
						return *this;
					}

					operator T() const {
						return value;
					}
				};

				struct style {
					attribute<pixel_32> color;
					attribute<io::input::texture*> background_image;
					attribute<solid_stroke> border;

					style();
					style(const attribute<pixel_32>& color, 
						const attribute<io::input::texture*>& background_image, 
						const attribute<solid_stroke>& border);

					operator material() const;

				} released, hovered, pushed;

				stylesheet(const style& released = style(), const style& hovered = style(), const style& pushed = style());

				rect::appearance current_appearance;

				void update_appearance(rect::event);
				const style& get_style() const;
			};
		}
	}
}