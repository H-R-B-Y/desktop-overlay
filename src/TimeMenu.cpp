#include <Popups.hpp>
#include <helpers.hpp>
#include <iostream>

const Layer::Config TimeMenu::time_menu_config_ = {
		{420, 350}, // size
		GTK_LAYER_SHELL_LAYER_TOP, // layer type
		{1000,0, 0, 0}, // margins
		{true, false, true, false}, // anchors
		false, // exclusive
		true, // capture hover
		true // capture click
};

TimeMenu::TimeMenu(Overlay &owner, Popup::PopupHandler *handler)
	: Popup(owner, "TimeMenu", time_menu_config_, handler),
	something_here_(false),
	grid_(nullptr),
	time_label_(nullptr),
	date_label_(nullptr),
	calendar_(nullptr)
{

}
void TimeMenu::on_frame(int global_tick)
{
	(void)global_tick;
}
void TimeMenu::on_draw(cairo_t *cr, int width, int height)
{
	std::array<float, 3> col = colours(1);
	clear_to_transparent(cr);
	cairo_set_source_rgba(cr, col[0],col[1],col[2], 1.0);
	cairo_rounded_rect(cr, 0,0,width,height,15,RoundedCorner::BOTTOM_LEFT | RoundedCorner::BOTTOM_RIGHT);
	cairo_fill(cr);
}
void TimeMenu::on_hover_enter(double x, double y)
{
	on_hover_move(x, y);
	if (handler)
		handler->popup_hovered = true;
}

void TimeMenu::on_hover_move(double x, double y)
{
	(void)x;(void)y;
	if (handler)
		handler->popup_hovered = true;
}
void TimeMenu::on_hover_leave()
{
	if (handler)
		handler->popup_hovered = false;
}
bool TimeMenu::on_click_pressed(int n_press, double x, double y)
{
	(void)x;(void)y;(void)n_press;
	std::cout << "Clicked the time menu" << std::endl;
	return false;
}

void	TimeMenu::on_created()
{
	// std::cout << "Creating calendar widget" << std::endl;#

	grid_ = gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(grid_), 10);
	gtk_grid_set_column_spacing(GTK_GRID(grid_), 10);
	gtk_widget_set_halign(grid_, GTK_ALIGN_FILL);
	gtk_widget_set_valign(grid_, GTK_ALIGN_FILL);
	gtk_widget_set_hexpand(grid_, TRUE);
	gtk_widget_set_vexpand(grid_, TRUE);
	gtk_grid_set_column_homogeneous(GTK_GRID(grid_), TRUE);
	// gtk_grid_set_row_homogeneous(GTK_GRID(grid_), TRUE);
	gtk_widget_set_margin_bottom(grid_, 10);
	gtk_widget_set_margin_top(grid_, 10);
	gtk_widget_set_margin_end(grid_, 10);
	gtk_widget_set_margin_start(grid_, 10);
	
	time_label_ = gtk_label_new("time here");
	gtk_grid_attach(GTK_GRID(grid_), time_label_, 0, 0, 1, 1);

	date_label_ = gtk_label_new("date here");
	gtk_grid_attach(GTK_GRID(grid_), date_label_, 0, 1, 1, 1);

	calendar_ = gtk_calendar_new();
	// gtk_grid_attach(GTK_GRID(grid_), calendar_, 0, 2, 1, 1);
	// gtk_widget_set_vexpand(calendar_, TRUE);
	add_overlay_child(grid_);
}
void TimeMenu::on_closing()
{
	// std::cout << "Closing calendar widget" << std::endl;
	calendar_ = nullptr;
}
