#include <gtk/gtk.h>
#include "idou_theme_dialog.h"

void attach_skin_from_dir(GtkWidget *grid, const gchar *dir_name);
static void on_skin_changed(GtkWidget *widget, gpointer data);

GtkWidget *idou_theme_dialog_new()
{
    GtkWidget *dialog = gtk_dialog_new();
    gtk_window_set_decorated(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 200, 200);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    attach_skin_from_dir(grid, RESDIR"image/skin");

    gtk_container_add(GTK_CONTAINER(content_area), grid);

    return dialog;
}

void attach_skin_from_dir(GtkWidget *grid, const gchar *dir_name)
{
    if(!g_file_test(dir_name, G_FILE_TEST_IS_DIR))
    {
        g_warning("%s is not directory.", dir_name);
        return;
    }
    GDir *dir = NULL;
    if(NULL == (dir = g_dir_open(dir_name, 0, NULL)))
    {
        g_warning("Can not open dirctory %s.", dir_name);
        return;
    }

    const gchar *file_name = NULL;
    gint count = 0;
    while(file_name = g_dir_read_name(dir))
    {
        gchar **strs = g_strsplit(file_name, ".", 2);
        gchar *name = strs[0];
        if(g_str_has_suffix(name, "Small"))
        {
            gchar *full_name = g_strconcat(dir_name, "/", file_name, NULL);
            GtkWidget *btn = gtk_button_new();
            gtk_widget_set_size_request(btn, 60, 60);
            GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_size(full_name, 60, 60, NULL);
            GtkWidget *img = gtk_image_new_from_pixbuf(pixbuf);
            gtk_button_set_image(GTK_BUTTON(btn), img);
            gtk_grid_attach(GTK_GRID(grid), btn, count%3, count/3, 1, 1);

            gchar **t = g_strsplit(name, "Small", 2);
            gchar *skin_filename = g_strconcat(dir_name, "/", t[0], ".", strs[1], NULL);
            g_object_set_data(G_OBJECT(btn), "filename", (gpointer)skin_filename);
            g_signal_connect(G_OBJECT(btn), "clicked", G_CALLBACK(on_skin_changed), NULL);

            g_strfreev(t);
            g_object_unref(pixbuf);
            g_free(full_name);
            count++;
        }
        g_strfreev(strs);
    }
    g_dir_close(dir);
}

static void on_skin_changed(GtkWidget *widget, gpointer data)
{
    gchar *file_name = g_object_get_data(G_OBJECT(widget), "filename");
    gchar *css_data = g_strconcat("#iDouWindow {"
                                  "background-image:url('",
                                  file_name, "');",
                                  "background-size: 100% 100%;"
                                  "}",
                                  NULL);
    GtkCssProvider *provider = gtk_css_provider_get_default();
    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_remove_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider));

    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css_data, -1, NULL);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_style_context_reset_widgets(screen);
}
