#include <Layers.hpp>
#include <Overlay.hpp>
#include <iomanip>
#include <iostream>
#include "Widgets.hpp"


TopOverlay::TopOverlay(Overlay &owner, std::string name, Config config)
	: Layer(owner, std::move(name), config), WidgetContainer(), size_known_(false),
	power_handler_()
{
}

TopOverlay::~TopOverlay()
{
	std::cout << "Destroying top overlay" << std::endl;
}

void TopOverlay::on_frame(int global_tick)
{
	// (void)global_tick;
	// { // time handler open and close paths
	// 	if (time_handler_.is_open())
	// 	{
	// 		if (!time_handler_.self)
	// 		{
	// 			time_handler_.ensure_popup<TimeMenu>(owner());
	// 			time_handler_.self->create(owner().application());
	// 			GdkRectangle	sc;
	// 			sc = owner().get_monitor_geo(GTK_WIDGET(window_));
	// 			time_handler_.self->set_margins({
	// 				(int)(time_widget_.x + (time_widget_.w / 2)) - (TimeMenu::time_menu_config_.size[0] / 2) - 30
	// 				, 0, 0, 0});
	// 		}
	// 		if (time_handler_.close_pending)
	// 		{
	// 			time_handler_.unschedule_close_popup();
	// 		}
	// 	}
	// 	else if (!time_handler_.is_open() && time_handler_.self && !time_handler_.close_pending) // this is for when we sort out timers
	// 	{
	// 		time_handler_.schedule_close_popup();
	// 	}
	// }
	handle_tick_widgets(global_tick);
	{ // power handler open and close paths
		if (power_handler_.is_open())
		{
			if (!power_handler_.self)
			{
				power_handler_.ensure_popup<PowerMenu>(owner());
				power_handler_.self->create(owner().application());
			}
			if (power_handler_.close_pending)
			{
				power_handler_.schedule_close_popup();
			}
		}
		else if (!power_handler_.is_open() && power_handler_.self && !power_handler_.close_pending)
		{
			power_handler_.schedule_close_popup();
		}
	}
}

void TopOverlay::on_draw(cairo_t *cr, int width, int height)
{
	std::array<float, 3> col = colours(0);

	const double panel_end = std::max(0, width - 30);
	const double widget_h = std::max(16.0, static_cast<double>(height) - 8.0);
	if (!size_known_)
	{
		add_widget(std::make_unique<TimeWidget>(owner(), Rect{std::max(8.0, (panel_end - 140.0) * 0.5), 0.0, 140.0, (double)height}));
		power_widget_ = {std::max(8.0, panel_end - 48.0), 0.0, 38.0, (double)height};
		size_known_ = true;
	}
	// draws the background of the panel including the cut corner
	clear_to_transparent(cr);
	cairo_set_source_rgba(cr, col[0], col[1], col[2], 1.0);
	cairo_rectangle(cr, 0, 0, panel_end, height);
	cairo_fill(cr);
	cairo_rounded_rect(cr, panel_end, 0, 30, 30, 15, RoundedCorner::BOTTOM_RIGHT);
	cairo_fill(cr);

	// draw any custom widgets on top of the background
	handle_draw_widgets(cr, width, height);

	// draws the time widget
	// cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, time_handler_.widget_hovered ? 0.28 : 0.18);
	// cairo_rounded_rect(cr, time_widget_.x, time_widget_.y + 4, time_widget_.w, widget_h, 10, RoundedCorner::ALL);
	// cairo_fill(cr);

	// draws the power widget
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, power_handler_.widget_hovered ? 0.28 : 0.18);
	cairo_rounded_rect(cr, power_widget_.x, power_widget_.y + 4, power_widget_.w, widget_h, 10, RoundedCorner::ALL);
	cairo_fill(cr);

	// cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.95);
	// draw_centered_text(cr, current_time_string("%H:%M"), time_widget_.x, time_widget_.y, time_widget_.w, time_widget_.h,
	// 	"Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD, 16.0);

	cairo_new_path(cr);
	cairo_set_line_width(cr, 2.0);
	cairo_arc(cr, power_widget_.x + power_widget_.w * 0.5, power_widget_.y + power_widget_.h * 0.5 + 1.0, 6.0, -M_PI * 0.3, M_PI * 1.3);
	cairo_stroke(cr);
	cairo_move_to(cr, power_widget_.x + power_widget_.w * 0.5, power_widget_.y + 8.0);
	cairo_line_to(cr, power_widget_.x + power_widget_.w * 0.5, power_widget_.y + power_widget_.h * 0.5);
	cairo_stroke(cr);
}

void TopOverlay::on_hover_enter(double x, double y)
{
	on_hover_move(x, y);
}

void TopOverlay::on_hover_move(double x, double y)
{
	// time_handler_.widget_hovered = time_widget_.contains(x, y);
	power_handler_.widget_hovered = power_widget_.contains(x, y);
	handle_hover_drawn_widgets(x, y);
}

void TopOverlay::on_hover_leave()
{
	// time_handler_.widget_hovered= false;
	power_handler_.widget_hovered = false;
	handle_hover_leave_widgets();
}

bool TopOverlay::on_click_pressed(int n_press, double x, double y)
{
	(void)n_press;(void)x;(void)y;
	return handle_click_drawn_widgets(n_press, x, y);
}

