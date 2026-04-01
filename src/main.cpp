#include "Overlay.hpp"

static void on_activate(GtkApplication *app, gpointer user_data)
{
	Overlay *overlay = static_cast<Overlay *>(user_data);
	overlay->activate(app);
}

int main(int argc, char **argv)
{
	Overlay overlay;
	GtkApplication *app = gtk_application_new("com.example.desktopoverlay", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(on_activate), &overlay);

	int status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
	return status;
}