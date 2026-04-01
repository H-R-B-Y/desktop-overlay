#pragma once

# include <Popup.hpp>


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
	bool on_click_pressed(int n_press, double x, double y) override;
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
	static const Config power_menu_config_;
	PowerMenu(Overlay &owner, Popup::PopupHandler *handler);

protected:
	void on_draw(cairo_t *cr, int width, int height) override;
	bool on_click_pressed(int n_press, double x, double y) override;
	void on_created() override;
	void on_closing() override;

private:
	
};