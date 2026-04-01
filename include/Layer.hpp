#ifndef LAYER_HPP
#define LAYER_HPP

#include <array>
#include <functional>
#include <string>
#include <assert.h>
#include <gtk/gtk.h>
#include <gtk4-layer-shell.h>

class Overlay;

class Layer {
public:
	using DrawHandler = std::function<void(cairo_t *, int, int, int)>;

	struct Config {
		std::array<int, 2> size;
		GtkLayerShellLayer layer;
		std::array<int, 4> margins;
		std::array<bool, 4> anchors;
		bool exclusive;
		bool capture_hover;
		bool capture_click;
	};

	Layer(Overlay &owner, std::string name, Config config, DrawHandler draw_handler = {});
	virtual ~Layer() = default;

	void create(GtkApplication *app);
	void queue_redraw();
	void frame(int global_tick);
	void close();

	void resize(std::array<int, 2> size);
	void set_margins(std::array<int, 4> margins);

	const char *name_c_str() const;
	int current_tick() const;

protected:
	virtual void on_frame(int global_tick);
	virtual void on_draw(cairo_t *cr, int width, int height);
	virtual void on_hover_enter(double x, double y);
	virtual void on_hover_move(double x, double y);
	virtual void on_hover_leave();
	virtual bool on_click_pressed(int n_press, double x, double y);
	virtual void on_created();
	virtual void on_closing();
	virtual bool use_drawing_area() const;
	virtual void create_widget_visuals(GtkWidget *root_overlay);
	void add_overlay_child(GtkWidget *child);
	
	Overlay &owner();
	GtkWidget *window_;

private:
	static void draw_cb(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data);
	static void hover_enter_cb(GtkEventControllerMotion *controller, double x, double y, gpointer user_data);
	static void hover_move_cb(GtkEventControllerMotion *controller, double x, double y, gpointer user_data);
	static void hover_leave_cb(GtkEventControllerMotion *controller, gpointer user_data);
	static void click_pressed_cb(GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data);
	
	void attach_shared_handlers();

	Overlay &owner_;
	std::string name_;
	Config config_;
	DrawHandler draw_handler_;
	int current_tick_;
	GtkWidget *root_overlay_;
	GtkWidget *drawing_area_;
};

#endif