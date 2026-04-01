
#pragma once
#include "Layer.hpp"
#include <memory>

class Popup : public Layer
{
public:
	struct PopupHandler {
		bool					widget_hovered = false;
		bool					popup_hovered = false;
		bool					close_pending = false;
		guint					close_timer_id = 0;
		std::unique_ptr<Popup>	self = nullptr;

		template <typename T, typename... Args>
		T *ensure_popup(Args&&... args)
		{
			if (!self)
			{
				self = std::make_unique<T>(std::forward<Args>(args)..., this);
			}
			return static_cast<T*>(self.get());
		}

		static gboolean			close_popup_cb(gpointer user_data);
		void					unschedule_close_popup(void);
		void					schedule_close_popup(void);
		void					close_popup(void);

		inline bool				is_open(void){return widget_hovered | popup_hovered;};
	};
	virtual ~Popup() = 0;
	
protected:
	Popup(Overlay &owner, std::string name, Layer::Config cfg, PopupHandler *handler);
	PopupHandler	*handler;
	void on_hover_enter(double x, double y) override;
	void on_hover_move(double x, double y) override;
	void on_hover_leave() override;
	
private:

};