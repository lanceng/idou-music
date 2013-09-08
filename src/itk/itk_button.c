#include "itk_button.h"

#define DEFAULT_SIZE 16
#define ITK_BUTTON_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), \
		itk_button_get_type(), ItkButtonPrivate))

struct _ItkButtonPrivate
{
	GdkPixbuf *normal_img;
	GdkPixbuf *hovered_img;
	GdkPixbuf *pressed_img;
	gboolean use_hovered_bg;
	gint btn_status;
};

G_DEFINE_TYPE(ItkButton, itk_button, GTK_TYPE_BUTTON);

enum {
	PRESSED,
	RELEASED,
	CLITKKED,
	ENTER,
	LEAVE,
	ACTIVATE,
	LAST_SIGNAL
};

static void itk_button_class_init(ItkButtonClass *button_class);
static void itk_button_init(ItkButton *button);
static void itk_button_size_allocate(GtkWidget *widget, GtkAllocation *allocation);

static gboolean itk_button_draw(GtkWidget *widget, cairo_t *cr);
static gboolean itk_button_enter(GtkWidget *widget, gpointer data);
static gboolean itk_button_leave(GtkWidget *widget, gpointer data);
static gboolean itk_button_pressed(GtkWidget *widget, gpointer data);
static gboolean itk_button_released(GtkWidget *widget, gpointer data);

static void itk_button_class_init(ItkButtonClass *button_class)
{
	g_type_class_add_private(button_class, sizeof(ItkButtonPrivate));
	GtkWidgetClass *object_class;
	object_class = (GtkWidgetClass*)button_class;
	object_class->draw = itk_button_draw;
	object_class->size_allocate = itk_button_size_allocate;
}

static void itk_button_init(ItkButton *button)
{
    ItkButtonPrivate *priv;

    button->priv = G_TYPE_INSTANCE_GET_PRIVATE(button,
                    ITK_TYPE_BUTTON,
            ItkButtonPrivate);

	priv = button->priv;
	priv->normal_img = NULL;
	priv->hovered_img = NULL;
	priv->pressed_img = NULL;
	priv->use_hovered_bg = FALSE;
	priv->btn_status = -1;

	gtk_widget_add_events(GTK_WIDGET(button), GDK_ENTER_NOTIFY_MASK |
                                              GDK_LEAVE_NOTIFY_MASK |
                                              GDK_BUTTON_PRESS_MASK |
                                              GDK_BUTTON_RELEASE_MASK);

	g_signal_connect(G_OBJECT(button), "enter-notify-event",
			G_CALLBACK(itk_button_enter), NULL);
	g_signal_connect(G_OBJECT(button), "leave-notify-event",
			G_CALLBACK(itk_button_leave), NULL);
	g_signal_connect(G_OBJECT(button), "button-press-event",
			G_CALLBACK(itk_button_pressed), NULL);
	g_signal_connect(G_OBJECT(button), "button-release-event",
			G_CALLBACK(itk_button_released), NULL);
}

GtkWidget *itk_button_new(void)
{
	return g_object_new(ITK_TYPE_BUTTON, NULL);
}

GtkWidget *itk_button_new_with_pixbuf(GdkPixbuf *pixbuf)
{
	GtkWidget * widget = g_object_new(ITK_TYPE_BUTTON, NULL);
    itk_button_set_pixbuf(widget, pixbuf);

    return widget;
}

void itk_button_set_pixbuf(GtkWidget *button, GdkPixbuf *pixbuf)
{
	ItkButtonPrivate *priv = NULL;

	priv = ITK_BUTTON(button)->priv;
	priv->normal_img = pixbuf;
	priv->hovered_img = pixbuf;
	priv->pressed_img = pixbuf;
}

void itk_button_set_normal_pixbuf(GtkWidget *button, GdkPixbuf *pixbuf)
{
    ItkButtonPrivate *priv = NULL;

    priv = ITK_BUTTON(button)->priv;
    priv->normal_img = pixbuf;
}

void itk_button_set_hovered_pixbuf(GtkWidget *button, GdkPixbuf *pixbuf)
{
	ItkButtonPrivate *priv = NULL;

	priv = ITK_BUTTON(button)->priv;
	priv->hovered_img = pixbuf;
	priv->use_hovered_bg = FALSE;
}

void itk_button_set_pressed_pixbuf(GtkWidget *button, GdkPixbuf *pixbuf)
{
	ItkButtonPrivate *priv = NULL;

	priv = ITK_BUTTON(button)->priv;
	priv->pressed_img = pixbuf;
}

void itk_button_set_hovered_bg(GtkWidget *button, gboolean use_bg)
{
   ItkButtonPrivate *priv = NULL;

   priv = ITK_BUTTON(button)->priv;
   priv->use_hovered_bg = use_bg;
}

static void itk_button_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
	GtkButton *button = GTK_BUTTON (widget);
	gtk_widget_set_allocation(widget, allocation);

	if (gtk_widget_get_realized (widget))
	    gdk_window_move_resize (gtk_button_get_event_window(button),
	                            allocation->x,
	                            allocation->y,
	                            allocation->width,
	                            allocation->height);
}

static gboolean itk_button_draw(GtkWidget *widget, cairo_t *cr)
{
	ItkButtonPrivate *priv = NULL;
	GtkAllocation alloc;
	gint x, y, width, height, r;

	priv = ITK_BUTTON(widget)->priv;
	gtk_widget_get_allocation(widget, &alloc);
	x = alloc.x;
	y = alloc.y;
	width = alloc.width;
	height = alloc.height;
	r = 3;
	
	cr = gdk_cairo_create(gtk_widget_get_window(widget));
	if(priv->btn_status != LEAVE && priv->btn_status != -1 && priv->use_hovered_bg)
	{
		cairo_set_line_width (cr, 1);
		cairo_move_to (cr, x+r, y);
		cairo_line_to (cr, x+width-r, y);
		cairo_move_to (cr, x+width, y+r);
		cairo_line_to (cr, x+width, y+height-r);
		cairo_move_to (cr, x+width-r, y+height);
		cairo_move_to (cr, x+r, y+height);
		cairo_move_to (cr, x, y+height-r);
		cairo_line_to (cr, x, y+r);
		cairo_arc (cr, x+r, y+r, r, G_PI, 3 * G_PI / 2.0 );
		cairo_arc (cr, x+width-r, y+r, r, 3 * G_PI /2.0, 2 * G_PI);
		cairo_arc (cr, x+width-r, y+height-r, r, 0, G_PI / 2);
		cairo_arc (cr, x+r, y+height-r, r, G_PI / 2, G_PI);
		cairo_set_source_rgba(cr, 1, 1, 1, 0.4);
		cairo_fill(cr);
	}

	switch(priv->btn_status)
	{
	case PRESSED :
		gdk_cairo_set_source_pixbuf(cr, priv->pressed_img, alloc.x, alloc.y);
		cairo_paint(cr);
		break;
	case RELEASED :
		gdk_cairo_set_source_pixbuf(cr, priv->hovered_img, alloc.x, alloc.y);
		cairo_paint(cr);
		break;
	case ENTER :
		gdk_cairo_set_source_pixbuf(cr, priv->hovered_img, alloc.x, alloc.y);
		cairo_paint(cr);
		break;
	case LEAVE :
		gdk_cairo_set_source_pixbuf(cr, priv->normal_img, alloc.x, alloc.y);
		cairo_paint(cr);
		break;
	default :
		gdk_cairo_set_source_pixbuf(cr, priv->normal_img, alloc.x, alloc.y);
		cairo_paint(cr);
		break;
	}

	cairo_destroy(cr);
	return TRUE;
}

static gboolean itk_button_enter(GtkWidget *widget, gpointer data)
{
	ItkButton *button = ITK_BUTTON(widget);
	button->priv->btn_status = ENTER;
	itk_button_draw(widget, NULL);
	return FALSE;
}

static gboolean itk_button_leave(GtkWidget *widget, gpointer data)
{
	ItkButton *button = ITK_BUTTON(widget);
	button->priv->btn_status = LEAVE;
	itk_button_draw(widget, NULL);
	return FALSE;
}

static gboolean itk_button_pressed(GtkWidget *widget, gpointer data)
{
	ItkButton *button = ITK_BUTTON(widget);
	button->priv->btn_status = PRESSED;
	itk_button_draw(widget, NULL);
	return FALSE;
}

static gboolean itk_button_released(GtkWidget *widget, gpointer data)
{
	ItkButton *button = ITK_BUTTON(widget);
	button->priv->btn_status = RELEASED;
	itk_button_draw(widget, NULL);
	return FALSE;
}

