#include <gtk/gtk.h>

#include "ui.h"
#include "cal.h"

#define DATASET_FILE "dataset.csv"

typedef struct {
        GtkWidget *entry;

        double a0;
        double a1;
        double a2;

        int polynomial_ready;
} AppData;

static void show_message(GtkWindow *window, const char *message) {
        GtkAlertDialog *dialog;

        dialog = gtk_alert_dialog_new("%s", message);

        gtk_alert_dialog_show(dialog, window);

        g_object_unref(dialog);
}

static void on_submit(GtkButton *button, gpointer data) {
        AppData *app_data = data;

        const char *text;
        char *end;

        double speed;
        double distance;

        GtkWindow *window;
        char message[256];

        window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(button)));

        if (!app_data->polynomial_ready) {
                show_message(window, "can't load dataset.");
                return;
        }

        text = gtk_editable_get_text(GTK_EDITABLE(app_data->entry));
        speed = g_ascii_strtod(text, &end);

        if (text == end || *end != '\0' || speed < 0) {
                show_message(window, "please enter a valid speed.");
                return;
        }

        distance = calculate_distance(speed, app_data->a0, app_data->a1, app_data->a2);
        g_snprintf(message, sizeof(message), "speed: %.2f km/h\nbreaking distance: %.2f m", speed, distance);

        show_message(window, message);

}

void create_ui(GtkApplication *app, gpointer data) {
        (void)data;

        GtkWidget *window;
        GtkWidget *box;
        GtkWidget *entry;
        GtkWidget *button;

        AppData *app_data;

        window = gtk_application_window_new(app);

        gtk_window_set_title(GTK_WINDOW(window), "Finding Break Distance");
        gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768);

        box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

        entry = gtk_entry_new();

        gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter Speed(km/h)");

        gtk_widget_set_size_request(entry, 300, -1);

        button = gtk_button_new_with_label("calculate");

        app_data = g_new0(AppData, 1);
        app_data->entry = entry;
        app_data->polynomial_ready = calculate_polynomial(DATASET_FILE, &app_data->a0, &app_data->a1, &app_data->a2);

        g_signal_connect(button, "clicked", G_CALLBACK(on_submit), app_data);
        g_object_set_data_full(G_OBJECT(window), "app-data", app_data, g_free);
        gtk_box_append(GTK_BOX(box), entry);
        gtk_box_append(GTK_BOX(box), button);
        gtk_window_set_child(GTK_WINDOW(window), box);
        gtk_window_present(GTK_WINDOW(window));
}