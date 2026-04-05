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

const std::string PowerMenu::_icon_button::strs[3] = {
	"Shutdown", "Reboot", "Logout"
};

PowerMenu::_icon_button::_icon_button(PowerAction action_, PowerMenu *parent)
{
	parent_ = parent;
	action = action_;
	grid = gtk_grid_new();
	gtk_widget_add_css_class(grid, "power-menu-button");

	GtkGesture *click = gtk_gesture_click_new();
	g_signal_connect(click, "pressed", G_CALLBACK(&PowerMenu::button_click_cb), this);
	gtk_widget_add_controller(grid, GTK_EVENT_CONTROLLER(click));
	
	label = gtk_label_new(PowerMenu::_icon_button::strs[(int)action].c_str());
	gtk_widget_add_css_class(label, "power-menu-button-label");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
	gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(label, TRUE);
	gtk_widget_set_margin_top(label, 10);
	gtk_widget_set_margin_bottom(label, 5);
	gtk_widget_set_margin_start(label, 15);
	gtk_widget_set_margin_end(label, 15);
	
}

void PowerMenu::button_click_cb(GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data)
{
	(void)n_press;
	(void)x;
	(void)y;
	(void)gesture;
	PowerMenu::_icon_button	*button	= static_cast<PowerMenu::_icon_button *>(user_data);
	button->parent_->button_open_modal(button);
	gtk_gesture_set_state(GTK_GESTURE(gesture), GTK_EVENT_SEQUENCE_CLAIMED);
}

static void on_power_confirm_ready(GObject *source, GAsyncResult *result, gpointer user_data)
{
	GtkAlertDialog *dialog = GTK_ALERT_DIALOG(source);
	PowerMenuConfirmCtx	*ctx = static_cast<PowerMenuConfirmCtx *>(user_data);
	GError	*err = nullptr;

	int response = gtk_alert_dialog_choose_finish(dialog, result, &err);

	if (err)
	{
		g_warning("Confirm dialog failed: %s", err->message);
		g_error_free(err);
		g_free(ctx);
		return ;
	}
	// button 0 = confirm, 1 = cancel
	const char *argv_shutdown[] = {"systemctl", "poweroff", nullptr};
	const char *argv_reboot[] = {"systemctl", "reboot", nullptr};
	const char *argv_logout[] = {"loginctl", "terminate-user", g_get_user_name(), nullptr};
	const char *const *argv = nullptr;
	GError	*spawn_err = nullptr;
	switch (response)
	{
		case 0:
			switch (ctx->action)
			{
				case PowerMenu::PowerAction::Shutdown: argv = argv_shutdown; break;
				case PowerMenu::PowerAction::Reboot: argv = argv_reboot; break;
				case PowerMenu::PowerAction::Logout: argv = argv_logout; break;
			}
			g_subprocess_newv(argv, G_SUBPROCESS_FLAGS_NONE, &spawn_err);
			if (spawn_err)
			{
				g_warning("Failed to spawn subprocess: %s.", spawn_err->message);
				g_error_free(spawn_err);
			}
		// fallthrough
		default:
		case 1:
			ctx->handler->child_hovered = false;
			g_free(ctx);
			break ;
	}
}


void PowerMenu::button_open_modal(PowerMenu::_icon_button *button)
{

	if (confirm_modal)
		g_clear_object(&confirm_modal);
	confirm_modal = gtk_alert_dialog_new("Do you want to %s.", PowerMenu::_icon_button::strs[(int)button->action].c_str());
	gtk_alert_dialog_set_buttons(confirm_modal, (char *[3]){"Confirm", "Cancel", nullptr});
	gtk_alert_dialog_set_cancel_button(confirm_modal, 1);
	gtk_alert_dialog_set_default_button(confirm_modal, 1);
	PowerMenuConfirmCtx *ctx = g_new0(PowerMenuConfirmCtx, 1);
	ctx->action = button->action;
	ctx->handler = static_cast<PowerMenu::PowerMenuHandler *>(handler);
	ctx->handler->child_hovered = true;
	gtk_alert_dialog_choose(confirm_modal, GTK_WINDOW(window_), nullptr, on_power_confirm_ready, ctx);

}

void PowerMenu::_icon_button::close(void)
{
	grid = nullptr;
	label = nullptr;
}

PowerMenu::PowerMenu(Overlay &owner, Popup::PopupHandler *handler)
	:
	Popup(owner, "Power Menu", PowerMenu::power_menu_config_, handler),
	shutdown_button(PowerAction::Shutdown, this),
	reboot_button(PowerAction::Reboot, this),
	logout_button(PowerAction::Logout, this),
	confirm_modal(nullptr)
{
	grid = gtk_grid_new();

	gtk_widget_add_css_class(grid, "power-menu-grid");
	gtk_grid_attach(GTK_GRID(grid), shutdown_button.grid, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), reboot_button.grid, 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), logout_button.grid, 0, 2, 1, 1);
}

void PowerMenu::on_draw(cairo_t *cr, int width, int height)
{
	std::array<float, 3> col = colours(0);
	clear_to_transparent(cr);
	cairo_set_source_rgba(cr, col[0], col[1], col[2], 1.0);
	cairo_rounded_rect(cr, 0,0,width,height, 15, RoundedCorner::BOTTOM_LEFT | RoundedCorner::BOTTOM_RIGHT);
	cairo_fill(cr);
	// cairo_set_source_rgb(cr, 1.0, 0.0,0.0);
	// cairo_new_path(cr);
	// cairo_move_to(cr, width / 2, 0);
	// cairo_line_to(cr, width / 2, height);
	// cairo_stroke(cr);
}

bool PowerMenu::on_click_pressed(int n_press, double x, double y)
{
	return false;
}

void PowerMenu::on_created()
{
	gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
	gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
	gtk_widget_set_halign(grid, GTK_ALIGN_FILL);
	gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
	gtk_widget_set_hexpand(grid, TRUE);
	gtk_widget_set_vexpand(grid, TRUE);
	gtk_widget_set_margin_bottom(grid, 10);
	gtk_widget_set_margin_top(grid, 10);
	gtk_widget_set_margin_end(grid, 10);
	gtk_widget_set_margin_start(grid, 10);
	gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
	add_overlay_child(grid);
}

void PowerMenu::on_closing()
{
	shutdown_button.close();
	reboot_button.close();
	logout_button.close();
	grid = nullptr;
}
