#include "Layers.hpp"

LeftOverlay::LeftOverlay(Overlay &owner, std::string name, Config config)
	: Layer(owner, std::move(name), config), widget_hovered_(false)
{
}

void LeftOverlay::on_draw(cairo_t *cr, int width, int height)
{
	std::array<float, 3> col = colours(0);
	clear_to_transparent(cr);
	cairo_set_source_rgba(cr, col[0], col[1], col[2], 1.0);
	cairo_rectangle(cr, 0, 0, width, height - 30);
	cairo_fill(cr);
	cairo_rounded_rect(cr, 0, height - 30, 30, 30, 15, RoundedCorner::BOTTOM_RIGHT);
	cairo_fill(cr);

	widget_bounds_ = {4.0, 40.0, 22.0, 22.0};
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, widget_hovered_ ? 0.28 : 0.18);
	cairo_rounded_rect(cr, widget_bounds_.x, widget_bounds_.y, widget_bounds_.w, widget_bounds_.h, 7.0, RoundedCorner::ALL);
	cairo_fill(cr);

	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.92);
	draw_centered_text(cr, "W", widget_bounds_.x, widget_bounds_.y, widget_bounds_.w, widget_bounds_.h,
		"Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD, 13.0);
}

void LeftOverlay::on_hover_enter(double x, double y)
{
	on_hover_move(x, y);
}

void LeftOverlay::on_hover_move(double x, double y)
{
	widget_hovered_ = widget_bounds_.contains(x, y);
}

void LeftOverlay::on_hover_leave()
{
	widget_hovered_ = false;
}