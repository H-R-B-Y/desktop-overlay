#include "Layer.hpp"

#include <utility>

#include "Overlay.hpp"

Layer::Layer(Overlay &owner, std::string name, Config config, DrawHandler draw_handler)
	:
	window_(nullptr),
	owner_(owner),
	name_(std::move(name)),
	config_(config),
	draw_handler_(std::move(draw_handler)),
	current_tick_(0),
	root_overlay_(nullptr),
	drawing_area_(nullptr)
{
}

void Layer::create(GtkApplication *app)
{
	window_ = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window_), name_.c_str());
	gtk_window_set_default_size(GTK_WINDOW(window_), config_.size[0], config_.size[1]);
	gtk_widget_add_css_class(window_, "overlay-window");

	gtk_layer_init_for_window(GTK_WINDOW(window_));
	gtk_layer_set_layer(GTK_WINDOW(window_), config_.layer);

	constexpr GtkLayerShellEdge edges[4] = {
		GTK_LAYER_SHELL_EDGE_LEFT,
		GTK_LAYER_SHELL_EDGE_RIGHT,
		GTK_LAYER_SHELL_EDGE_TOP,
		GTK_LAYER_SHELL_EDGE_BOTTOM
	};

	for (int i = 0; i < 4; ++i) {
		gtk_layer_set_anchor(GTK_WINDOW(window_), edges[i], config_.anchors[i]);
		gtk_layer_set_margin(GTK_WINDOW(window_), edges[i], config_.margins[i]);
	}

	gtk_layer_set_keyboard_mode(GTK_WINDOW(window_), GTK_LAYER_SHELL_KEYBOARD_MODE_NONE);
	if (config_.exclusive) {
		gtk_layer_auto_exclusive_zone_enable(GTK_WINDOW(window_));
	} else {
		gtk_layer_set_exclusive_zone(GTK_WINDOW(window_), 0);
	}

	root_overlay_ = gtk_overlay_new();
	gtk_widget_set_hexpand(root_overlay_, TRUE);
	gtk_widget_set_vexpand(root_overlay_, TRUE);
	gtk_window_set_child(GTK_WINDOW(window_), root_overlay_);

	if (use_drawing_area())
	{
		drawing_area_ = gtk_drawing_area_new();
		gtk_widget_set_hexpand(drawing_area_, TRUE);
		gtk_widget_set_vexpand(drawing_area_, TRUE);
		gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area_), &Layer::draw_cb, this, nullptr);
		gtk_overlay_set_child(GTK_OVERLAY(root_overlay_), drawing_area_);
	}
	create_widget_visuals(root_overlay_);
	attach_shared_handlers();
	gtk_window_present(GTK_WINDOW(window_));
	on_created();
}

void Layer::queue_redraw()
{
	if (drawing_area_ != nullptr) {
		gtk_widget_queue_draw(drawing_area_);
	}
}

void Layer::frame(int global_tick)
{
	current_tick_ = global_tick;
	on_frame(global_tick);
}

const char *Layer::name_c_str() const
{
	return name_.c_str();
}

int Layer::current_tick() const
{
	return current_tick_;
}

void Layer::on_frame(int global_tick)
{
	(void)global_tick;
}

void Layer::on_hover_enter(double x, double y)
{
	(void)x;
	(void)y;
}

void Layer::on_hover_move(double x, double y)
{
	(void)x;
	(void)y;
}

void Layer::on_hover_leave()
{
}

bool Layer::on_click_pressed(int button, int n_press, double x, double y)
{
	(void)button;
	(void)n_press;
	(void)x;
	(void)y;
	return FALSE;
}

Overlay &Layer::owner()
{
	return owner_;
}

void Layer::draw_cb(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data)
{
	(void)area;
	Layer *self = static_cast<Layer *>(user_data);
	self->on_draw(cr, width, height);
}

void Layer::hover_enter_cb(GtkEventControllerMotion *controller, double x, double y, gpointer user_data)
{
	(void)controller;
	Layer *self = static_cast<Layer *>(user_data);
	self->on_hover_enter(x, y);
}

void Layer::hover_move_cb(GtkEventControllerMotion *controller, double x, double y, gpointer user_data)
{
	(void)controller;
	Layer *self = static_cast<Layer *>(user_data);
	self->on_hover_move(x, y);
}

void Layer::hover_leave_cb(GtkEventControllerMotion *controller, gpointer user_data)
{
	(void)controller;
	Layer *self = static_cast<Layer *>(user_data);
	self->on_hover_leave();
}

void Layer::click_pressed_cb(GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data)
{
	(void)gesture;
	Layer *self = static_cast<Layer *>(user_data);
	int button = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));
	if (self->on_click_pressed(button, n_press, x, y))
		gtk_gesture_set_state(GTK_GESTURE(gesture), GTK_EVENT_SEQUENCE_CLAIMED);
}

void	Layer::scroll_event_cb(GtkEventControllerScroll *controller, double dx, double dy, gpointer user_data)
{
	(void)controller;
	Layer *self = static_cast<Layer *>(user_data);
	self->on_scroll(dx, dy);
}

void Layer::attach_shared_handlers()
{
	if (config_.capture_hover) {
		GtkEventController *motion = gtk_event_controller_motion_new();
		gtk_event_controller_set_propagation_phase(motion, GtkPropagationPhase::GTK_PHASE_CAPTURE);
		g_signal_connect(motion, "enter", G_CALLBACK(&Layer::hover_enter_cb), this);
		g_signal_connect(motion, "motion", G_CALLBACK(&Layer::hover_move_cb), this);
		g_signal_connect(motion, "leave", G_CALLBACK(&Layer::hover_leave_cb), this);
		gtk_widget_add_controller(root_overlay_, motion);
	}

	if (config_.capture_click) {
		GtkGesture *click = gtk_gesture_click_new();
		gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(click), GtkPropagationPhase::GTK_PHASE_CAPTURE);
		g_signal_connect(click, "pressed", G_CALLBACK(&Layer::click_pressed_cb), this);
		gtk_widget_add_controller(root_overlay_, GTK_EVENT_CONTROLLER(click));
	}

	if (config_.capture_scroll)
	{
		GtkEventController *scroll = gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES);
		gtk_event_controller_set_propagation_phase(scroll, GtkPropagationPhase::GTK_PHASE_CAPTURE);
		g_signal_connect(scroll, "scroll", G_CALLBACK(scroll_event_cb), this);
		gtk_widget_add_controller(root_overlay_, GTK_EVENT_CONTROLLER(scroll));
	}
}

void Layer::resize(std::array<int, 2> size)
{
	gtk_window_set_default_size(GTK_WINDOW(this->window_), size[0], size[1]);
}

void Layer::set_margins(std::array<int, 4> margins)
{
	config_.margins = margins;
	if (window_ == nullptr) {
		return;
	}

	constexpr GtkLayerShellEdge edges[4] = {
		GTK_LAYER_SHELL_EDGE_LEFT,
		GTK_LAYER_SHELL_EDGE_RIGHT,
		GTK_LAYER_SHELL_EDGE_TOP,
		GTK_LAYER_SHELL_EDGE_BOTTOM
	};

	for (int i = 0; i < 4; ++i) {
		gtk_layer_set_margin(GTK_WINDOW(window_), edges[i], config_.margins[i]);
	}
}

void Layer::close()
{
	if (window_ != nullptr) {
		gtk_window_destroy(GTK_WINDOW(window_));
		on_closing();
		window_ = nullptr;
		drawing_area_ = nullptr;
		root_overlay_ = nullptr;
	}
}

void	Layer::on_scroll(double dx, double dy)
{
	(void)dx;(void)dy;
}

void Layer::on_draw(cairo_t *cr, int width, int height)
{
	if (draw_handler_) {
		draw_handler_(cr, width, height, current_tick_);
	}
}

void Layer::on_created(){};
void Layer::on_closing(){};
bool Layer::use_drawing_area() const {return true;};
void Layer::create_widget_visuals(GtkWidget *root_overlay){(void)root_overlay;};

void Layer::add_overlay_child(GtkWidget *child)
{
	assert(root_overlay_);
	gtk_overlay_add_overlay(GTK_OVERLAY(root_overlay_), child);
};

