#include "idou_music_manager.h"
#include "idou_combobox.h"

static GtkWidget *idou_music_view_new();
static void on_row_activated(GtkTreeView *treeview, GtkTreePath *path, 
        GtkTreeViewColumn *column, gpointer data);

GtkWidget *idou_music_manager_new()
{
    GtkWidget *music_manager = gtk_notebook_new();
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *music_view = idou_music_view_new();
    gtk_container_add(GTK_CONTAINER(scrolled), music_view);

    GtkWidget *label = gtk_label_new("播放列表");
    gtk_widget_set_size_request(label, 150, -1);
    gtk_notebook_append_page(GTK_NOTEBOOK(music_manager), scrolled, label);

    label = gtk_label_new("音乐电台");
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_notebook_append_page(GTK_NOTEBOOK(music_manager), scrolled, label);

    scrolled = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *combobox = idou_combobox_new();
    gtk_widget_set_size_request(combobox, 30, 20);
    gtk_notebook_append_page(GTK_NOTEBOOK(music_manager), scrolled, combobox);

    return music_manager;
}

static GtkWidget *idou_music_view_new()
{
    GtkTreeStore *tree_store = gtk_tree_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkTreeIter iter;
    gtk_tree_store_append(tree_store, &iter, NULL);
    gtk_tree_store_set(tree_store, &iter, 0, "亏欠一生", 1, "裘海正", 2, "3:00", -1);

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

    return tree_view;
}

static void on_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer data)
{

}
