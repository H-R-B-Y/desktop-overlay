
#include "WidgetContainer.hpp"
#include <iostream>

WidgetContainer::WidgetContainer()
	:
	hovered(nullptr)
{
	this->container = gtk_fixed_new();
}

WidgetContainer::~WidgetContainer()
{
}

void	WidgetContainer::handle_hover_drawn_widgets(double x, double y)
{
	std::array<double, 2>	local;
	for (std::vector<std::unique_ptr<LayerWidget>>::iterator a = drawn_widgets.begin();
		a != drawn_widgets.end(); a++)
	{
		if (a->get()->hit_test(x, y))
		{
			local = a->get()->to_local(x, y);
			if (a->get() != hovered)
			{
				if (hovered)
					hovered->on_hover_leave();
				hovered = a->get();
				hovered->on_hover_enter(local[0], local[1]);
			}
			else if (hovered)
			{
				hovered->on_hover_move(local[0], local[1]);
			}
			return ;
		}
	}
	handle_hover_leave_widgets();
	return ;
}

void	WidgetContainer::handle_hover_leave_widgets(void)
{
	if (hovered)
	{
		hovered->on_hover_leave();
		hovered = nullptr;
	}
}

bool	WidgetContainer::handle_click_drawn_widgets(int button, int n_click, double x, double y)
{
	if (!hovered)
		return false;
	return hovered->on_click(button, n_click, x, y);
}

bool	WidgetContainer::is_hovered() const
{
	return hovered != nullptr;
}

void	WidgetContainer::add_widget(std::unique_ptr<LayerWidget> widget)
{
	// std::cout << "adding new widget: "<< widget.get() << std::endl;
	if (widget->type == LayerWidget::WidgetType::DRAWN)
	{
		drawn_widgets.push_back(std::move(widget));
	}
	else
	{
		gtk_fixed_put(GTK_FIXED(container), widget->widget_get_widget(), widget->widget_get_geo().x, widget->widget_get_geo().y);
		gtk_widgets.push_back(std::move(widget));
	}
}

void	WidgetContainer::handle_draw_widgets(cairo_t *cr, int width, int height)
{
	for (std::vector<std::unique_ptr<LayerWidget>>::iterator a = drawn_widgets.begin();
		a != drawn_widgets.end(); a++)
	{
		// std::cout << "calling draw for: "<<a->get()<<std::endl;
		a->get()->draw(cr);
	}
}

void	WidgetContainer::handle_tick_widgets(int global_tick)
{
	(void)global_tick;
	for (std::vector<std::unique_ptr<LayerWidget>>::iterator a = drawn_widgets.begin();
		a != drawn_widgets.end(); a++)
	{
		a->get()->on_tick();
	}
}

void	WidgetContainer::handle_scroll_drawn_widgets(double dx, double dy)
{
	if (!hovered)
		return ;
	hovered->on_scroll(dx, dy);
}