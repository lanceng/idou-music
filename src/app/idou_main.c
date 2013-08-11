#include <gtk/gtk.h>

#include "idou_window.h"

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    iDouWindow *window = idou_window_new();
    idou_window_show(window);

    gtk_main();
    return 0;
}

