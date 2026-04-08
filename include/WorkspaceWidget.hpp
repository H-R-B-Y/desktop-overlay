
#include "Overlay.hpp"
#include "Widget.hpp"
#include "SwayIPC.hpp"
#include "simdjson.hpp"
#include <vector>

struct workspace_entry
{
	std::string	name;
	int			num;
	bool		focused;
	bool		visible;
	bool		urgent;
};

class	WorkspaceWidget : public LayerWidget
{
public:
	WorkspaceWidget(Overlay &owner, Rect geo, GtkWidget *parent);
	~WorkspaceWidget();

	void	on_hover_enter(double x, double y) override;
	void	on_hover_move(double x, double y) override;
	void	on_hover_leave(void) override;
	void	on_scroll(double dx, double dy) override;
	void	on_tick(void) override;

	// click and scroll handlers needed
protected:
	void	on_draw(cairo_t *cr, int width, int height) override;

private:

	void	sway_ipc_cb_(const std::string &msg);
	void	sway_cmd_cb_(bool res, std::string msg);
	void	refresh_state_from_get_workspaces_(void);

	std::string						monitor_name_;
	SwayIpcClient					sub_client_;
	std::vector<workspace_entry>	workspaces_;
	int								focused_workspace_idx_;
	int								visible_workspace_idx_;

	bool							widget_hovered_;
};