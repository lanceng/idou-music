#include "idou_combobox.h"

G_DEFINE_TYPE(iDouCombobox, idou_combobox, GTK_TYPE_EVENT_BOX);

static gboolean idou_combobox_draw(GtkWidget *widget, cairo_t *cr);
static void idou_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void *idou_pixbuf_scale(GdkPixbuf **pixbuf, gint width, gint height);
static gboolean on_mouse_enter(GtkWidget *widget, GdkEventMotion *event, gpointer data);
static gboolean on_mouse_leave(GtkWidget *widget, GdkEvent *event, gpointer data);
static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean on_button_release(GtkWidget *widget, GdkEventButton *event, gpointer data);

GtkWidget *idou_combobox_new()
{
    return g_object_new(IDOU_TYPE_COMBOBOX, NULL);
}

void idou_combobox_add_child(iDouCombobox *combobox, GtkWidget *child)
{
    combobox->child = child; 
}

static void idou_combobox_class_init(iDouComboboxClass *class)
{
    GtkWidgetClass *object_class = (GtkWidgetClass*)class;
    object_class->draw = idou_combobox_draw;
    object_class->size_allocate = idou_size_allocate; 
}

static void idou_combobox_init(iDouCombobox *self)
{
    self->child = NULL;
    self->popup_menu = gtk_menu_new();
    self->margin_left = gdk_pixbuf_new_from_file(RESDIR"image/combobox/margin_left.png", NULL);
    self->margin_middle = gdk_pixbuf_new_from_file(RESDIR"image/combobox/margin_middle.png", NULL);
    self->margin_right = gdk_pixbuf_new_from_file(RESDIR"image/combobox/margin_right.png", NULL);
    self->left_normal = gdk_pixbuf_new_from_file(RESDIR"image/combobox/left_normal.png", NULL);
    self->left_hover = gdk_pixbuf_new_from_file(RESDIR"image/combobox/left_hover.png", NULL);
    self->left_press = gdk_pixbuf_new_from_file(RESDIR"image/combobox/left_press.png", NULL);
    self->right_normal = gdk_pixbuf_new_from_file(RESDIR"image/combobox/right_normal.png", NULL);
    self->right_hover = gdk_pixbuf_new_from_file(RESDIR"image/combobox/right_hover.png", NULL);
    self->arrow_normal = gdk_pixbuf_new_from_file(RESDIR"image/combobox/arrow_normal.png", NULL);
    self->arrow_press = gdk_pixbuf_new_from_file(RESDIR"image/combobox/arrow_press.png", NULL);

    self->status = STATUS_NORMAL;
     
	gtk_widget_add_events(GTK_WIDGET(self), GDK_ENTER_NOTIFY_MASK |
                                              GDK_LEAVE_NOTIFY_MASK |
                                              GDK_BUTTON_PRESS_MASK |
                                              GDK_BUTTON_RELEASE_MASK);
    g_signal_connect(G_OBJECT(self), "enter-notify-event", G_CALLBACK(on_mouse_enter), NULL);
    g_signal_connect(G_OBJECT(self), "leave-notify-event", G_CALLBACK(on_mouse_leave), NULL);
    g_signal_connect(G_OBJECT(self), "button-press-event", G_CALLBACK(on_button_press), NULL);
    g_signal_connect(G_OBJECT(self), "button-release-event", G_CALLBACK(on_button_release), NULL);
}

static gboolean idou_combobox_draw(GtkWidget *widget, cairo_t *cr)
{
    GtkAllocation alloc;
    gint x, y, width, height;

    iDouCombobox *self = IDOU_COMBOBOX(widget);
    cr = gdk_cairo_create(gtk_widget_get_window(widget));
    gtk_widget_get_allocation(widget, &alloc);
    x = alloc.x;
    y = alloc.y;
    width = alloc.width;
    height = alloc.height;

    GdkPixbuf *left = self->left_normal;
    GdkPixbuf *right = self->right_normal;
    GdkPixbuf *arrow = NULL;
    switch(self->status)
    {
    case STATUS_NORMAL:
        break;
    case STATUS_LEFT_HOVER:
        left = self->left_hover;
        break;
    case STATUS_LEFT_PRESS:
        left = self->left_press;
        break;
    case STATUS_RIGHT_HOVER:
        right = self->right_hover;
        arrow = self->arrow_normal;
        break;
    case STATUS_RIGHT_PRESS:
        arrow = self->arrow_press;
        break;
    default:
        break;
    }

    gdk_cairo_set_source_pixbuf(cr, self->margin_left, x, y);
    cairo_paint(cr);
    gdk_cairo_set_source_pixbuf(cr, left, x+3, y);
    cairo_paint(cr);
    gdk_cairo_set_source_pixbuf(cr, self->margin_middle, width-19, y);
    cairo_paint(cr);
    gdk_cairo_set_source_pixbuf(cr, right, width-18, y);
    cairo_paint(cr);
    if(arrow != NULL)
    {
        gdk_cairo_set_source_pixbuf(cr, arrow, width-15, y);
        cairo_paint(cr);
    }
    gdk_cairo_set_source_pixbuf(cr, self->margin_right, width-3, y);
    cairo_paint(cr);

    cairo_destroy(cr);
    return TRUE;
}

static gboolean on_mouse_enter(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    iDouCombobox *self = IDOU_COMBOBOX(widget);
    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);
    if(event->type == GDK_ENTER_NOTIFY)
    {
        if(event->x >= 3 && event->x <= alloc.width-19)
        {
            self->status = STATUS_LEFT_HOVER;
        }
        else if(event->x >= alloc.width-18 && event->x <= alloc.width-3)
        {
            self->status = STATUS_RIGHT_HOVER;
        }
        else
            self->status = STATUS_NORMAL;
    }
    idou_combobox_draw(widget, NULL);
    return FALSE;
}

static gboolean on_mouse_leave(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    iDouCombobox *self = IDOU_COMBOBOX(widget);
    self->status = STATUS_NORMAL;
    idou_combobox_draw(widget, NULL);
    return FALSE;
}

static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    iDouCombobox *self = IDOU_COMBOBOX(widget);
    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);
    if(event->type == GDK_BUTTON_PRESS)
    {
        if(event->button == 1)
        {
            if(event->x >= 3 && event->x <= alloc.width-19) 
            {
                self->status = STATUS_LEFT_PRESS;
            }
            else if(event->x >= alloc.width-18 && event->x <= alloc.width-3)
            {
                self->status = STATUS_LEFT_PRESS;
            }
            else
                self->status = STATUS_NORMAL;
        }
    }
    return FALSE;
}

static gboolean on_button_release(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    return FALSE;
}

static void idou_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
    iDouCombobox *self = IDOU_COMBOBOX(widget);
    gint width, height;

	gtk_widget_set_allocation(widget, allocation);
	if (gtk_widget_get_realized (widget))
        gdk_window_move_resize (gtk_widget_get_window(widget),
	                            allocation->x,
	                            allocation->y,
	                            allocation->width,
	                            allocation->height);
    width = allocation->width;
    height = allocation->height;
    idou_pixbuf_scale(&self->margin_left, 3, height);
    idou_pixbuf_scale(&self->left_normal, width-22, height);
    idou_pixbuf_scale(&self->left_hover, width-22, height);
    idou_pixbuf_scale(&self->left_press, width-22, height);
    idou_pixbuf_scale(&self->margin_middle, 1, height);
    idou_pixbuf_scale(&self->right_normal, 15, height);
    idou_pixbuf_scale(&self->right_hover, 15, height);
    idou_pixbuf_scale(&self->margin_right, 3, height);
}

static void *idou_pixbuf_scale(GdkPixbuf **pixbuf, gint width, gint height)
{
    GdkPixbuf *src_p = NULL;
    src_p = *pixbuf;
    *pixbuf = gdk_pixbuf_scale_simple(src_p, width, height, GDK_INTERP_BILINEAR);
    g_object_unref(src_p);
}
