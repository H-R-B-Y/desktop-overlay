#include "Layers.hpp"
#include "Overlay.hpp"
#include "WorkspaceWidget.hpp"
#include "VolumeWidget.hpp"

LeftOverlay::LeftOverlay(Overlay &owner, std::string name, Config config)
	:
	Layer(owner, std::move(name), config),
	WidgetContainer()
{
}

void LeftOverlay::on_draw(cairo_t *cr, int width, int height)
{
	if (!size_known_)
	{
		size_known_ = true;
		add_widget(std::make_unique<WorkspaceWidget>(owner(), Rect{0, 40, 30, 100}, window_));
		add_widget(std::make_unique<VolumeWidget>(owner(), Rect{0, 200, 30, 100}));
	}
	std::array<float, 3> col = colours(0);
	clear_to_transparent(cr);
	cairo_set_source_rgba(cr, col[0], col[1], col[2], 1.0);
	cairo_rectangle(cr, 0, 0, width, height - 30);
	cairo_fill(cr);
	cairo_rounded_rect(cr, 0, height - 30, 30, 30, 15, RoundedCorner::BOTTOM_RIGHT);
	cairo_fill(cr);

	handle_draw_widgets(cr, width, height);

	// widget_bounds_ = {4.0, 40.0, 22.0, 22.0};
	// cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, widget_hovered_ ? 0.28 : 0.18);
	// cairo_rounded_rect(cr, widget_bounds_.x, widget_bounds_.y, widget_bounds_.w, widget_bounds_.h, 7.0, RoundedCorner::ALL);
	// cairo_fill(cr);

	// cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.92);
	// draw_centered_text(cr, "W", widget_bounds_.x, widget_bounds_.y, widget_bounds_.w, widget_bounds_.h,
	// 	"Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD, 13.0);
}

void LeftOverlay::on_hover_enter(double x, double y)
{
	on_hover_move(x, y);
}

void LeftOverlay::on_hover_move(double x, double y)
{
	// widget_hovered_ = widget_bounds_.contains(x, y);
	handle_hover_drawn_widgets(x, y);
}

void LeftOverlay::on_hover_leave()
{
	// widget_hovered_ = false;
	handle_hover_leave_widgets();
}

void	LeftOverlay::on_scroll(double dx, double dy)
{
	handle_scroll_drawn_widgets(dx, dy);
}

void LeftOverlay::on_frame(int global_tick)
{
	handle_tick_widgets(global_tick);
}