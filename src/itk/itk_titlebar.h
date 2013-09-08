#ifndef ITK_TITLEBAR_H
#define ITK_TITLEBAR_H

#include <gtk/gtk.h>

#define ITK_TYPE_TITLEBAR			(itk_titlebar_get_type())
#define ITK_TITLEBAR(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), ITK_TYPE_TITLEBAR, ItkTitlebar))
#define ITK_TITLEBAR_CLASS(class)	(G_TYPE_CHECK_CLASS_CAST((class), ITK_TYPE_TITLEBAR, ItkTitlebarClass))

typedef struct _ItkTitlebar 		ItkTitlebar;
typedef struct _ItkTitlebarClass ItkTitlebarClass;

struct _ItkTitlebar
{
    GtkEventBox parent;
    GtkWidget *label;
    GtkWidget *min_btn;
    GtkWidget *max_btn;
    GtkWidget *close_btn;
};

struct _ItkTitlebarClass
{
    GtkEventBoxClass parent_class;
};

GType itk_titlebar_get_type();
GtkWidget *itk_titlebar_new();
GtkWidget *itk_titlebar_new_with_label(gchar *label);
void itk_titlebar_set_label(GtkWidget *widget, gchar *label);

#endif
