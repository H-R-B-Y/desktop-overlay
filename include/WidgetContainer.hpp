#include "Widget.hpp"
#include <memory>
#include <vector>

class WidgetContainer 
{
public:
	WidgetContainer();
	virtual ~WidgetContainer();

	void	handle_hover_drawn_widgets(double x, double y);
	void	handle_hover_leave_widgets(void);
	bool	is_hovered() const;
	bool	handle_click_drawn_widgets(int n_click, double x, double y);
	void	handle_draw_widgets(cairo_t *cairo, int width, int height);
	void	handle_tick_widgets(int global_tick);
	void	add_widget(std::unique_ptr<LayerWidget> widget);

protected:
	GtkWidget									*container;

private:
	LayerWidget									*hovered;
	std::vector<std::unique_ptr<LayerWidget>>	drawn_widgets;
	std::vector<std::unique_ptr<LayerWidget>>	gtk_widgets;
};