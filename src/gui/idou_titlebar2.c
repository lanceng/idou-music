#include <gtk/gtk.h>
#include "idou_button.h"
#include "idou_titlebar2.h"
#include "idou_theme_dialog.h"

G_DEFINE_TYPE(iDouTitlebar2, idou_titlebar2, IDOU_TYPE_TITLEBAR);


static void on_skin_button_clicked(GtkWidget *widget, gpointer data);

static void idou_titlebar2_class_init(iDouTitlebar2Class *cls)
{

}

static void idou_titlebar2_init(iDouTitlebar2 *self)
{
    GList *list = gtk_container_get_children(GTK_CONTAINER(self));
    GtkWidget *hbox = GTK_WIDGET(list->data);

    GtkWidget *icon_btn;
    IDOU_BUTTON_SET(icon_btn,
                    RESDIR"image/icon/icon.png",
                    NULL, 
                    NULL, 
                    65, 30);
    IDOU_BUTTON_SET(self->skin_btn,
                    RESDIR"image/button/theme_normal.png",
                    RESDIR"image/button/theme_hover.png",
                    RESDIR"image/button/theme_press.png",
                    25, 18);
    gtk_box_pack_start(GTK_BOX(hbox), icon_btn, FALSE, FALSE, 2);
    gtk_box_pack_end(GTK_BOX(hbox), self->skin_btn, FALSE, FALSE, 0);

    g_signal_connect(G_OBJECT(self->skin_btn), "clicked", G_CALLBACK(on_skin_button_clicked), NULL);
}

GtkWidget *idou_titlebar2_new()
{
    g_object_new(IDOU_TYPE_TITLEBAR2, NULL);
}

static void on_skin_button_clicked(GtkWidget *widget, gpointer data)
{
    GtkAllocation alloc;
    gint root_x, root_y;

    GtkWidget *top_win = gtk_widget_get_toplevel(widget);
    GdkWindow *window = gtk_widget_get_window(widget);
    gtk_widget_get_allocation(widget, &alloc);
    gdk_window_get_root_origin(GDK_WINDOW(window), &root_x, &root_y);

    GtkWidget *dialog = idou_theme_dialog_new();
    gtk_window_move(GTK_WINDOW(dialog), root_x + alloc.x,
                    root_y + alloc.y + alloc.height);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(top_win));

    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
