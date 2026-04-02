#include "Widgets.hpp"
#include "Overlay.hpp"
#include <iostream>

TimeWidget::TimeWidget(Overlay &owner, Rect geo)
	:
	LayerWidget(LayerWidget::WidgetType::DRAWN, geo),
	owner(owner)
{
	std::cout << "created time widget" << std::endl;
}

TimeWidget::~TimeWidget(void)
{ 
	if (_handler.close_pending)
		_handler.unschedule_close_popup();
	std::cout << "destroyed time widget" << std::endl;
}

void	TimeWidget::on_tick(void)
{
	if (_handler.is_open())
	{
		if (_handler.self.get() == nullptr)
		{
			// open the popup
			_handler.self = std::make_unique<TimeMenu>(owner, &_handler);
			_handler.self->create(owner.application());
			_handler.self->set_margins({ // Note: should stop using hardcoded values for this so you actually understand what is being calculated here.
				(int)(geo.x + (geo.w / 2)) - (TimeMenu::time_menu_config_.size[0] / 2) - 30,
				0,0,0
			});
		}
		if (_handler.close_pending)
		{
			_handler.unschedule_close_popup();
		}
	}
	else
	{
		if (!_handler.close_pending)
		{
			_handler.schedule_close_popup();
		}
	}
}

void	TimeWidget::on_hover_enter(double x, double y)
{
	this->on_hover_move(x, y);
}

void	TimeWidget::on_hover_move(double x, double y)
{
	this->_handler.widget_hovered = true;
}

void	TimeWidget::on_hover_leave(void)
{
	this->_handler.widget_hovered = false;
}

void	TimeWidget::on_draw(cairo_t *cr, int width, int height)
{
	// std::cout << "width: " << width << "\nheight: " << height << std::endl; 
	// need to copy in the code from the draw method inside the top overlay
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.25 + (0.45 * _handler.widget_hovered));
	cairo_rounded_rect(cr, 0, 0 + 4, width, height - 8, 15, RoundedCorner::ALL);
	cairo_fill(cr);

	cairo_set_source_rgba(cr, 1.0,1.0,1.0,1.0);
	draw_centered_text(cr, current_time_string("%H:%M"), 0, 0, width, height,
		"Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD, 16.0);
}