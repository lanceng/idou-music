#ifndef IDOU_WINDOW_H
#define IDOU_WINDOW_H

#include <gtk/gtk.h>

#define IDOU_TYPE_WINDOW    (idou_window_get_type())
#define IDOU_WINDOW(obj)    (G_TYPE_CHECK_INSTANCE_CAST(obj, IDOU_TYPE_WINDOW, iDouWindow))

typedef struct _iDouWindow iDouWindow;
typedef struct _iDouWindowClass iDouWindowClass;

struct _iDouWindow {
    GtkWindow parent;
    GtkWidget *music_name;
    GtkAdjustment *progress_adjust;
    GtkAdjustment *volume_adjust;
    GtkWidget *time_label;
};

struct _iDouWindowClass {
    GtkWindowClass parent_class;
};

GType idou_window_get_type();
GtkWidget *idou_window_new();
void idou_window_show();

#endif
