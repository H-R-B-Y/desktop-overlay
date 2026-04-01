#include "Widget.hpp"
#include <memory>
#include <vector>

class WidgetContainer 
{
public:
	WidgetContainer();
	virtual ~WidgetContainer();

	void	handle_hover_drawn_widgets(double x, double y);
	bool	handle_click_drawn_widgets(int n_click, double x, double y);
	bool	is_hovered() const;

	void	add_widget(LayerWidget *widget);

protected:
	GtkWidget									*container;

private:
	LayerWidget									*hovered;
	std::vector<std::unique_ptr<LayerWidget>>	drawn_widgets;
	std::vector<std::unique_ptr<LayerWidget>>	gtk_widgets;
};