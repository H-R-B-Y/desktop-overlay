
#include "WidgetContainer.hpp"

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
	if (hovered)
	{
		hovered->on_hover_leave();
		hovered = nullptr;
	}
	return ;
}

bool	WidgetContainer::handle_click_drawn_widgets(int n_click, double x, double y)
{
	if (!hovered)
		return ;
	return hovered->on_click(n_click, x, y);
}

bool	WidgetContainer::is_hovered() const
{
	return hovered != nullptr;
}

void	WidgetContainer::add_widget(LayerWidget *widget)
{
	if (widget->type == LayerWidget::WidgetType::DRAWN)
		drawn_widgets.push_back(std::make_unique<LayerWidget>(widget));
	else
		gtk_widgets.push_back(std::make_unique<LayerWidget>(widget));
}
