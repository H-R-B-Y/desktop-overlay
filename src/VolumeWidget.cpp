

#include "VolumeWidget.hpp"
#include <iostream>

const char *VolumeWidget::get_volume_cmd_[] = {
	"wpctl", "get-volume", "@DEFAULT_AUDIO_SINK@", nullptr
};

const char *VolumeWidget::volume_up_cmd_[] = {
	"wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@", "1%+", nullptr
};

const char *VolumeWidget::volume_down_cmd_[] = {
	"wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@", "1%-", nullptr
};

VolumeWidget::VolumeWidget(Overlay &owner, Rect geo)
	:
	LayerWidget(LayerWidget::WidgetType::DRAWN, geo),
	owner_(owner)
{
}

VolumeWidget::~VolumeWidget()
{
	if (volume_process_)
	{
		g_subprocess_force_exit(volume_process_);
		g_clear_object(&volume_process_);
	}
}

void	VolumeWidget::on_hover_enter(double x, double y)
{
	on_hover_move(x, y);
}

void	VolumeWidget::on_hover_move(double x, double y)
{
}

void	VolumeWidget::on_hover_leave(void)
{
}

void	VolumeWidget::on_scroll(double dx, double dy)
{
	if (dy < 0)
	{
		GError *err = nullptr;
		GSubprocess *proc = g_subprocess_newv(
			const_cast<char **>(volume_up_cmd_),
			G_SUBPROCESS_FLAGS_NONE,
			&err);
		if (err)
		{
			g_warning("Failed to start volume up subprocess: %s.", err->message);
			g_error_free(err);
			return;
		}
		g_clear_object(&proc);
		volume_ = std::min(1.0f, volume_ + 0.01f);
	}
	else
	{
		GError *err = nullptr;
		GSubprocess *proc = g_subprocess_newv(
			const_cast<char **>(volume_down_cmd_),
			G_SUBPROCESS_FLAGS_NONE,
			&err);
		if (err)
		{
			g_warning("Failed to start volume down subprocess: %s.", err->message);
			g_error_free(err);
			return;
		}
		g_clear_object(&proc);
		volume_ = std::max(0.0f, volume_ - 0.01f);
	}
}

void	VolumeWidget::on_tick(void)
{
	timeout_--;
	if (timeout_ <= 0)
	{
		if (!volume_request_triggered_)
		{
			// std::cout << "Triggering volume update..." << std::endl;
			// how do we set atomic values? just like this? 
			volume_request_triggered_ = true;
			GError *err = nullptr;

			volume_process_ = g_subprocess_newv(
				const_cast<char **>(get_volume_cmd_),
				G_SUBPROCESS_FLAGS_STDOUT_PIPE,
				&err);
			if (err)
			{
				g_warning("Failed to start volume subprocess: %s.", err->message);
				g_error_free(err);
				volume_request_triggered_ = false;
				volume_process_ = nullptr;
				return;
			}
			g_subprocess_communicate_utf8_async(
				volume_process_,
				nullptr,
				nullptr,
				volume_cmd_finished,
				this
			);
			timeout_ = volume_update_interval_;
		}
	}
}

void	VolumeWidget::on_draw(cairo_t *cr, int width, int height)
{
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.25);
	cairo_rounded_rect(cr, 0 + 4, 0, geo.w - 8, geo.h, 15, RoundedCorner::ALL);
	cairo_fill(cr);

	// // draw the volume text
	// std::string vol_text = std::to_string(static_cast<int>(volume_ * 100)) + "%";
	// cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
	// draw_centered_text(cr, vol_text.c_str(), 0, 0, geo.w, geo.h,
	// 	"Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD, 13.0);

	const int y_margin = 10;
	const int bar_width = 10; // idk
	const int bar_height = height - (y_margin * 2);
	
	cairo_set_source_rgba(cr, 0, 0, 0, 0.25);
	cairo_rounded_rect(cr, (width - bar_width) / 2.0, y_margin, bar_width, bar_height, 10, RoundedCorner::ALL);
	cairo_fill(cr);
	std::array<float, 3> col = colours(1);
	cairo_set_source_rgba(cr,col[0], col[1], col[2], 0.95);
	cairo_rounded_rect(cr, (width - bar_width) / 2.0, y_margin + bar_height * (1.0f - volume_), bar_width, bar_height * volume_, 10, RoundedCorner::ALL);
	cairo_fill(cr);
}

void	VolumeWidget::volume_cmd_finished(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
	auto *self = static_cast<VolumeWidget *>(user_data);
	GError *err = nullptr;
	gchar *stdout_buf = nullptr;
	gsize stdout_size = 0;

	if (!g_subprocess_communicate_utf8_finish(self->volume_process_, res, &stdout_buf, nullptr, &err))
	{
		g_warning("Failed to read volume subprocess output: %s.", err->message);
		g_error_free(err);
		self->volume_request_triggered_ = false;
		g_clear_object(&self->volume_process_);
		return;
	}

	if (stdout_buf)
	{
		try
		{
			// std::cout << "Volume subprocess output: " << stdout_buf << std::endl;
			// index of first colon
			char *colon_pos = strchr(stdout_buf, ':');
			if (!colon_pos)
			{
				throw std::runtime_error("Unexpected volume output format");
			}
			// move past colon and any whitespace
			char *vol_str = colon_pos + 1;
			while (*vol_str && isspace(*vol_str))
				vol_str++;
			float vol = std::stof(vol_str);
			if (vol < 0.0f || vol > 1.0f)
			{
				throw std::runtime_error("Volume value out of range");
			}
			self->volume_ = vol;
		}
		catch (const std::exception &e)
		{
			g_warning("Failed to parse volume output: %s.", e.what());
			self->volume_ = 0.0f;
		}
		g_free(stdout_buf);
	}
	else
	{
		g_warning("Volume subprocess produced no output.");
		self->volume_ = 0.0f;
	}

	g_clear_object(&self->volume_process_);
	self->volume_process_ = nullptr;
	self->volume_request_triggered_ = false;
}


