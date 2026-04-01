# include "Popups.hpp"
# include "helpers.hpp"

const Layer::Config PowerMenu::power_menu_config_ = {
		{200, 150}, // size
		GTK_LAYER_SHELL_LAYER_TOP, // layer type
		{0,30, 0, 0}, // margins
		{false, true, true, false}, // anchors
		false, // exclusive
		true, // capture hover
		true // capture click
};

PowerMenu::PowerMenu(Overlay &owner, Popup::PopupHandler *handler)
	:
	Popup(owner, "Power Menu", PowerMenu::power_menu_config_, handler)
{
}

void PowerMenu::on_draw(cairo_t *cr, int width, int height)
{
	std::array<float, 3> col = colours(0);
	clear_to_transparent(cr);
	cairo_set_source_rgba(cr, col[0], col[1], col[2], 1.0);
	cairo_rounded_rect(cr, 0,0,width,height, 15, RoundedCorner::ALL);
	cairo_fill(cr);
	cairo_set_source_rgb(cr, 1.0, 0.0,0.0);
	cairo_new_path(cr);
	cairo_move_to(cr, width / 2, 0);
	cairo_line_to(cr, width / 2, height);
	cairo_stroke(cr);
}

bool PowerMenu::on_click_pressed(int n_press, double x, double y)
{
	return false;
}

void PowerMenu::on_created()
{

}

void PowerMenu::on_closing()
{

}