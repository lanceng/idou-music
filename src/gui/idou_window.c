#include <gst/gst.h>

#include "idou_window.h"
#include "idou_titlebar2.h"
#include "idou_window.h"
#include "idou_button.h"
#include "gstplay.h"

G_DEFINE_TYPE(iDouWindow, idou_window, GTK_TYPE_WINDOW);


static GtkWidget *idou_music_view_new();
static gpointer on_draw(GtkWidget *widget, gpointer data);
static void on_row_activated(GtkTreeView *treeview, GtkTreePath *path, 
        GtkTreeViewColumn *column, gpointer data);
static void on_play_event(GtkWidget *widget, gpointer data);
static void on_time_scale_change_value(GtkWidget *widget, gpointer data);
static gboolean timer_cb(gpointer data);

void idou_window_class_init(iDouWindowClass *klass)
{
}

void idou_window_init(iDouWindow *self)
{
    GtkWidget *window;
    GtkWidget *hbox, *hbox2, *hbox3, *hbox4;
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

    GtkWidget *title_bar = idou_titlebar2_new();
    idou_titlebar_set_label(title_bar, "爱逗音乐");
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
    pixbuf = gdk_pixbuf_new_from_file_at_size(RESDIR"image/button/last.png", 32, 32, NULL);
    image = gtk_image_new_from_pixbuf(pixbuf);
    GtkWidget *last_btn = gtk_button_new();
    gtk_button_set_image(last_btn, image);

    pixbuf = gdk_pixbuf_new_from_file_at_size(RESDIR"image/button/play.png", 32, 32, NULL);
    image = gtk_image_new_from_pixbuf(pixbuf);
    GtkWidget *play_btn = gtk_button_new();
    gtk_button_set_image(play_btn, image);
    g_signal_connect(G_OBJECT(play_btn), "clicked", G_CALLBACK(on_play_event), (gpointer)self);

    pixbuf = gdk_pixbuf_new_from_file_at_size(RESDIR"image/button/next.png", 32, 32, NULL);
    image = gtk_image_new_from_pixbuf(pixbuf);
    GtkWidget *next_btn = gtk_button_new();
    gtk_button_set_image(next_btn, image);

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

    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *music_view = idou_music_view_new();
    gtk_container_add(GTK_CONTAINER(scrolled), music_view);

    GtkWidget *notebook = gtk_notebook_new();
    GtkWidget *label = gtk_label_new("播放列表");
    gtk_widget_set_size_request(label, 150, -1);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled, label);

    label = gtk_label_new("音乐电台");
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled, label);
    gtk_box_pack_start(GTK_BOX(vbox2), notebook, TRUE, TRUE, 2);

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
    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);
    gint width = alloc.width;
    gint height = alloc.height;
    gint r = 5;

    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                          width,
                                                          height);
    cairo_t *cr = cairo_create(surface);

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

    cairo_stroke_preserve(cr);
    cairo_fill(cr) ;

    cairo_region_t *region = gdk_cairo_region_create_from_surface(surface);
    gtk_widget_shape_combine_region(widget, region);
}

static GtkWidget *idou_music_view_new()
{
    GtkTreeStore *tree_store = gtk_tree_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkTreeIter iter;
    gtk_tree_store_append(tree_store, &iter, NULL);
    gtk_tree_store_set(tree_store, &iter, 0, "亏欠一生", 1, "裘海正", 2, "3:00", -1);

    GtkWidget *tree_view = gtk_tree_view_new();

    GtkTreeViewColumn *column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(GTK_TREE_VIEW_COLUMN(column), "歌曲");
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 0);

    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(GTK_TREE_VIEW_COLUMN(column), "歌手");
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 1);

    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(GTK_TREE_VIEW_COLUMN(column), "时长");
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 2);

    gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), tree_store);

    g_signal_connect(G_OBJECT(tree_view), "row-activated", G_CALLBACK(on_row_activated), NULL);

    return tree_view;
}

static void on_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer data)
{

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
