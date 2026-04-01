#include "helpers.hpp"


void clear_to_transparent(cairo_t *cr)
{
	cairo_save(cr);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.0);
	cairo_paint(cr);
	cairo_restore(cr);
}


RoundedCorner operator|(RoundedCorner a, RoundedCorner b) {
	return static_cast<RoundedCorner>(static_cast<int>(a) | static_cast<int>(b));
}

bool has_corner(RoundedCorner flags, RoundedCorner corner) {
	return (static_cast<int>(flags) & static_cast<int>(corner)) != 0;
}

void cairo_rounded_rect(cairo_t *cr, double x, double y, double w, double h, double r, RoundedCorner corners = RoundedCorner::ALL)
{
	// Radius must not exceed half width/height.
	r = std::max(0.0, std::min(r, std::min(w, h) * 0.5));

	const double x2 = x + w;
	const double y2 = y + h;

	cairo_new_sub_path(cr);
	
	// Start from top-left, moving clockwise
	cairo_move_to(cr, x + (has_corner(corners, RoundedCorner::TOP_LEFT) ? r : 0.0), y);

	// Top-right corner
	cairo_line_to(cr, x2 - (has_corner(corners, RoundedCorner::TOP_RIGHT) ? r : 0.0), y);
	if (has_corner(corners, RoundedCorner::TOP_RIGHT)) {
		cairo_arc(cr, x2 - r, y + r, r, -M_PI / 2.0, 0.0);
	}

	// Bottom-right corner
	cairo_line_to(cr, x2, y2 - (has_corner(corners, RoundedCorner::BOTTOM_RIGHT) ? r : 0.0));
	if (has_corner(corners, RoundedCorner::BOTTOM_RIGHT)) {
		cairo_arc(cr, x2 - r, y2 - r, r, 0.0, M_PI / 2.0);
	}

	// Bottom-left corner
	cairo_line_to(cr, x + (has_corner(corners, RoundedCorner::BOTTOM_LEFT) ? r : 0.0), y2);
	if (has_corner(corners, RoundedCorner::BOTTOM_LEFT)) {
		cairo_arc(cr, x + r, y2 - r, r, M_PI / 2.0, M_PI);
	}

	// Top-left corner
	cairo_line_to(cr, x, y + (has_corner(corners, RoundedCorner::TOP_LEFT) ? r : 0.0));
	if (has_corner(corners, RoundedCorner::TOP_LEFT)) {
		cairo_arc(cr, x + r, y + r, r, M_PI, 3.0 * M_PI / 2.0);
	}

	cairo_close_path(cr);
}


// Draw a panel with a concave (inward) rounded cut on a specified corner.
void draw_concave_panel(cairo_t *cr, double x, double y, double w, double h, double r, ConcaveCorner corner, double alpha)
{
	(void)alpha;
	r = std::max(0.0, std::min(r, std::min(w, h)));
	const double x2 = x + w;
	const double y2 = y + h;

	cairo_new_path(cr);

	switch (corner) {
		case ConcaveCorner::TOP_RIGHT:
			// Outer shape (clockwise from top-right)
			cairo_move_to(cr, x2, y2);
			cairo_line_to(cr, x2, y);
			cairo_arc(cr, x, y, r, 0.0, M_PI / 2.0);  // 0deg -> 90deg
			cairo_line_to(cr, x, y2);
			cairo_close_path(cr);
			break;

		case ConcaveCorner::BOTTOM_RIGHT:
			// Outer shape (clockwise from top-right)
			cairo_move_to(cr, x2, y);
			cairo_line_to(cr, x, y);
			cairo_line_to(cr, x, y2);
			cairo_arc(cr, x2, y2, r, M_PI, 3.0 * M_PI / 2.0);  // 180deg -> 270deg
			cairo_close_path(cr);
			break;

		case ConcaveCorner::BOTTOM_LEFT:
			// Outer shape (clockwise from top-right)
			cairo_move_to(cr, x2, y);
			cairo_line_to(cr, x, y);
			cairo_arc(cr, x, y2, r, M_PI / 2.0, M_PI);  // 90deg -> 180deg
			cairo_line_to(cr, x2, y2);
			cairo_close_path(cr);
			break;

		case ConcaveCorner::TOP_LEFT:
			// Outer shape (clockwise from top-right)
			cairo_move_to(cr, x2, y);
			cairo_arc(cr, x, y, r, -M_PI / 2.0, 0.0);  // -90deg -> 0deg
			cairo_line_to(cr, x2, y2);
			cairo_line_to(cr, x, y2);
			cairo_close_path(cr);
			break;
	}
}

void draw_centered_text(cairo_t *cr, const std::string &text, double x, double y, double w, double h,
	const char *font, cairo_font_slant_t slant, cairo_font_weight_t weight, double size)
{
	cairo_select_font_face(cr, font, slant, weight);
	cairo_set_font_size(cr, size);

	cairo_text_extents_t ext;
	cairo_text_extents(cr, text.c_str(), &ext);
	const double tx = x + (w - ext.width) * 0.5 - ext.x_bearing;
	const double ty = y + (h - ext.height) * 0.5 - ext.y_bearing;
	cairo_move_to(cr, tx, ty);
	cairo_show_text(cr, text.c_str());
}

# define c_f_c(r, g, b) {r/255.0f, g/255.0f, b/255.0f}
std::array<float, 3> colours(int idx)
{
	switch (idx)
	{
		default:
		case 0:
			return std::array<float, 3>c_f_c(54,50,50);
		case 1:
			return std::array<float, 3>c_f_c(192,151,207);
		case 2:
			return std::array<float, 3>c_f_c(130,96,162);
		case 3:
			return std::array<float, 3>c_f_c(238,238,238);
		case 4:
			return std::array<float, 3>c_f_c(194,123,160);
	}
}
# undef c_f_c

std::tm localtime_now()
{
	std::time_t now = std::time(nullptr);
	std::tm local_tm{};
	localtime_r(&now, &local_tm);
	return local_tm;
}

std::string current_time_string(const char *fmt)
{
	std::tm local_tm = localtime_now();
	std::ostringstream ss;
	ss << std::put_time(&local_tm, fmt);
	return ss.str();
}