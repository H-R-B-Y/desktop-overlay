#include "Widget.hpp"
#include "Popups.hpp"

class	TimeWidget : public LayerWidget
{
public:
	TimeWidget(Overlay &owner, Rect geo);
	~TimeWidget();

	// Determine's the _handler state for hoving the widget
	void	on_hover_enter(double x, double y) override;
	void	on_hover_move(double x, double y) override;
	void	on_hover_leave(void) override;

	// tick to check if the popup should remain open or not
	// this means we also need to write the callback for the G scheduled event
	void	on_tick(void) override;

protected:
	// draw the widget onto the overlay
	void	on_draw(cairo_t *cr, int width, int height) override;

private:
	Popup::PopupHandler			_handler;
	Overlay						&owner;
};

class	PowerWidget : public LayerWidget
{
public:
	PowerWidget(Overlay &owner, Rect geo);
	~PowerWidget();

	void	on_hover_enter(double x, double y) override;
	void	on_hover_move(double x, double y) override;
	void	on_hover_leave(void) override;
	
	void	on_tick(void) override;
protected:
	void	on_draw(cairo_t *cr, int width, int height) override;

private:
	PowerMenu::PowerMenuHandler	_handler;
	Overlay						&owner;
};
