
#include "Overlay.hpp"
#include "Widget.hpp"

// we also want some atomics because we are working with async
#include <atomic>

class	VolumeWidget : public LayerWidget
{
public:
	VolumeWidget(Overlay &owner, Rect geo);
	~VolumeWidget();

	void	on_hover_enter(double x, double y) override;
	void	on_hover_move(double x, double y) override;
	void	on_hover_leave(void) override;
	void	on_scroll(double dx, double dy) override;
	void	on_tick(void) override;
protected:
	void	on_draw(cairo_t *cr, int width, int height) override;

private:
	static const int	volume_update_interval_ = 100; // how many ticks between volume updates
	static const char	*get_volume_cmd_[];
	static const char	*volume_up_cmd_[];
	static const char	*volume_down_cmd_[];

	// this is the callback for g_subprocess_communicate_utf8_async
	static void	volume_cmd_finished(GObject *source_object, GAsyncResult *res, gpointer user_data);

	Overlay						&owner_;
	std::atomic<float>			volume_{0.0f};
	std::atomic<bool>			volume_request_triggered_{false};
	GSubprocess					*volume_process_ = nullptr;
	int							timeout_ = 0;
};