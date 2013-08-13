#include "idou_titlebar.h"
#include "idou_button.h"

enum {
	IDOU_TITLEBAR_DESTROY_SIGNAL,
	IDOU_TITLEBAR_SIGNAL_N
};

static gint idou_titlebar_signals[IDOU_TITLEBAR_SIGNAL_N] = {0};
static void idou_titlebar_class_init(iDouTitlebarClass *titlebar_class);
static void idou_titlebar_init(iDouTitlebar *titlebar);
static void on_close_button_clicked(GtkWidget *widget);
static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data);

G_DEFINE_TYPE(iDouTitlebar, idou_titlebar, GTK_TYPE_EVENT_BOX);

static void idou_titlebar_class_init(iDouTitlebarClass *titlebar_class)
{
	GtkWidgetClass *object_class;
	object_class = (GtkWidgetClass*)titlebar_class;
	idou_titlebar_signals[IDOU_TITLEBAR_DESTROY_SIGNAL] =
					g_signal_new("idou-destroy",
					G_TYPE_FROM_CLASS(object_class),
					G_SIGNAL_RUN_FIRST,
					0,
					NULL, NULL,
					g_cclosure_marshal_VOID__VOID,
					G_TYPE_NONE, 0);
}

static void idou_titlebar_init(iDouTitlebar *titlebar)
{
    GtkWidget *hbox;
	GdkPixbuf *pixbuf;

    gtk_event_box_set_visible_window(GTK_EVENT_BOX(titlebar), FALSE);
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	titlebar->label = gtk_label_new (NULL);

    IDOU_BUTTON_NEW(titlebar->min_btn,
                    RESDIR"image/button/min_normal.png",
                    RESDIR"image/button/min_hover.png",
                    RESDIR"image/button/min_press.png",
                    25, 18);
    IDOU_BUTTON_NEW(titlebar->max_btn,
                    RESDIR"image/button/max_normal.png",
                    RESDIR"image/button/max_hover.png",
                    RESDIR"image/button/max_press.png",
                    25, 18);
    IDOU_BUTTON_NEW(titlebar->close_btn, 
                    RESDIR"image/button/close_normal.png", 
                    RESDIR"image/button/close_press.png", 
                    RESDIR"image/button/close_press.png", 
                    38, 18);

	g_signal_connect_swapped(G_OBJECT(titlebar->close_btn), "clicked",
			 G_CALLBACK(on_close_button_clicked), titlebar);
    g_signal_connect(G_OBJECT(titlebar), "button-press-event", G_CALLBACK(on_button_press), NULL);
	
	gtk_box_pack_start(GTK_BOX(hbox), titlebar->label, FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(hbox), titlebar->close_btn, FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(hbox), titlebar->max_btn, FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(hbox), titlebar->min_btn, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(titlebar), hbox);
}

GtkWidget *idou_titlebar_new ()
{
	return g_object_new(IDOU_TYPE_TITLEBAR, NULL);
}

GtkWidget *idou_titlebar_new_with_label(gchar *label)
{
	GtkWidget *widget = idou_titlebar_new ();
	idou_titlebar_set_label (widget, label);
	return widget;
}

void idou_titlebar_set_label(GtkWidget *widget, gchar *label)
{
    iDouTitlebar *titlebar = IDOU_TITLEBAR(widget);
	gtk_label_set_text (GTK_LABEL(titlebar->label), label);
}

static void on_close_button_clicked(GtkWidget *widget)
{
	g_signal_emit((gpointer)widget, idou_titlebar_signals[IDOU_TITLEBAR_DESTROY_SIGNAL], 
	              0);
}

static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if(event->type == GDK_BUTTON_PRESS)
    {
        if(event->button == 1)
        {
            gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                       event->button, event->x_root, event->y_root,
                                       event->time);
        }
    }
    return FALSE;
}
