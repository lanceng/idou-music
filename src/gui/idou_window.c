#include <gst/gst.h>

#include "idou_window.h"
#include "idou_titlebar2.h"
#include "idou_window.h"
#include "idou_button.h"
#include "idou_music_manager.h"
#include "gstplay.h"

G_DEFINE_TYPE(iDouWindow, idou_window, GTK_TYPE_WINDOW);

static gpointer on_draw(GtkWidget *widget, gpointer data);
static void on_play_event(GtkWidget *widget, gpointer data);
static void on_time_scale_change_value(GtkWidget *widget, gpointer data);
static gboolean timer_cb(gpointer data);
static void draw_rounded_rectangle(cairo_t *cr, gint width, gint height, gint r);

void idou_window_class_init(iDouWindowClass *klass)
{
}

void idou_window_init(iDouWindow *self)
{
    GtkWidget *window;
    GtkWidget *hbox, *hbox2, *hbox3, *hbox4, *hbox5;
    GtkWidget *vbox, *vbox2, *vbox3;
    GtkWidget *image;
    GdkPixbuf *pixbuf;
    
    window = GTK_WIDGET(self);
    gtk_window_set_default_size(GTK_WINDOW(window), 720, 580);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_widget_set_name(window, "iDouWindow");
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);

    vbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    
    vbox3 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    hbox3 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    hbox4 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    hbox5 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);

    GtkWidget *title_bar = idou_titlebar2_new();
    gtk_box_pack_start(GTK_BOX(vbox), title_bar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), vbox2, FALSE, FALSE, 2);
    
    self->music_name = gtk_label_new(NULL);
    self->time_label = gtk_label_new("00:00/00:00");
    gtk_box_pack_start(GTK_BOX(hbox3), self->music_name, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(hbox3), self->time_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox3), hbox3, FALSE, FALSE, 0);

    self->progress_adjust = gtk_adjustment_new(0, 0, 100, 1, 0, 0);
    GtkWidget *time_scale = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, self->progress_adjust);
    gtk_scale_set_draw_value(GTK_SCALE(time_scale), FALSE);
    g_signal_connect(G_OBJECT(time_scale), "value-changed", G_CALLBACK(on_time_scale_change_value), NULL);

    gtk_box_pack_start(GTK_BOX(vbox3), time_scale, FALSE, FALSE, 0);

    GtkWidget *lyric_toggle = gtk_check_button_new_with_label("歌词");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lyric_toggle), TRUE);
    gtk_box_pack_start(GTK_BOX(hbox4), lyric_toggle, FALSE, FALSE, 0);

    GtkWidget *align = gtk_alignment_new(0.3, 0, 0, 0);
    GtkWidget *last_btn = idou_button_new();
    IDOU_BUTTON_SET(last_btn,
                    RESDIR"image/button/last_normal.png",
                    RESDIR"image/button/last_hover.png",
                    RESDIR"image/button/last_press.png",
                    32, 32);
    GtkWidget *play_btn = idou_button_new();
    IDOU_BUTTON_SET(play_btn,
                    RESDIR"image/button/play_normal.png",
                    RESDIR"image/button/play_hover.png",
                    RESDIR"image/button/play_press.png",
                    32, 32);
    GtkWidget *next_btn = idou_button_new();
    IDOU_BUTTON_SET(next_btn,
                    RESDIR"image/button/next_normal.png",
                    RESDIR"image/button/next_hover.png",
                    RESDIR"image/button/next_press.png",
                    32, 32);
    
    g_signal_connect(G_OBJECT(play_btn), "clicked", G_CALLBACK(on_play_event), (gpointer)self);

    gtk_box_pack_start(GTK_BOX(hbox2), last_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox2), play_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox2), next_btn, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(align), hbox2);
    gtk_box_pack_start(GTK_BOX(hbox4), align, TRUE, FALSE, 0);

    self->volume_adjust = gtk_adjustment_new(0, 0, 100, 1, 0, 0);
    GtkWidget *volume_scale = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, self->volume_adjust);
    gtk_range_set_value(GTK_RANGE(volume_scale), 50);
    gtk_widget_set_size_request(volume_scale, 60, 10);
    gtk_scale_set_draw_value(GTK_SCALE(volume_scale), FALSE);
    gtk_box_pack_end(GTK_BOX(hbox4), volume_scale, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox3), hbox4, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox2), vbox3, FALSE, FALSE, 3);

    GtkWidget *music_manager = idou_music_manager_new();
    gtk_box_pack_start(GTK_BOX(vbox2), music_manager, TRUE, TRUE, 2);

    GtkWidget *order_btn = gtk_button_new();
    IDOU_BUTTON_SET(order_btn,
                    RESDIR"image/playmode/order_normal.png",
                    RESDIR"image/playmode/order_hover.png",
                    RESDIR"image/playmode/order_press.png",
                    18, 14);
    GtkWidget *loop_btn = gtk_button_new();
    IDOU_BUTTON_SET(loop_btn,
                    RESDIR"image/playmode/loop_normal.png",
                    RESDIR"image/playmode/loop_hover.png",
                    RESDIR"image/playmode/loop_press.png",
                    18, 14);
    GtkWidget *random_btn = gtk_button_new();
    IDOU_BUTTON_SET(random_btn,
                    RESDIR"image/playmode/random_normal.png",
                    RESDIR"image/playmode/random_hover.png",
                    RESDIR"image/playmode/random_press.png",
                    18, 14);
    GtkWidget *single_btn = gtk_button_new();
    IDOU_BUTTON_SET(single_btn,
                    RESDIR"image/playmode/single_normal.png",
                    RESDIR"image/playmode/single_hover.png",
                    RESDIR"image/playmode/single_press.png",
                    18, 14);

    gtk_box_pack_start(GTK_BOX(hbox5), order_btn, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox5), loop_btn, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox5), random_btn, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox5), single_btn, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox2), hbox5, FALSE, FALSE, 3);

    gtk_container_add(GTK_CONTAINER(window), vbox);

    g_signal_connect(G_OBJECT(title_bar), "idou-destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect_after(G_OBJECT(window), "draw", G_CALLBACK(on_draw), NULL);

    GtkCssProvider *provider = gtk_css_provider_new();
    GdkScreen *screen = gdk_screen_get_default();

    const gchar *css_data = "#iDouWindow {"
                            "background-image: url('"
                            RESDIR"image/skin/blueSky.jpg');"
                            "background-size: 100% 100%;"
                            "}";
    gtk_css_provider_load_from_data(provider, css_data, -1, NULL);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);

    gtk_widget_show_all(window);
}

GtkWidget *idou_window_new()
{
    g_object_new(IDOU_TYPE_WINDOW, NULL);
}

void idou_window_show(iDouWindow *window)
{
    gtk_widget_show_all(GTK_WINDOW(window));
}

static gpointer on_draw(GtkWidget *widget, gpointer data)
{
    cairo_t *cr;
    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);
    gint width = alloc.width;
    gint height = alloc.height;
    gint r = 5;

    cr = gdk_cairo_create(gtk_widget_get_window(widget));
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                          width,
                                                          height);
    cairo_t *cr2 = cairo_create(surface);
    draw_rounded_rectangle(cr2, width, height, r);
    cairo_stroke_preserve(cr2);
    cairo_fill(cr2) ;
    cairo_destroy(cr2);

    cairo_region_t *region = gdk_cairo_region_create_from_surface(surface);
    gtk_widget_shape_combine_region(widget, region);

    cairo_set_line_width(cr, 0.3);
    cairo_set_source_rgb(cr, 0.78, 0.78, 0.78);
    draw_rounded_rectangle(cr, width, height, r);
    cairo_stroke(cr);

    cairo_region_destroy(region);
    cairo_surface_destroy(surface);
    cairo_destroy(cr);
}

static void draw_rounded_rectangle(cairo_t *cr, gint width, gint height, gint r)
{
    cairo_move_to(cr, r, 0);
    cairo_line_to(cr, width-r, 0);

	cairo_move_to(cr, width, r);
	cairo_line_to(cr, width, height-r);

	cairo_move_to(cr, width-r, height);
	cairo_move_to(cr, r, height);

	cairo_move_to(cr, 0, height-r);
	cairo_line_to(cr, 0, r);

	cairo_arc(cr, r, r, r, G_PI, 3 * G_PI / 2.0 );
	cairo_arc(cr, width-r, r, r, 3 * G_PI /2.0, 2 * G_PI);
	cairo_arc(cr, width-r, height-r, r, 0, G_PI / 2);
	cairo_arc(cr, r, height-r, r, G_PI / 2, G_PI);
}

static void on_play_event(GtkWidget *widget, gpointer data)
{
    g_thread_new("gst_play", gst_play_thread_cb, "/home/yjq/gtk/jianao.mp3");
    g_timeout_add(250, timer_cb, data);
    iDouWindow *win = IDOU_WINDOW(data);
    gtk_label_set_text(GTK_LABEL(win->music_name), "煎熬");
}

static gboolean timer_cb(gpointer data)
{
    iDouWindow *win = IDOU_WINDOW(data);
    GtkAdjustment *adjust = GTK_ADJUSTMENT(win->progress_adjust);
    GtkWidget *time_label = GTK_WIDGET(win->time_label);
    gint64 pos = gst_play_get_position();
    gint64 dur = gst_play_get_duration();
    gchar text[20];

    gint cur_time_s = pos / 1000000000;
    gint dur_time_s = dur / 1000000000;
    g_snprintf(text, 20, "%02u:%02u/%02u:%02u", cur_time_s/60, cur_time_s%60, dur_time_s/60, dur_time_s%60);

    gtk_adjustment_set_value(GTK_ADJUSTMENT(adjust), 1.0 * pos / dur * 100);
    gtk_label_set_text(GTK_LABEL(time_label), text);
    return TRUE;
}

static void on_time_scale_change_value(GtkWidget *widget, gpointer data)
{

}
