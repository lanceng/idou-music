#ifndef ITK_BUTTON_H
#define ITK_BUTTON_H

#include <gtk/gtk.h>

#define ITK_TYPE_BUTTON		(itk_button_get_type())
#define ITK_BUTTON(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), ITK_TYPE_BUTTON, ItkButton))
#define ITK_BUTTON_CLASS(class)	(G_TYPE_CHECK_CLASS_CAST((class), ITK_TYPE_BUTTON, ItkButtonClass))
#define ITK_IS_BUTTON(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), ITK_TYPE_BUTTON))
#define ITK_IS_BUTTON_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), ITK_TYPE_BUTTON))
#define ITK_BUTTON_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), ITK_TYPE_BUTTON, ItkButtonClass))

#define ITK_BUTTON_SET(btn, path1, path2, path3, width, height)   { \
        btn = itk_button_new(); \
        gtk_widget_set_size_request(btn, width, height); \
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale(path1, width, height, FALSE, NULL); \
        itk_button_set_pixbuf(btn, pixbuf); \
        if(path2 != NULL) { \
            pixbuf = gdk_pixbuf_new_from_file_at_scale(path2, width, height, FALSE, NULL); \
            itk_button_set_hovered_pixbuf(btn, pixbuf); \
        } \
        if(path3 != NULL) { \
            pixbuf = gdk_pixbuf_new_from_file_at_scale(path3, width, height, FALSE, NULL); \
            itk_button_set_pressed_pixbuf(btn, pixbuf); \
        }} 

typedef struct _ItkButton          ItkButton;
typedef struct _ItkButtonClass     ItkButtonClass;
typedef struct _ItkButtonPrivate   ItkButtonPrivate;

struct _ItkButton {
    GtkButton button;
    ItkButtonPrivate *priv;
};

struct _ItkButtonClass {
	GtkButtonClass button_class;
};

GType itk_button_get_type(void);
GtkWidget *itk_button_new(void);
GtkWidget *itk_button_new_with_pixbuf(GdkPixbuf *pixbuf);
void itk_button_set_pixbuf(GtkWidget *button, GdkPixbuf *pixbuf);
void itk_button_set_normal_pixbuf(GtkWidget *button, GdkPixbuf *pixbuf);
void itk_button_set_hovered_pixbuf(GtkWidget *button, GdkPixbuf *pixbuf);
void itk_button_set_pressed_pixbuf(GtkWidget *button, GdkPixbuf *pixbuf);
void itk_button_set_hovered_bg(GtkWidget *button, gboolean use_bg);

#endif
