#include "WorkspaceWidget.hpp"

WorkspaceWidget::WorkspaceWidget(Overlay &owner, Rect geo, GtkWidget *parent)
	:
	LayerWidget(LayerWidget::WidgetType::DRAWN, geo),
	monitor_name_(owner.get_monitor_name(parent)), // TODO: get monitor name from overlay
	focused_workspace_idx_(0),
	visible_workspace_idx_(0)
{
	refresh_state_from_get_workspaces_();
	sub_client_.set_on_event([this](const std::string &msg) { this->sway_ipc_cb_(msg); });
	sub_client_.start({"workspace", "output"});
}

WorkspaceWidget::~WorkspaceWidget()
{
	// not sure if there is anything to do here
}

void	WorkspaceWidget::on_hover_enter(double x, double y)
{(void)x;(void)y;}

void	WorkspaceWidget::on_hover_leave(void)
{}

void	WorkspaceWidget::on_hover_move(double x, double y)
{(void)x;(void)y;}

void	WorkspaceWidget::on_tick(void) // TODO: shouldnt the widget be passed the global tick?
{}

void	WorkspaceWidget::on_draw(cairo_t *cr, int width, int height)
{
	cairo_set_source_rgb(cr, 1.0, 1.0, 0.2);
	cairo_rectangle(cr, 0, 0, width, height);
	cairo_fill(cr);
}

void	WorkspaceWidget::sway_ipc_cb_(const std::string &msg)
{
	std::cout << msg << std::endl;
}

void	WorkspaceWidget::sway_cmd_cb_(bool res, std::string msg)
{
	(void)res;
	std::cout << msg << std::endl;
}

void	WorkspaceWidget::refresh_state_from_get_workspaces_(void)
{
	SwayCommand	get_workspaces;
	get_workspaces.send_command(
		IpcType::GetWorkspaces,
		"",
		[this](bool ok, std::string msg) { this->sway_cmd_cb_(ok, msg); }
	);
}