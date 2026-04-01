#pragma once
#include <gtk/gtk.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <array>


void clear_to_transparent(cairo_t *cr);

// Enum for specifying which corners should be rounded
enum class RoundedCorner {
	TOP_LEFT = 1,
	TOP_RIGHT = 2,
	BOTTOM_RIGHT = 4,
	BOTTOM_LEFT = 8,
	ALL = 15  // TOP_LEFT | TOP_RIGHT | BOTTOM_RIGHT | BOTTOM_LEFT
};

RoundedCorner operator|(RoundedCorner a, RoundedCorner b);

bool has_corner(RoundedCorner flags, RoundedCorner corner);

void cairo_rounded_rect(cairo_t *cr, double x, double y, double w, double h, double r, RoundedCorner corners);


// Enum for specifying which corner should have the concave cut
enum class ConcaveCorner {
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT
};

// Draw a panel with a concave (inward) rounded cut on a specified corner.
void draw_concave_panel(cairo_t *cr, double x, double y, double w, double h, double r, ConcaveCorner corner, double alpha);

void draw_centered_text(cairo_t *cr, const std::string &text, double x, double y, double w, double h,
	const char *font, cairo_font_slant_t slant, cairo_font_weight_t weight, double size);

std::array<float, 3> colours(int idx);

std::tm localtime_now();

std::string current_time_string(const char *fmt);
