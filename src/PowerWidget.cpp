
#include "Widgets.hpp"
#include <Overlay.hpp>

PowerWidget::PowerWidget(Overlay &owner, Rect geo)
	:
	LayerWidget(LayerWidget::WidgetType::DRAWN, geo),
	owner(owner)
{
}

PowerWidget::~PowerWidget()
{
	if (_handler.close_pending)
		_handler.unschedule_close_popup();
}

void	PowerWidget::on_hover_enter(double x, double y)
{
	on_hover_move(x, y);
}

void	PowerWidget::on_hover_move(double x, double y)
{
	_handler.widget_hovered = true;
}

void	PowerWidget::on_hover_leave(void)
{
	_handler.widget_hovered = false;
}

void	PowerWidget::on_tick(void)
{
	if (_handler.is_open())
	{
		if (!_handler.self)
		{
			_handler.ensure_popup<PowerMenu>(owner);
			_handler.self->create(owner.application());
		}
		if (_handler.close_pending)
		{
			_handler.unschedule_close_popup();
		}
	}
	else if (_handler.self && !_handler.close_pending)
	{
		_handler.schedule_close_popup();
	}
}

void	PowerWidget::on_draw(cairo_t *cr, int width, int height)
{
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, _handler.widget_hovered ? 0.28 : 0.18);
	cairo_rounded_rect(cr, 0, 4, geo.w, geo.h - 8, 15, RoundedCorner::ALL);
	cairo_fill(cr);
	cairo_new_path(cr);
	cairo_set_line_width(cr, 2.0);
	std::array<float, 3> col = colours(1);
	cairo_set_source_rgba(cr, col[0], col[1], col[2], 1.0);
	cairo_arc(cr, 0 + geo.w * 0.5, 0 + geo.h * 0.5 + 1.0, 6.0, -M_PI * 0.3, M_PI * 1.3);
	cairo_stroke(cr);
	cairo_move_to(cr, 0 + geo.w * 0.5, 0 + 8.0);
	cairo_line_to(cr, 0 + geo.w * 0.5, 0 + geo.h * 0.5);
	cairo_stroke(cr);
}
