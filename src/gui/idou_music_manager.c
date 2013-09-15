#include "idou_music_manager.h"
#include "idou_combobox.h"
#include "itk_button.h"

iDouMusicMgr *idou_music_mgr;

static GtkWidget *idou_music_view_new();
static void on_row_activated(GtkTreeView *treeview, GtkTreePath *path, 
        GtkTreeViewColumn *column, gpointer data);
static gboolean on_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data);
static void _play_music(GtkMenuItem *item, gpointer data);
static void _open_music_directory(GtkMenuItem *item, gpointer data);
static void _add_music(GtkMenuItem *item, gpointer data);
static void _remove_music(GtkMenuItem *item, gpointer data);
static void _foreach_list(gpointer data, gpointer user_data);

GtkWidget *idou_music_manager_new()
{
    idou_music_mgr = g_new0(iDouMusicMgr, 1);
    idou_music_mgr->music_mgr = gtk_notebook_new();
    GtkWidget *music_mgr;
    music_mgr = idou_music_mgr->music_mgr;

    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *music_view = idou_music_view_new();
    gtk_container_add(GTK_CONTAINER(scrolled), music_view);

    GtkWidget *label;
    label = gtk_label_new("播放列表");
    gtk_widget_set_size_request(label, 50, 30);
    gtk_notebook_append_page(GTK_NOTEBOOK(music_mgr), scrolled, label);

    label = gtk_label_new("本地音乐");
    gtk_widget_set_size_request(label, 50, 30);
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_notebook_append_page(GTK_NOTEBOOK(music_mgr), scrolled, label);

    label = gtk_label_new("网络音乐");
    gtk_widget_set_size_request(label, 50, 30);
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_notebook_append_page(GTK_NOTEBOOK(music_mgr), scrolled, label);

    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);

    const char *css_data =  "GtkNoteBook {"
                            "-GtkWidget-focus-line-width: 0;"
                            "padding: 0 0 0 0;"
                            "border-width: 0;"
                            "border-radius: 0"
                            "}";
    gtk_css_provider_load_from_data(GTK_CSS_PROVIDER(provider), css_data, -1, NULL);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), 
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    g_object_unref(provider);

    return music_mgr;
}

static GtkWidget *idou_music_view_new()
{
    GtkTreeStore *tree_store = gtk_tree_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkTreeIter iter;
    gtk_tree_store_append(tree_store, &iter, NULL);
    gtk_tree_store_set(tree_store, &iter, 0, "煎熬", 1, "--", 2, "3:00", -1);

    GtkWidget *tree_view = gtk_tree_view_new();

    GtkTreeViewColumn *column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(GTK_TREE_VIEW_COLUMN(column), "歌曲");
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 0);

    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(GTK_TREE_VIEW_COLUMN(column), "歌手");
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 1);

    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(GTK_TREE_VIEW_COLUMN(column), "时长");
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 2);

    gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), tree_store);

    g_signal_connect(G_OBJECT(tree_view), "row-activated", G_CALLBACK(on_row_activated), NULL);
    g_signal_connect(G_OBJECT(tree_view), "button-press-event", G_CALLBACK(on_button_press_event), (gpointer)idou_music_mgr);

    GtkWidget *menu;
    GtkWidget *item;
    idou_music_mgr->item_right_menu = gtk_menu_new();
    menu = idou_music_mgr->item_right_menu;
    item = gtk_menu_item_new_with_label("播放");
    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(_play_music), (gpointer)idou_music_mgr);
    gtk_menu_shell_append(menu, item);

    item = gtk_menu_item_new_with_label("打开目录");
    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(_open_music_directory), NULL);
    gtk_menu_shell_append(menu, item);

    item = gtk_menu_item_new_with_label("移除");
    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(_remove_music), NULL);
    gtk_menu_shell_append(menu, item);
    gtk_widget_show_all(menu);

    idou_music_mgr->right_menu = gtk_menu_new();
    menu = idou_music_mgr->right_menu;
    item = gtk_menu_item_new_with_label("打开目录");
    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(_open_music_directory), NULL);
    gtk_menu_shell_append(menu, item);

    item = gtk_menu_item_new_with_label("添加歌曲");
    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(_add_music), (gpointer)tree_view);
    gtk_menu_shell_append(menu, item);
    gtk_widget_show_all(menu);

    return tree_view;
}

static void on_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer data)
{
}

static gboolean on_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    iDouMusicMgr *mgr = (iDouMusicMgr *)data;
    GtkTreePath *path;

    gboolean is_exist = gtk_tree_view_get_path_at_pos(
                            GTK_TREE_VIEW(widget), 
                            event->x, event->y, 
                            &path, NULL, 
                            NULL, NULL);
    if(path != NULL)
    {
        mgr->cur_index = *gtk_tree_path_get_indices(path);    
        gtk_tree_path_free(path);
    }

    if(event->button == GDK_BUTTON_SECONDARY)
    {
        if(is_exist)
        {
            guint32 time = gtk_get_current_event_time();
            gtk_menu_popup(GTK_MENU(mgr->item_right_menu), NULL, NULL, NULL, NULL, 0, time);
        }
        else
        {
            gtk_menu_popup(GTK_MENU(mgr->right_menu), NULL, NULL, NULL, event, event->button, event->time);
        }
    }
    return FALSE;
}

static void _play_music(GtkMenuItem *item, gpointer data)
{

}

static void _open_music_directory(GtkMenuItem *item, gpointer data)
{
    char *argv[] = {"xdg-open", "/home/yjq/Music", NULL };

    /** use a cast to shut up gcc **/
    g_spawn_async( NULL, (gchar **)argv, NULL, (GSpawnFlags)(G_SPAWN_SEARCH_PATH |
                G_SPAWN_STDOUT_TO_DEV_NULL |
                G_SPAWN_STDERR_TO_DEV_NULL),
                NULL, NULL, NULL, NULL);
}

static void _add_music(GtkMenuItem *item, gpointer data)
{
    GtkWindow *toplevel = gtk_widget_get_toplevel(GTK_WIDGET(data));
    GtkWidget *file_chooser = gtk_file_chooser_dialog_new("添加音乐", toplevel, GTK_FILE_CHOOSER_ACTION_OPEN, 
                                                          "取消(_C)", GTK_RESPONSE_CANCEL, 
                                                          "添加(_S)", GTK_RESPONSE_APPLY, 
                                                          NULL);
    const gchar *home_dir = g_get_home_dir();
    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(file_chooser), TRUE);
    gtk_window_set_position(GTK_WINDOW(file_chooser), GTK_WIN_POS_CENTER_ON_PARENT);
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(file_chooser), home_dir);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Music Files");
    gtk_file_filter_add_pattern(filter, "*.[Mm][Pp]3");
    gtk_file_filter_add_pattern(filter, "*.[Ww][Aa][Vv]");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), filter);

    gint response = gtk_dialog_run(GTK_DIALOG(file_chooser));
    if (response == GTK_RESPONSE_CANCEL)
    {
        goto add_music_exit;
    }
    else if (response == GTK_RESPONSE_APPLY)
    {
        GSList *uris;
        uris = gtk_file_chooser_get_uris(GTK_FILE_CHOOSER(file_chooser));
        if (NULL == uris)
        {
            goto add_music_exit;
        }
        gchar *dir_path = g_build_filename(home_dir, "/.idou-music2", NULL);
        if (!g_file_test(dir_path, G_FILE_TEST_IS_DIR))
        {
            g_mkdir_with_parents(dir_path, 0755);
        }
        gchar *file_path = g_build_filename(home_dir, "/.idou-music2/idoudb.xml", NULL);
        if (!g_file_test(file_path, G_FILE_TEST_IS_REGULAR))
        {
            g_creat(file_path, 0755);
        }
        g_slist_foreach(uris, _foreach_list, NULL);

        g_free(dir_path);
        g_free(file_path);
        g_slist_free_full(uris, g_free);
    }

add_music_exit:
    gtk_widget_destroy(file_chooser);
}

static void _foreach_list(gpointer data, gpointer user_data)
{
    if (data)
    {
    }
}

static void _remove_music(GtkMenuItem *item, gpointer data)
{

}
