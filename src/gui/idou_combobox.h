#ifndef IDOU_COMBOBOX_H
#define IDOU_COMBOBOX_H

#include <gtk/gtk.h>

#define IDOU_TYPE_COMBOBOX      (idou_combobox_get_type())
#define IDOU_COMBOBOX(obj)      (G_TYPE_CHECK_INSTANCE_CAST((obj), IDOU_TYPE_COMBOBOX, iDouCombobox))

typedef struct _iDouCombobox        iDouCombobox;
typedef struct _iDouComboboxClass   iDouComboboxClass; 
typedef enum   _iDouComboboxStatus  iDouComboboxStatus;

enum _iDouComboboxStatus {
    STATUS_NORMAL,
    STATUS_LEFT_HOVER,
    STATUS_LEFT_PRESS,
    STATUS_RIGHT_HOVER,
    STATUS_RIGHT_PRESS,
    STATUS_MAX
};

struct _iDouCombobox {
    GtkEventBox combobox; 
    GtkWidget *child;
    GtkWidget *popup_menu;
    GdkPixbuf *margin_left;
    GdkPixbuf *margin_middle;
    GdkPixbuf *margin_right;
    GdkPixbuf *left_normal;
    GdkPixbuf *left_hover;
    GdkPixbuf *left_press;
    GdkPixbuf *right_normal;
    GdkPixbuf *right_hover;
    GdkPixbuf *arrow_normal;
    GdkPixbuf *arrow_press;
    iDouComboboxStatus status;
};

struct _iDouComboboxClass {
    GtkEventBoxClass parent;
};

GType idou_combobox_get_type();
GtkWidget *idou_combobox_new();

#endif
