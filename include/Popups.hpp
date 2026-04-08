#pragma once

# include <Popup.hpp>
#include <iostream>
#include <iomanip>

class TimeMenu : public Popup {
public:
	static const Config time_menu_config_;
	TimeMenu(Overlay &owner, Popup::PopupHandler *handler);
protected:
	void on_frame(int global_tick) override;
	void on_draw(cairo_t *cr, int width, int height) override;
	void on_hover_enter(double x, double y) override;
	void on_hover_move(double x, double y) override;
	void on_hover_leave() override;
	bool on_click_pressed(int button, int n_press, double x, double y) override;
	void on_created() override;
	void on_closing() override;

private:
	bool something_here_;
	GtkWidget	*grid_;
	GtkWidget	*time_label_;
	GtkWidget	*date_label_;
	GtkWidget	*calendar_;
};

class PowerMenu : public Popup {
public:
	class PowerMenuHandler : public Popup::PopupHandler
	{
	public:
		bool	child_hovered = false;
		inline bool is_open(void) override
		{
			// std::cout << std::boolalpha
			// 	<< "Widget hovered: " << (bool)widget_hovered
			// 	<< "\nPopup hovered: " << (bool)popup_hovered
			// 	<< "\nChild hovered: "<< (bool)child_hovered << std::endl;
			return child_hovered | widget_hovered | popup_hovered;
		};
	};

	enum PowerAction{
		Shutdown,
		Reboot,
		Logout
	};

	struct _icon_button
	{
		static const std::string	strs[3];
		PowerMenu	*parent_;
		PowerAction	action;
		GtkWidget	*grid;
		GtkWidget	*icon;
		GtkWidget	*label;
		_icon_button(PowerAction action, PowerMenu *parent);
		void close(void);
	};

	static void	button_click_cb(GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data);
	void			button_open_modal(PowerMenu::_icon_button *button);

	static const Config power_menu_config_;
	PowerMenu(Overlay &owner, PopupHandler *handler);


protected:
	void on_draw(cairo_t *cr, int width, int height) override;
	bool on_click_pressed(int button, int n_press, double x, double y) override;
	void on_created() override;
	void on_closing() override;

private:



	GtkWidget	*grid;
	struct _icon_button	shutdown_button;
	struct _icon_button	reboot_button;
	struct _icon_button	logout_button;
	GtkAlertDialog		*confirm_modal;
};

struct PowerMenuConfirmCtx
{
	PowerMenu::PowerAction			action;
	PowerMenu::PowerMenuHandler		*handler;
};

