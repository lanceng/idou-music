#ifndef IDOU_MUSIC_MANAGER_H
#define IDOU_MUSIC_MANAGER_H

#include <gtk/gtk.h>

typedef struct _iDouMusicMgr{
    GtkWidget *music_mgr;
    GtkWidget *right_menu;
    GtkWidget *item_right_menu;
    gint cur_index;
}iDouMusicMgr;

GtkWidget *idou_music_manager_new();

#endif
