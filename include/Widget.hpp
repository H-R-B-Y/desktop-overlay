#pragma once

#include "helpers.hpp"
#include "Rect.hpp"


class LayerWidget
{
public:
	enum WidgetType{
		DRAWN,
		GTK,
	};

	LayerWidget(WidgetType type, Rect geo);
	LayerWidget(Rect geo, GtkWidget *_widget);
	virtual ~LayerWidget();

	void					draw(cairo_t *cr);
	virtual void			on_tick(void);
	virtual bool			on_click(int n_click, double x, double y);
	virtual void			on_hover_enter(double x, double y);
	virtual void			on_hover_move(double x, double y);
	virtual void			on_hover_leave();
	// virtual void	on_scroll();
	
	bool					hit_test(double x, double y);
	std::array<double, 2>	to_local(double x, double y);
	const WidgetType		type;
	Rect					widget_get_geo(void) const;
	GtkWidget				*widget_get_widget(void) const;


protected:
	virtual void	on_draw(cairo_t *cr, int width, int height);
	Rect					geo;

private:
	// these are provided for default handlers if needed but custom handlers can be installed if required
	static void hover_enter_cb(GtkEventControllerMotion *controller, double x, double y, gpointer user_data);
	static void hover_move_cb(GtkEventControllerMotion *controller, double x, double y, gpointer user_data);
	static void hover_leave_cb(GtkEventControllerMotion *controller, gpointer user_data);
	static void click_pressed_cb(GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data);

	GtkWidget		*const	_widget =  nullptr;
};