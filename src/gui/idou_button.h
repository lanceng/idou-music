#ifndef IDOU_BUTTON_H
#define IDOU_BUTTON_H

#include <gtk/gtk.h>

#define IDOU_TYPE_BUTTON		(idou_button_get_type())
#define IDOU_BUTTON(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), IDOU_TYPE_BUTTON, iDouButton))
#define IDOU_BUTTON_CLASS(class)	(G_TYPE_CHECK_CLASS_CAST((class), IDOU_TYPE_BUTTON, iDouButtonClass))
#define IDOU_IS_BUTTON(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), IDOU_TYPE_BUTTON))
#define IDOU_IS_BUTTON_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), IDOU_TYPE_BUTTON))
#define IDOU_BUTTON_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), IDOU_TYPE_BUTTON, iDouButtonClass))

typedef struct _iDouButton          iDouButton;
typedef struct _iDouButtonClass     iDouButtonClass;
typedef struct _iDouButtonPrivate   iDouButtonPrivate;

struct _iDouButton {
    GtkButton button;
    iDouButtonPrivate *priv;
};

struct _iDouButtonClass {
	GtkButtonClass button_class;
};

GType idou_button_get_type(void);
GtkWidget *idou_button_new(void);
GtkWidget *idou_button_new_with_pixbuf(GdkPixbuf *pixbuf);
void idou_button_set_pixbuf(GtkWidget *button, GdkPixbuf *pixbuf);
void idou_button_set_normal_pixbuf(GtkWidget *button, GdkPixbuf *pixbuf);
void idou_button_set_hovered_pixbuf(GtkWidget *button, GdkPixbuf *pixbuf);
void idou_button_set_pressed_pixbuf(GtkWidget *button, GdkPixbuf *pixbuf);
void idou_button_set_hovered_bg(GtkWidget *button, gboolean use_bg);

#endif
