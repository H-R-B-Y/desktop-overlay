#include "Overlay.hpp"
#include "Layers.hpp"
#include <cmath>
#include <utility>

Overlay::Overlay()
	: app_(nullptr),
	  animation_timer_id_(0),
	  tick_(0)
{
}

Overlay::~Overlay()
{
	stop_animation();
}

void Overlay::install_transparent_css()
{
	GtkCssProvider *provider = gtk_css_provider_new();
	gtk_css_provider_load_from_string(provider,
		"window.overlay-window { background-color: transparent; }"
		"window.overlay-window > * { background-color: transparent; }"
		".power-menu-grid > .power-menu-button {"
		"  background-color: rgba(66, 67, 67, 0.78);"
		"  border-radius: 15px;"
		"  border: 1px solid rgba(255, 255, 255, 0.12);"
		"}"
		".power-menu-grid > .power-menu-button > label.power-menu-button-label {"
		"  color: rgb(242, 245, 248);"
		"  font-weight: 600;"
		"}");
	gtk_style_context_add_provider_for_display(
		gdk_display_get_default(),
		GTK_STYLE_PROVIDER(provider),
		GTK_STYLE_PROVIDER_PRIORITY_USER);
	g_object_unref(provider);
}

gboolean Overlay::animation_tick_cb(gpointer user_data)
{
	Overlay *self = static_cast<Overlay *>(user_data);
	self->tick_ = (self->tick_ + 1) % 10000;
	for (const auto &layer : self->layers_) {
		layer->frame(self->tick_);
	}
	self->queue_redraw_all();
	return G_SOURCE_CONTINUE;
}

void Overlay::start_animation()
{
	if (animation_timer_id_ != 0) {
		return;
	}
	animation_timer_id_ = g_timeout_add(16, &Overlay::animation_tick_cb, this);
}

void Overlay::stop_animation()
{
	if (animation_timer_id_ != 0) {
		g_source_remove(animation_timer_id_);
		animation_timer_id_ = 0;
	}
}

void Overlay::queue_redraw_all()
{
	for (const auto &layer : layers_) {
		layer->queue_redraw();
	}
}

int Overlay::tick() const
{
	return tick_;
}

GtkApplication *Overlay::application() const
{
	return app_;
}

GdkRectangle	Overlay::get_monitor_geo(GtkWidget *parent_)
{
	GdkSurface	*surface = gtk_native_get_surface(GTK_NATIVE(parent_));
	if (!surface)
	{
		throw ;
	}
	GdkDisplay *display = gdk_surface_get_display(surface);
	if (!display)
	{
		throw ;
	}
	GdkMonitor *monitor = gdk_display_get_monitor_at_surface(display, surface);
	if (!monitor)
	{
		throw ;
	}
	GdkRectangle	output;
	gdk_monitor_get_geometry(monitor, &output);
	return output;
}

void Overlay::activate(GtkApplication *app)
{
	app_ = app;
	tick_ = 0;
	layers_.clear();
	install_transparent_css();

	Layer::Config layer_a_cfg{
		{30, 30},
		GTK_LAYER_SHELL_LAYER_TOP,
		{0, 0, 0, 0},
		{true, true, true, false},
		true,
		true,
		true
	};
	layers_.push_back(std::make_unique<TopOverlay>(
		*this,
		"Desktop Overlay 1",
		layer_a_cfg));

	Layer::Config layer_b_cfg{
		{30, 30},
		GTK_LAYER_SHELL_LAYER_TOP,
		{0, 0, 0, 0},
		{true, false, true, true},
		true,
		false,
		true
	};
	layers_.push_back(std::make_unique<LeftOverlay>(
		*this,
		"Desktop Overlay 2",
		layer_b_cfg));

	for (const auto &layer : layers_) {
		layer->create(app_);
	}

	start_animation();
}