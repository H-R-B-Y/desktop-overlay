#pragma once
#include "Layer.hpp"
#include "Rect.hpp"
#include "helpers.hpp"
#include "Popups.hpp"
#include <memory>
#include "WidgetContainer.hpp"


class TopOverlay : public Layer, public WidgetContainer {
public:
	TopOverlay(Overlay &owner, std::string name, Config config);
	~TopOverlay() override;

protected:
	void	on_frame(int global_tick) override;
	void	on_draw(cairo_t *cr, int width, int height) override;
	void	on_hover_enter(double x, double y) override;
	void	on_hover_move(double x, double y) override;
	void	on_hover_leave() override;
	bool	on_click_pressed(int n_press, double x, double y) override;

private:
	bool 										size_known_;
	Rect 										power_widget_;
	Popup::PopupHandler							power_handler_;
};


class LeftOverlay : public Layer {
public:
	LeftOverlay(Overlay &owner, std::string name, Config config);

	protected:
	void on_draw(cairo_t *cr, int width, int height) override;
	void on_hover_enter(double x, double y) override;
	void on_hover_move(double x, double y) override;
	void on_hover_leave() override;

private:
	Rect widget_bounds_;
	bool widget_hovered_;
};
