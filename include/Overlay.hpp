#ifndef OVERLAY_HPP
#define OVERLAY_HPP

#include <memory>
#include <vector>
# include "helpers.hpp"
#include <gtk/gtk.h>

class Layer;

class Overlay {
public:
	Overlay();
	~Overlay();

	void activate(GtkApplication *app);
	int tick() const;
	GtkApplication *application() const;
	void queue_redraw_all();
	GdkRectangle	get_monitor_geo(GtkWidget *parent_);
	std::string		get_monitor_name(GtkWidget *parent_);

private:
	void install_transparent_css();
	void start_animation();
	void stop_animation();

	static gboolean animation_tick_cb(gpointer user_data);

	GtkApplication *app_;
	std::vector<std::unique_ptr<Layer>> layers_;
	guint animation_timer_id_;
	int tick_;
};

#endif