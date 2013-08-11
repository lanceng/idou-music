#ifndef IDOU_TITLEBAR2_H
#define IDOU_TITLEBAR2_H

#include "idou_titlebar.h"

#define IDOU_TYPE_TITLEBAR2 (idou_titlebar2_get_type())

typedef struct _iDouTitlebar2Class iDouTitlebar2Class;
typedef struct _iDouTitlebar2 iDouTitlebar2;

struct _iDouTitlebar2Class {
    iDouTitlebarClass cls;     
};

struct _iDouTitlebar2 {
    iDouTitlebar parent;
    GtkWidget *skin_btn;
};

GType idou_titlebar2_get_type();
GtkWidget *idou_titlebar2_new();

#endif
