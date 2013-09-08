#ifndef ITK_WINDOW_H
#define ITK_WINDOW_H

#include <gtk/gtk.h>

#define ITK_TYPE_WINDOW    (itk_window_get_type())
#define ITK_WINDOW(obj)    (G_TYPE_CHECK_INSTANCE_CAST(obj, ITK_TYPE_WINDOW, ItkWindow))

typedef struct _ItkWindow ItkWindow;
typedef struct _ItkWindowClass ItkWindowClass;

struct _ItkWindow {
    GtkWindow parent;
    GtkWidget *window_frame;
    GdkCursorType cursor_type;
    gint shadow_padding;
    gint shadow_radius;
    gint frame_radius;
};

struct _ItkWindowClass {
    GtkWindowClass parent_class;
};

GType itk_window_get_type();
GtkWidget *itk_window_new();
void itk_window_add(ItkWindow *self, GtkWidget *child);
void itk_window_show(GtkWidget *self);

#endif
