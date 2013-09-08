#ifndef IDOU_TITLEBAR_H
#define IDOU_TITLEBAR_H

#include "itk_titlebar.h"

#define IDOU_TYPE_TITLEBAR (idou_titlebar_get_type())

typedef struct _iDouTitlebarClass iDouTitlebarClass;
typedef struct _iDouTitlebar iDouTitlebar;

struct _iDouTitlebarClass {
    ItkTitlebarClass cls;     
};

struct _iDouTitlebar{
    ItkTitlebar parent;
    GtkWidget *skin_btn;
};

GType idou_titlebar_get_type();
GtkWidget *idou_titlebar_new();

#endif
