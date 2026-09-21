#include <gtk/gtk.h>

#include "ui.h"

void create_ui(GtkApplication *app, gpointer data) {
        (void)data;
        GtkWidget *window;

        window = gtk_application_window_new(app);

        gtk_window_set_title(GTK_WINDOW(window), "Finding Break Distance");

        gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768);

        gtk_window_present(GTK_WINDOW(window));
}