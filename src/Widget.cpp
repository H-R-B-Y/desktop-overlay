#include "Widget.hpp"

LayerWidget::LayerWidget(WidgetType type, Rect geo)
	:
	geo(geo),
	type(type)
{
}

LayerWidget::~LayerWidget() = default;

bool	LayerWidget::on_click(int n_click, double x, double y)
{
	return false;
}

void	LayerWidget::on_hover_enter(double x, double y)
{
	on_hover_move(x, y);
}

void	LayerWidget::on_hover_move(double x, double y)
{

}

void	LayerWidget::on_hover_leave()
{

}

void	LayerWidget::draw(cairo_t *cr)
{
	cairo_save(cr);
	cairo_rectangle(cr, geo.x, geo.h, geo.w, geo.h);
	cairo_clip(cr);
	cairo_translate(cr, geo.x, geo.y);
	on_draw(cr, geo.w, geo.h);
	cairo_restore(cr);
}

void	LayerWidget::on_draw(cairo_t *cr, int width, int height)
{

}

bool	LayerWidget::hit_test(double x, double y)
{
	if (geo.contains(x, y))
		return true;
	return false;
}

void	LayerWidget::hover_enter_cb(GtkEventControllerMotion *controller, double x, double y, gpointer user_data)
{
	(void)controller;
	LayerWidget *self = static_cast<LayerWidget *>(user_data);
	self->on_hover_enter(x, y);
}

void	LayerWidget::hover_move_cb(GtkEventControllerMotion *controller, double x, double y, gpointer user_data)
{
	(void)controller;
	LayerWidget *self = static_cast<LayerWidget *>(user_data);
	self->on_hover_move(x, y);
}

void	LayerWidget::hover_leave_cb(GtkEventControllerMotion *controller, gpointer user_data)
{
	(void)controller;
	LayerWidget *self = static_cast<LayerWidget *>(user_data);
	self->on_hover_leave();
}

void	LayerWidget::click_pressed_cb(GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data)
{
	(void)gesture;
	LayerWidget *self = static_cast<LayerWidget *>(user_data);
	if (self->on_click(n_press, x, y))
		gtk_gesture_set_state(GTK_GESTURE(gesture), GTK_EVENT_SEQUENCE_CLAIMED);
}

std::array<double, 2> LayerWidget::to_local(double x, double y)
{
	return std::array<double, 2>{x - geo.x, y - geo.y};
}
