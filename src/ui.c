#include <gtk/gtk.h>

#include "ui.h"
#include "cal.h"

#define DATASET_FILE "dataset.csv"

#define GRAPH_MIN_X 0.0
#define GRAPH_MAX_X 1000.0
#define GRAPH_STEP 50.0
#define GRAPH_MARGIN 70.0

typedef struct
{
        GtkWidget *entry;

        double a0;
        double a1;
        double a2;

        int polynomial_ready;
} AppData;

typedef struct
{
        double a0;
        double a1;
        double a2;
} GraphData;

static void show_message(GtkWindow *window, const char *message)
{
        GtkAlertDialog *dialog;

        dialog = gtk_alert_dialog_new("%s", message);

        gtk_alert_dialog_show(dialog, window);

        g_object_unref(dialog);
}

static void draw_graph(GtkDrawingArea *area, cairo_t *cr,
                       int width, int height, gpointer data)
{
        (void)area;

        GraphData *graph_data = data;

        double graph_width;
        double graph_height;

        double y_min;
        double y_max;
        double y_range;

        double x_axis_y;

        double x;
        double y;

        double plot_x;
        double plot_y;

        char label[128];

        int y_tick_count = 5;

        if (width <= GRAPH_MARGIN * 2 ||
            height <= GRAPH_MARGIN * 2)
        {
                return;
        }

        /*
         * Find minimum and maximum Y values
         */
        y_min = calculate_distance(
            GRAPH_MIN_X,
            graph_data->a0,
            graph_data->a1,
            graph_data->a2);

        y_max = y_min;

        for (x = GRAPH_MIN_X;
             x <= GRAPH_MAX_X;
             x += GRAPH_STEP)
        {

                y = calculate_distance(
                    x,
                    graph_data->a0,
                    graph_data->a1,
                    graph_data->a2);

                if (y < y_min)
                {
                        y_min = y;
                }

                if (y > y_max)
                {
                        y_max = y;
                }
        }

        /*
         * Include zero on Y axis
         */
        if (y_min > 0.0)
        {
                y_min = 0.0;
        }

        if (y_max < 0.0)
        {
                y_max = 0.0;
        }

        if (y_max == y_min)
        {
                y_max += 1.0;
                y_min -= 1.0;
        }

        /*
         * Add some space above and below graph
         */
        y_range = y_max - y_min;

        y_max += y_range * 0.10;

        if (y_min < 0.0)
        {
                y_min -= y_range * 0.10;
        }

        y_range = y_max - y_min;

        graph_width =
            width - GRAPH_MARGIN * 2;

        graph_height =
            height - GRAPH_MARGIN * 2;

        /*
         * White background
         */
        cairo_set_source_rgb(
            cr,
            1.0,
            1.0,
            1.0);

        cairo_paint(cr);

        /*
         * Axis color
         */
        cairo_set_source_rgb(
            cr,
            0.15,
            0.15,
            0.15);

        cairo_set_line_width(
            cr,
            1.5);

        /*
         * Find Y position of X axis
         */
        x_axis_y =
            GRAPH_MARGIN +
            ((y_max - 0.0) / y_range) * graph_height;

        /*
         * Draw Y axis
         */
        cairo_move_to(
            cr,
            GRAPH_MARGIN,
            GRAPH_MARGIN);

        cairo_line_to(
            cr,
            GRAPH_MARGIN,
            height - GRAPH_MARGIN);

        cairo_stroke(cr);

        /*
         * Draw X axis
         */
        cairo_move_to(
            cr,
            GRAPH_MARGIN,
            x_axis_y);

        cairo_line_to(
            cr,
            width - GRAPH_MARGIN,
            x_axis_y);

        cairo_stroke(cr);

        /*
         * Font settings
         */
        cairo_select_font_face(
            cr,
            "Sans",
            CAIRO_FONT_SLANT_NORMAL,
            CAIRO_FONT_WEIGHT_NORMAL);

        cairo_set_font_size(
            cr,
            12.0);

        /*
         * X axis values
         *
         * Graph points are calculated every 50.
         * Labels are shown every 250 to avoid overlap.
         */
        for (x = GRAPH_MIN_X;
             x <= GRAPH_MAX_X;
             x += 250.0)
        {

                plot_x =
                    GRAPH_MARGIN +
                    ((x - GRAPH_MIN_X) /
                     (GRAPH_MAX_X - GRAPH_MIN_X)) *
                        graph_width;

                /*
                 * Draw X tick
                 */
                cairo_move_to(
                    cr,
                    plot_x,
                    x_axis_y - 5.0);

                cairo_line_to(
                    cr,
                    plot_x,
                    x_axis_y + 5.0);

                cairo_stroke(cr);

                /*
                 * Draw X value
                 */
                g_snprintf(
                    label,
                    sizeof(label),
                    "%.0f",
                    x);

                cairo_move_to(
                    cr,
                    plot_x - 12.0,
                    x_axis_y + 22.0);

                cairo_show_text(
                    cr,
                    label);
        }

        /*
         * Y axis values
         */
        for (int i = 0;
             i <= y_tick_count;
             ++i)
        {

                double tick_value;
                double tick_y;

                tick_value =
                    y_max -
                    ((y_max - y_min) /
                     y_tick_count) *
                        i;

                tick_y =
                    GRAPH_MARGIN +
                    ((double)i /
                     y_tick_count) *
                        graph_height;

                /*
                 * Draw Y tick
                 */
                cairo_move_to(
                    cr,
                    GRAPH_MARGIN - 5.0,
                    tick_y);

                cairo_line_to(
                    cr,
                    GRAPH_MARGIN + 5.0,
                    tick_y);

                cairo_stroke(cr);

                /*
                 * Draw Y value
                 */
                g_snprintf(
                    label,
                    sizeof(label),
                    "%.2f",
                    tick_value);

                cairo_move_to(
                    cr,
                    5.0,
                    tick_y + 4.0);

                cairo_show_text(
                    cr,
                    label);

                /*
                 * Draw horizontal grid line
                 */
                cairo_set_source_rgb(
                    cr,
                    0.85,
                    0.85,
                    0.85);

                cairo_set_line_width(
                    cr,
                    0.5);

                cairo_move_to(
                    cr,
                    GRAPH_MARGIN,
                    tick_y);

                cairo_line_to(
                    cr,
                    width - GRAPH_MARGIN,
                    tick_y);

                cairo_stroke(cr);

                /*
                 * Return to axis color
                 */
                cairo_set_source_rgb(
                    cr,
                    0.15,
                    0.15,
                    0.15);

                cairo_set_line_width(
                    cr,
                    1.5);
        }

        /*
         * Display equation
         */
        g_snprintf(
            label,
            sizeof(label),
            "y = %.4f + %.4fx + %.4fx^2",
            graph_data->a0,
            graph_data->a1,
            graph_data->a2);

        cairo_move_to(
            cr,
            GRAPH_MARGIN,
            30.0);

        cairo_show_text(
            cr,
            label);

        /*
         * Axis labels
         */
        cairo_move_to(
            cr,
            width - GRAPH_MARGIN - 70.0,
            x_axis_y - 10.0);

        cairo_show_text(
            cr,
            "Speed");

        cairo_move_to(
            cr,
            GRAPH_MARGIN + 10.0,
            GRAPH_MARGIN - 15.0);

        cairo_show_text(
            cr,
            "Distance");

        /*
         * Draw graph line
         */
        cairo_set_source_rgb(
            cr,
            0.10,
            0.35,
            0.80);

        cairo_set_line_width(
            cr,
            2.5);

        for (x = GRAPH_MIN_X;
             x <= GRAPH_MAX_X;
             x += GRAPH_STEP)
        {

                y = calculate_distance(
                    x,
                    graph_data->a0,
                    graph_data->a1,
                    graph_data->a2);

                plot_x =
                    GRAPH_MARGIN +
                    ((x - GRAPH_MIN_X) /
                     (GRAPH_MAX_X - GRAPH_MIN_X)) *
                        graph_width;

                plot_y =
                    GRAPH_MARGIN +
                    ((y_max - y) /
                     y_range) *
                        graph_height;

                if (x == GRAPH_MIN_X)
                {
                        cairo_move_to(
                            cr,
                            plot_x,
                            plot_y);
                }
                else
                {
                        cairo_line_to(
                            cr,
                            plot_x,
                            plot_y);
                }
        }

        cairo_stroke(cr);

        /*
         * Draw data point every 50
         */
        cairo_set_source_rgb(
            cr,
            0.80,
            0.15,
            0.15);

        for (x = GRAPH_MIN_X;
             x <= GRAPH_MAX_X;
             x += GRAPH_STEP)
        {

                y = calculate_distance(
                    x,
                    graph_data->a0,
                    graph_data->a1,
                    graph_data->a2);

                plot_x =
                    GRAPH_MARGIN +
                    ((x - GRAPH_MIN_X) /
                     (GRAPH_MAX_X - GRAPH_MIN_X)) *
                        graph_width;

                plot_y =
                    GRAPH_MARGIN +
                    ((y_max - y) /
                     y_range) *
                        graph_height;

                cairo_arc(
                    cr,
                    plot_x,
                    plot_y,
                    3.5,
                    0.0,
                    2.0 * G_PI);

                cairo_fill(cr);
        }
}

static void on_submit(GtkButton *button, gpointer data)
{
        AppData *app_data = data;

        const char *text;

        char *end;

        double speed;
        double distance;

        GtkWindow *window;

        char message[256];

        window = GTK_WINDOW(
            gtk_widget_get_root(
                GTK_WIDGET(button)));

        if (!app_data->polynomial_ready)
        {
                show_message(
                    window,
                    "can't load dataset.");

                return;
        }

        text = gtk_editable_get_text(
            GTK_EDITABLE(
                app_data->entry));

        speed = g_ascii_strtod(
            text,
            &end);

        if (text == end ||
            *end != '\0' ||
            speed < 0)
        {

                show_message(
                    window,
                    "please enter a valid speed.");

                return;
        }

        distance = calculate_distance(
            speed,
            app_data->a0,
            app_data->a1,
            app_data->a2);

        g_snprintf(
            message,
            sizeof(message),
            "speed: %.2f km/h\n"
            "breaking distance: %.2f m",
            speed,
            distance);

        show_message(
            window,
            message);
}

static void on_graph(GtkButton *button, gpointer data)
{
        AppData *app_data = data;

        GtkWindow *window;

        GtkApplication *application;

        GtkWidget *graph_window;
        GtkWidget *drawing_area;

        GraphData *graph_data;

        window = GTK_WINDOW(
            gtk_widget_get_root(
                GTK_WIDGET(button)));

        if (!app_data->polynomial_ready)
        {
                show_message(
                    window,
                    "can't load dataset.");

                return;
        }

        application =
            gtk_window_get_application(
                window);

        graph_window =
            gtk_application_window_new(
                application);

        gtk_window_set_title(
            GTK_WINDOW(graph_window),
            "Equation Graph");

        gtk_window_set_default_size(
            GTK_WINDOW(graph_window),
            1024,
            768);

        gtk_window_set_transient_for(
            GTK_WINDOW(graph_window),
            window);

        drawing_area =
            gtk_drawing_area_new();

        gtk_drawing_area_set_content_width(
            GTK_DRAWING_AREA(
                drawing_area),
            900);

        gtk_drawing_area_set_content_height(
            GTK_DRAWING_AREA(
                drawing_area),
            650);

        graph_data =
            g_new(
                GraphData,
                1);

        graph_data->a0 =
            app_data->a0;

        graph_data->a1 =
            app_data->a1;

        graph_data->a2 =
            app_data->a2;

        gtk_drawing_area_set_draw_func(
            GTK_DRAWING_AREA(
                drawing_area),
            draw_graph,
            graph_data,
            g_free);

        gtk_window_set_child(
            GTK_WINDOW(graph_window),
            drawing_area);

        gtk_window_present(
            GTK_WINDOW(graph_window));
}

void create_ui(GtkApplication *app, gpointer data)
{
        (void)data;

        GtkWidget *window;

        GtkWidget *box;

        GtkWidget *entry;

        GtkWidget *button;
        GtkWidget *graph_button;

        AppData *app_data;

        window =
            gtk_application_window_new(
                app);

        gtk_window_set_title(
            GTK_WINDOW(window),
            "Finding Break Distance");

        gtk_window_set_default_size(
            GTK_WINDOW(window),
            1024,
            768);

        box =
            gtk_box_new(
                GTK_ORIENTATION_VERTICAL,
                10);

        gtk_widget_set_halign(
            box,
            GTK_ALIGN_CENTER);

        gtk_widget_set_valign(
            box,
            GTK_ALIGN_CENTER);

        entry =
            gtk_entry_new();

        gtk_entry_set_placeholder_text(
            GTK_ENTRY(entry),
            "Enter Speed(km/h)");

        gtk_widget_set_size_request(
            entry,
            300,
            -1);

        button =
            gtk_button_new_with_label(
                "calculate");

        graph_button =
            gtk_button_new_with_label(
                "Graph");

        app_data =
            g_new0(
                AppData,
                1);

        app_data->entry =
            entry;

        app_data->polynomial_ready =
            calculate_polynomial(
                DATASET_FILE,
                &app_data->a0,
                &app_data->a1,
                &app_data->a2);

        g_signal_connect(
            button,
            "clicked",
            G_CALLBACK(on_submit),
            app_data);

        g_signal_connect(
            graph_button,
            "clicked",
            G_CALLBACK(on_graph),
            app_data);

        g_object_set_data_full(
            G_OBJECT(window),
            "app-data",
            app_data,
            g_free);

        gtk_box_append(
            GTK_BOX(box),
            entry);

        gtk_box_append(
            GTK_BOX(box),
            button);

        gtk_box_append(
            GTK_BOX(box),
            graph_button);

        gtk_window_set_child(
            GTK_WINDOW(window),
            box);

        gtk_window_present(
            GTK_WINDOW(window));
}