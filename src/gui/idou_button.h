#ifndef IDOU_BUTTON_H
#define IDOU_BUTTON_H

#include <gtk/gtk.h>

#define IDOU_TYPE_BUTTON		(idou_button_get_type())
#define IDOU_BUTTON(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), IDOU_TYPE_BUTTON, iDouButton))
#define IDOU_BUTTON_CLASS(class)	(G_TYPE_CHECK_CLASS_CAST((class), IDOU_TYPE_BUTTON, iDouButtonClass))
#define IDOU_IS_BUTTON(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), IDOU_TYPE_BUTTON))
#define IDOU_IS_BUTTON_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), IDOU_TYPE_BUTTON))
#define IDOU_BUTTON_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), IDOU_TYPE_BUTTON, iDouButtonClass))

#define IDOU_BUTTON_NEW(btn, path1, path2, path3, width, height)   { \
        btn = idou_button_new(); \
        gtk_widget_set_size_request(btn, width, height); \
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale(path1, width, height, FALSE, NULL); \
        idou_button_set_pixbuf(btn, pixbuf); \
        if(path2 != NULL) { \
            pixbuf = gdk_pixbuf_new_from_file_at_scale(path2, width, height, FALSE, NULL); \
            idou_button_set_hovered_pixbuf(btn, pixbuf); \
        } \
        if(path3 != NULL) { \
            pixbuf = gdk_pixbuf_new_from_file_at_scale(path3, width, height, FALSE, NULL); \
            idou_button_set_pressed_pixbuf(btn, pixbuf); \
        }} 

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
