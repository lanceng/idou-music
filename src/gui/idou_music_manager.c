#include "idou_music_manager.h"
#include "idou_combobox.h"
#include "itk_button.h"

static GtkWidget *idou_music_view_new();
static void on_row_activated(GtkTreeView *treeview, GtkTreePath *path, 
        GtkTreeViewColumn *column, gpointer data);
static gboolean on_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data);
static void on_menu_deactivate(GtkMenuShell *menu, gpointer data);
static void _play_music(GtkMenuItem *item, gpointer data);
static void _open_music_directory(GtkMenuItem *item, gpointer data);
static void _remove_music(GtkMenuItem *item, gpointer data);

GtkWidget *idou_music_manager_new()
{
    GtkWidget *music_manager = gtk_notebook_new();
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *music_view = idou_music_view_new();
    gtk_container_add(GTK_CONTAINER(scrolled), music_view);

    GtkWidget *label;
    label = gtk_label_new("播放列表");
    gtk_widget_set_size_request(label, 50, 30);
    gtk_notebook_append_page(GTK_NOTEBOOK(music_manager), scrolled, label);

    label = gtk_label_new("本地音乐");
    gtk_widget_set_size_request(label, 50, 30);
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_notebook_append_page(GTK_NOTEBOOK(music_manager), scrolled, label);

    label = gtk_label_new("网络音乐");
    gtk_widget_set_size_request(label, 50, 30);
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_notebook_append_page(GTK_NOTEBOOK(music_manager), scrolled, label);

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

    return music_manager;
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
    g_signal_connect(G_OBJECT(tree_view), "button-press-event", G_CALLBACK(on_button_press_event), NULL);

    return tree_view;
}

static void on_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer data)
{
}

static void get_position(GtkMenu *menu, gint *x, gint *y, 
                                  gboolean *push_in, gpointer data)
{
    GdkEventButton *event = (GdkEventButton *)data;
    *x = event->x_root;
    *y = event->y_root;
}

static gboolean on_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    gboolean is_exist = gtk_tree_view_get_path_at_pos(
                            GTK_TREE_VIEW(widget), 
                            event->x, event->y, 
                            NULL, NULL, 
                            NULL, NULL);
    if(event->button == GDK_BUTTON_SECONDARY)
    {
        if(is_exist)
        {
            GtkWidget *menu = gtk_menu_new();
            GtkWidget *item = gtk_menu_item_new_with_label("播放");
            g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(_play_music), NULL);
            gtk_menu_shell_append(menu, item);

            item = gtk_menu_item_new_with_label("打开目录");
            g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(_open_music_directory), NULL);
            gtk_menu_shell_append(menu, item);

            item = gtk_menu_item_new_with_label("移除");
            g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(_remove_music), NULL);
            gtk_menu_shell_append(menu, item);

            gtk_widget_show_all(menu);
            //g_signal_connect(G_OBJECT(menu), "deactivate", G_CALLBACK(on_menu_deactivate), NULL);
            guint32 time = gtk_get_current_event_time();
            gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, 0, time);
        }
        else
        {
            GtkWidget *menu = gtk_menu_new();
            GtkWidget *item = gtk_menu_item_new_with_label("打开目录");
            gtk_menu_shell_append(menu, item);
            item = gtk_menu_item_new_with_label("添加歌曲");
            gtk_menu_shell_append(menu, item);
            gtk_widget_show_all(menu);
            g_signal_connect(G_OBJECT(menu), "deactivate", G_CALLBACK(on_menu_deactivate), NULL);
            gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, event, event->button, event->time);
        }
    }
    return FALSE;
}

static void on_menu_deactivate(GtkMenuShell *menu, gpointer data)
{
    gtk_widget_destroy(menu);
}

static void _play_music(GtkMenuItem *item, gpointer data)
{

}

static void _open_music_directory(GtkMenuItem *item, gpointer data)
{
    g_message("open directory");
    char *argv[] = { "xdg-open", "/home/Download", NULL };

    /** use a cast to shut up gcc **/
    g_spawn_async( NULL, (gchar **)argv, NULL, (GSpawnFlags)(G_SPAWN_SEARCH_PATH |
    G_SPAWN_STDOUT_TO_DEV_NULL |
    G_SPAWN_STDERR_TO_DEV_NULL |
    G_SPAWN_STDERR_TO_DEV_NULL),
    NULL, NULL, NULL, NULL );
}

static void _remove_music(GtkMenuItem *item, gpointer data)
{

}
