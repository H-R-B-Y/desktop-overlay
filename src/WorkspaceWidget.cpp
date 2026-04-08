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
{this->on_hover_move(x, y);}

void	WorkspaceWidget::on_hover_leave(void)
{widget_hovered_ = false;}

void	WorkspaceWidget::on_hover_move(double x, double y)
{
	widget_hovered_ = true;
}

void	WorkspaceWidget::on_tick(void) // TODO: shouldnt the widget be passed the global tick?
{}

void	empty_cb_fn(bool b, std::string s)
{
	(void)b;
	(void)s;
}
void	WorkspaceWidget::on_scroll(double dx, double dy)
{
	SwayCommand	cmd;
	// std::cout << "X axis: " << dx << "\nY axis: " << dy << std::endl;
	if (dy > 0)
		cmd.send_command(IpcType::Command, "workspace next_on_output", empty_cb_fn);
	else
		cmd.send_command(IpcType::Command, "workspace prev_on_output", empty_cb_fn);
}


void	WorkspaceWidget::on_draw(cairo_t *cr, int width, int height)
{
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, widget_hovered_ ? 0.30 : 0.25);
	// cairo_rectangle(cr, 0, 0, width, height);
	cairo_rounded_rect(cr, 0 + 4, 0, width - 8, height, 15, RoundedCorner::ALL);
	cairo_fill(cr);

	if (workspaces_.empty())
		return ;

	const int y_margin = 15;
	const int x_pos = width / 2;
	const int y_padding = 4;
	const int radius = 5;
	const int step = (radius * 2) + y_padding;
	const int available_height = height - (y_margin * 2);
	const int max_displayable = (step > 0) ? (available_height / (step)) : 0;

	if (max_displayable <= 0)
		return ;

	const int total_workspaces = static_cast<int>(workspaces_.size());
	const int display_count = (total_workspaces < max_displayable) ? total_workspaces : max_displayable;
	const int used_height = (display_count * (radius * 2)) + ((display_count - 1) * y_padding);
	const int y_start = y_margin + ((available_height - used_height) / 2) + radius;

	int visible_idx = visible_workspace_idx_;
	if (visible_idx < 0 || visible_idx >= total_workspaces)
		visible_idx = (focused_workspace_idx_ >= 0 && focused_workspace_idx_ < total_workspaces) ? focused_workspace_idx_ : 0;

	int ws_start = 0;
	int ws_count = display_count;
	bool show_top_indicator = false;
	bool show_bottom_indicator = false;
	if (total_workspaces > display_count)
	{
		if (display_count >= 3)
		{
			show_top_indicator = true;
			show_bottom_indicator = true;
			ws_count = display_count - 2;
		}
		else
		{
			ws_count = display_count - 1;
			if (ws_count < 1)
				ws_count = 1;
		}

		ws_start = visible_idx - (ws_count / 2);
		if (ws_start < 0)
			ws_start = 0;
		if (ws_start > total_workspaces - ws_count)
			ws_start = total_workspaces - ws_count;

		if (display_count < 3)
		{
			const int hidden_above_small = ws_start;
			const int hidden_below_small = total_workspaces - (ws_start + ws_count);
			if (hidden_above_small > 0 && hidden_below_small == 0)
				show_top_indicator = true;
			else if (hidden_below_small > 0 && hidden_above_small == 0)
				show_bottom_indicator = true;
			else if (hidden_above_small > 0 && hidden_below_small > 0)
				show_bottom_indicator = true;
		}
	}

	const int hidden_above = ws_start;
	const int hidden_below = total_workspaces - (ws_start + ws_count);

	for (int slot = 0; slot < display_count; ++slot)
	{
		const int cy = y_start + (slot * step);

		if (show_top_indicator && slot == 0 && hidden_above > 0)
		{
			// cairo_set_source_rgba(cr, 0.15, 0.15, 0.15, 0.95);
			// cairo_arc(cr, x_pos, cy, radius, 0.0, 6.283185307179586);
			// cairo_fill(cr);

			cairo_set_source_rgba(cr, 0.15, 0.15, 0.15, 1.0);
			cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
			cairo_set_font_size(cr, 10.0);
			std::string count = std::to_string(hidden_above);
			cairo_text_extents_t ext;
			cairo_text_extents(cr, count.c_str(), &ext);
			cairo_move_to(cr, x_pos - (ext.width / 2.0) - ext.x_bearing, cy - (ext.height / 2.0) - ext.y_bearing);
			cairo_show_text(cr, count.c_str());
			continue ;
		}

		if (show_bottom_indicator && slot == display_count - 1 && hidden_below > 0)
		{
			// cairo_set_source_rgba(cr, 0.15, 0.15, 0.15, 0.95);
			// cairo_arc(cr, x_pos, cy, radius, 0.0, 6.283185307179586);
			// cairo_fill(cr);

			cairo_set_source_rgba(cr, 0.15, 0.15, 0.15, 1.0);
			cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
			cairo_set_font_size(cr, 10.0);
			std::string count = std::to_string(hidden_below);
			cairo_text_extents_t ext;
			cairo_text_extents(cr, count.c_str(), &ext);
			cairo_move_to(cr, x_pos - (ext.width / 2.0) - ext.x_bearing, cy - (ext.height / 2.0) - ext.y_bearing);
			cairo_show_text(cr, count.c_str());
			continue ;
		}

		const int ws_slot = slot - (show_top_indicator ? 1 : 0);
		const int ws_idx = ws_start + ws_slot;
		if (ws_idx < 0 || ws_idx >= total_workspaces)
			continue ;

		std::array<float, 3> inactive = {0,0,0};
		std::array<float, 3> active = colours(1);
		std::array<float, 3> urgent = colours(3);

		const workspace_entry &ws = workspaces_[ws_idx];
		if (ws.urgent)
			cairo_set_source_rgba(cr, urgent[0], urgent[1], urgent[2], 1.0);
		else if (ws.focused)
			cairo_set_source_rgba(cr, active[0], active[1], active[2], 1.0);
		else if (ws.visible)
			cairo_set_source_rgba(cr, active[0], active[1], active[2], 0.75);
		else
			cairo_set_source_rgba(cr, inactive[0], inactive[1], inactive[2], 0.2);

		cairo_arc(cr, x_pos, cy, radius, 0.0, 6.283185307179586);
		cairo_fill(cr);
	}
}

void	WorkspaceWidget::sway_ipc_cb_(const std::string &msg)
{
	// std::cout << msg << std::endl;
	this->refresh_state_from_get_workspaces_();
}

void	WorkspaceWidget::sway_cmd_cb_(bool res, std::string msg)
{
	if (!res)
	{
		return ;
	}
	(void)res;
	simdjson::ondemand::parser parser;
	simdjson::padded_string str = simdjson::padded_string(msg);
	simdjson::ondemand::document doc = parser.iterate(str);
	// std::cout << msg << std::endl;
	workspaces_.clear();
	auto workspaces = doc.get_array();
	for (auto ws_val : workspaces) {
		auto ws = ws_val.get_object();
		std::string_view name = ws["name"];
		std::string_view display = ws["output"];
		int num = ws["id"].get_int64();
		bool focused = ws["focused"].get_bool();
		bool visible = ws["visible"].get_bool();
		bool urgent = ws["urgent"].get_bool();
		if (!display.compare(monitor_name_))
		{
			workspaces_.push_back(workspace_entry{
				std::string(name),
				num,
				focused,
				visible,
				urgent
			});
		}
		if (visible)
			visible_workspace_idx_ = workspaces_.size() - 1;
		if (focused)
			focused_workspace_idx_ = workspaces_.size() - 1;
	}
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
