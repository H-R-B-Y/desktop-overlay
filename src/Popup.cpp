#include "Popup.hpp"
#include <iostream>
Popup::Popup(Overlay &owner, std::string name, Layer::Config cfg, PopupHandler *handler)
	: Layer(owner, name, cfg), handler(handler)
{
}

Popup::~Popup() = default;

gboolean Popup::PopupHandler::close_popup_cb(gpointer user_data)
{
	Popup::PopupHandler *handler;

	handler = static_cast<Popup::PopupHandler*>(user_data);
	handler->close_popup();
	handler->close_pending = false;
	handler->close_timer_id = 0;
	return G_SOURCE_REMOVE;
}

void Popup::PopupHandler::unschedule_close_popup(void)
{
	if (close_timer_id)
	{
		g_source_remove(close_timer_id);
		close_timer_id = 0;
	}
	close_pending = false;
}

void Popup::PopupHandler::schedule_close_popup(void)
{
	std::cout << "schedule_close_popup called, is_open() = " << is_open() << "\n";
	if (!self)
	{
		std::cout << "  No popup instance (self == nullptr), skipping timer\n";
		close_pending = false;
		return ;
	}
	if (this->close_pending && this->close_timer_id != 0)
		return ;
	this->close_pending = true;
	this->close_timer_id = g_timeout_add(120, close_popup_cb, this);
	std::cout << "  Timer scheduled\n";
}

void Popup::PopupHandler::close_popup(void)
{
	std::cout << "close_popup called\n";
	if (!self) return;
	self->close();
	self.reset();
}

void Popup::on_hover_enter(double x, double y)
{
	on_hover_move(x, y);
}

void Popup::on_hover_move(double x, double y)
{
	if (handler)
		handler->popup_hovered = true;
}

void Popup::on_hover_leave()
{
	if (handler)
		handler->popup_hovered = false;
}
