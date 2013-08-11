#ifndef IDOU_TITLEBAR_H
#define IDOU_TITLEBAR_H

#include <gtk/gtk.h>

#define IDOU_TYPE_TITLEBAR			(idou_titlebar_get_type())
#define IDOU_TITLEBAR(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), IDOU_TYPE_TITLEBAR, iDouTitlebar))
#define IDOU_TITLEBAR_CLASS(class)	(G_TYPE_CHECK_CLASS_CAST((class), IDOU_TYPE_TITLEBAR, iDouTitlebarClass))

typedef struct _iDouTitlebar 		iDouTitlebar;
typedef struct _iDouTitlebarClass iDouTitlebarClass;

struct _iDouTitlebar
{
    GtkEventBox parent;
    GtkWidget *label;
    GtkWidget *min_btn;
    GtkWidget *max_btn;
    GtkWidget *close_btn;
};

struct _iDouTitlebarClass
{
    GtkEventBoxClass parent_class;
};

GType idou_titlebar_get_type();
GtkWidget *idou_titlebar_new();
GtkWidget *idou_titlebar_new_with_label(gchar *label);
void idou_titlebar_set_label(GtkWidget *widget, gchar *label);

#endif
