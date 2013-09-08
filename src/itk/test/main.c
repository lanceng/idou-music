#include <gtk/gtk.h>
#include "itk_window.h"

int main(int argc, char *argv[])
{
    GtkWidget *window;
    gtk_init(&argc, &argv);

    window = itk_window_new();
    GtkWidget *button = gtk_button_new_with_label("我爱Gtk");
    itk_window_add(ITK_WINDOW(window), button);
    itk_window_show(window);

    gtk_main();
    return 0;
}
