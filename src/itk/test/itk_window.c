#include <math.h>
#include "itk_window.h"

G_DEFINE_TYPE(ItkWindow, itk_window, GTK_TYPE_WINDOW);

static gboolean itk_window_draw(GtkWidget *widget, cairo_t *cr, gpointer data);
static gboolean on_window_shadow_draw(GtkWidget *widget, cairo_t *cr, gpointer data);
static gboolean on_window_frame_draw(GtkWidget *widget, cairo_t *cr, gpointer data);
static void on_window_frame_size_allocate(GtkWidget *widget, GtkAllocation *allocation, gpointer data);
static void draw_round_rectangle(cairo_t *cr, gint x, gint y, gint width, gint height, gint r);
static void draw_radial_round(cairo_t *cr, gint x, gint y, gint r);
static void draw_hlinear(cairo_t *cr, gint x, gint y, gint w, gint h, gboolean left_to_right);
static void draw_vlinear(cairo_t *cr, gint x, gint y, gint w, gint h, gboolean top_to_bottom);
static gboolean on_motion_notify(GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean get_window_cursor(gdouble x, gdouble y, gint width, gint height, gint shadow_radius, 
                                gint shadow_padding, GdkCursorType *cursor);
static gboolean get_edge(GdkCursorType cursor, GdkWindowEdge *edge);

void itk_window_class_init(ItkWindowClass *class)
{
}

void itk_window_init(ItkWindow *self)
{
    GtkWidget *window = GTK_WIDGET(self);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
    gtk_widget_set_app_paintable(window, TRUE);

    self->shadow_radius = 10;
    self->frame_radius = 2;
    self->shadow_padding = self->shadow_radius - self->frame_radius;
    GtkWidget *window_shadow = gtk_alignment_new(0.0, 0.0, 1.0, 1.0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(window_shadow), self->shadow_padding, self->shadow_padding, self->shadow_padding, self->shadow_padding);

    self->window_frame = gtk_alignment_new(0.0, 0.0, 1.0, 1.0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(self->window_frame), 2, 2, 2, 2);

    gtk_container_add(GTK_CONTAINER(window_shadow), self->window_frame);
    gtk_container_add(GTK_CONTAINER(window), window_shadow);

    g_signal_connect(G_OBJECT(self->window_frame), "draw", G_CALLBACK(on_window_frame_draw), NULL);
    g_signal_connect(G_OBJECT(window_shadow), "draw", G_CALLBACK(on_window_shadow_draw), (gpointer)self);
    g_signal_connect(G_OBJECT(window), "draw", G_CALLBACK(itk_window_draw), NULL);

    gtk_widget_add_events(window, GDK_POINTER_MOTION_MASK |
                                  GDK_BUTTON_PRESS_MASK);

    g_signal_connect(G_OBJECT(window), "motion-notify-event", G_CALLBACK(on_motion_notify), NULL);
    g_signal_connect(G_OBJECT(window), "button-press-event", G_CALLBACK(on_button_press), NULL);

    GdkScreen *screen = gdk_screen_get_default();
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    gtk_widget_set_visual(window, visual);
    g_object_unref(visual);
}

GtkWidget *itk_window_new()
{
    g_object_new(ITK_TYPE_WINDOW, NULL);
}

void itk_window_show(GtkWidget *window)
{
    gtk_widget_show_all(window);
}

void itk_window_add(ItkWindow *self, GtkWidget *child)
{
    gtk_container_add(GTK_CONTAINER(self->window_frame), child);
    gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void draw_rounded_rectangle(cairo_t *cr, gint width, gint height, gint r)
{
    cairo_move_to(cr, r, 0);
    cairo_line_to(cr, width-r, 0);

	cairo_move_to(cr, width, r);
	cairo_line_to(cr, width, height-r);

	cairo_move_to(cr, width-r, height);
	cairo_move_to(cr, r, height);

	cairo_move_to(cr, 0, height-r);
	cairo_line_to(cr, 0, r);

	cairo_arc(cr, r, r, r, G_PI, 3 * G_PI / 2.0 );
	cairo_arc(cr, width-r, r, r, 3 * G_PI /2.0, 2 * G_PI);
	cairo_arc(cr, width-r, height-r, r, 0, G_PI / 2);
	cairo_arc(cr, r, height-r, r, G_PI / 2, G_PI);
}

static void draw_window_shadow(cairo_t *cr, gint x, gint y, gint w, gint h, gint r, gint p)
{
    cairo_save(cr);
    cairo_rectangle(cr, x, y, r - 1, r - 1);
    cairo_rectangle(cr, x + r - 1, y, 1, r - 2);
    cairo_rectangle(cr, x, y + r - 1, r - 2, 1);
    
    cairo_rectangle(cr, x + w - r + 1, y, r - 1, r - 1);
    cairo_rectangle(cr, x + w - r, y, 1, r - 2);
    cairo_rectangle(cr, x + w - r + 2, y + r - 1, r - 2, 1);
    
    cairo_rectangle(cr, x, y + h - r + 1, r - 1, r - 1);
    cairo_rectangle(cr, x + r - 1, y + h - r + 2, 1, r - 2);
    cairo_rectangle(cr, x, y + h - r, r - 2, 1);
    
    cairo_rectangle(cr, x + w - r + 1, y + h - r + 1, r - 1, r - 1);
    cairo_rectangle(cr, x + w - r, y + h - r + 2, 1, r - 2);
    cairo_rectangle(cr, x + w - r + 2, y + h - r, r - 2, 1);
    cairo_clip(cr);
    
    // Draw four round.
    draw_radial_round(cr, x + r, y + r, r);
    draw_radial_round(cr, x + r, y + h - r, r);
    draw_radial_round(cr, x + w - r, y + r, r);
    draw_radial_round(cr, x + w - r, y + h - r, r);
    cairo_restore(cr);
    
    cairo_save(cr);
    // Clip four side.
    cairo_rectangle(cr, x, y + r, p, h - r * 2);
    cairo_rectangle(cr, x + w - p, y + r, p, h - r * 2);
    cairo_rectangle(cr, x + r, y, w - r * 2, p);
    cairo_rectangle(cr, x + r, y + h - p, w - r * 2, p);
    cairo_clip(cr);
    
    // Draw four side.
    draw_vlinear(
        cr, 
        x + r, y, 
        w - r * 2, r, TRUE);
    draw_vlinear(
        cr, 
        x + r, y + h - r, 
        w - r * 2, r, FALSE);
    draw_hlinear(
        cr, 
        x, y + r, 
        r, h - r * 2, TRUE);
    draw_hlinear(
        cr, 
        x + w - r, y + r, 
        r, h - r * 2, FALSE);
    cairo_restore(cr);
}

static void draw_radial_round(cairo_t *cr, gint x, gint y, gint r)
{
    cairo_pattern_t *radial = cairo_pattern_create_radial(x, y, r, x, y, 0);
    cairo_pattern_add_color_stop_rgba(radial, 0, 0, 0, 0, 0);
    cairo_pattern_add_color_stop_rgba(radial, 0.66, 0, 0, 0, 0.2);
    cairo_pattern_add_color_stop_rgba(radial, 1, 0, 0, 0, 0.33);
    cairo_arc(cr, x, y, r, 0, 2 * G_PI);
    cairo_set_source(cr, radial);
    cairo_fill(cr);

    cairo_pattern_destroy(radial);
}

static void draw_vlinear(cairo_t *cr, gint x, gint y, gint w, gint h, gboolean top_to_buttom)
{
    gint r = 0;
    cairo_save(cr);
    // Translate y coordinate, otherwise y is too big for LinearGradient cause render bug.
    cairo_translate(cr, 0, y);
    cairo_pattern_t *linear = NULL;
    if(top_to_buttom)
        linear = cairo_pattern_create_linear(0, 0, 0, h);
    else
        linear = cairo_pattern_create_linear(0, h, 0, 0);

    cairo_pattern_add_color_stop_rgba(linear, 0, 0, 0, 0, 0);
    cairo_pattern_add_color_stop_rgba(linear, 0.66, 0, 0, 0, 0.2);
    cairo_pattern_add_color_stop_rgba(linear, 1, 0, 0, 0, 0.33);
        
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_source(cr, linear);
    draw_round_rectangle(cr, x, 0, w, h, r);
    cairo_fill(cr);
    cairo_restore(cr);
    cairo_pattern_destroy(linear);
}

static void draw_hlinear(cairo_t *cr, gint x, gint y, gint w, gint h, gboolean left_to_right)
{
    gint r = 0;
    cairo_save(cr);
    // Translate x coordinate, otherwise x is too big for LinearGradient cause render bug.
    cairo_pattern_t *linear;
    cairo_translate(cr, x, 0);
    if(left_to_right)    
        linear = cairo_pattern_create_linear(0, 0, w, 0);
    else
        linear = cairo_pattern_create_linear(w, 0, 0, 0);

    cairo_pattern_add_color_stop_rgba(linear, 0, 0, 0, 0, 0);
    cairo_pattern_add_color_stop_rgba(linear, 0.66, 0, 0, 0, 0.2);
    cairo_pattern_add_color_stop_rgba(linear, 1, 0, 0, 0, 0.33);

    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_source(cr, linear);
    draw_round_rectangle(cr, 0, y, w, h, r);
    cairo_fill(cr);
    cairo_restore(cr);

    cairo_pattern_destroy(linear);
}

static void draw_round_rectangle(cairo_t *cr, gint x, gint y, gint width, gint height, gint r)
{
    // Adjust coordinate when width and height is negative.
    if(width < 0)
    {
        x = x + width;
        width = -width;
    } 

    if(height < 0)
    {
        y = y + height;
        height = -height;
    }
    
    // Top side.
    cairo_move_to(cr, x + r, y);
    cairo_line_to(cr, x + width - r, y);
    // Top-right corner.
    cairo_arc(cr, x + width - r, y + r, r, G_PI * 3 / 2, G_PI * 2);
    // Right side.
    cairo_line_to(cr, x + width, y + height - r);
    // Bottom-right corner.
    cairo_arc(cr, x + width - r, y + height - r, r, 0, G_PI / 2);
    // Bottom side.
    cairo_line_to(cr, x + r, y + height);
    
    // Bottom-left corner.
    cairo_arc(cr, x + r, y + height - r, r, G_PI / 2, G_PI);
    
    // Left side.
    cairo_line_to(cr, x, y + r);
    // Top-left corner.
    cairo_arc(cr, x + r, y + r, r, G_PI, G_PI * 3 / 2);

    // Close path.
    cairo_close_path(cr);
}

static gboolean on_window_shadow_draw(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    ItkWindow *self = ITK_WINDOW(data);

    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);
    draw_window_shadow(cr, alloc.x, alloc.y, alloc.width, alloc.height, self->shadow_radius, self->shadow_padding);

    return FALSE;
}

static gboolean itk_window_draw(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    ItkWindow *self = ITK_WINDOW(widget);
    GtkAllocation alloc;
    gint x, y, w, h, r;
    gtk_widget_get_allocation(widget, &alloc);
    x = alloc.x;
    y = alloc.y;
    w = alloc.width;
    h = alloc.height;
    r = self->shadow_radius;

    // Set window transparent
    cairo_set_source_rgba(cr, 0, 0, 0, 0);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);

    // Draw background
    cairo_set_source_rgb(cr, 0.93, 0.93, 0.93);
    cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);

    cairo_rectangle(cr, x + r - 1, y + r - 2, w - 2 * r + 2, 1);
    cairo_rectangle(cr, x + r - 1, y + h - r + 1, w - 2 * r + 2, 1);
    cairo_rectangle(cr, x + r - 2, y + r - 1, 1, h - 2 * r + 2);
    cairo_rectangle(cr, x + w + 1 - r, y + r - 1, 1, h - 2 * r + 2);
    cairo_rectangle(cr, x + r -1, y + r - 1, w - 2 * r + 2, h - 2 * r + 2);

    cairo_fill(cr);

    return FALSE;
}

static gboolean on_window_frame_draw(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    GtkAllocation alloc;
    gint x, y, w, h;

    gtk_widget_get_allocation(widget, &alloc);
    x = alloc.x;
    y = alloc.y;
    w = alloc.width;
    h = alloc.height;

    cr = gdk_cairo_create(gtk_widget_get_window(widget));
    cairo_antialias_t antialias = cairo_get_antialias(cr);
    cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);

    // Set line width.
    cairo_set_line_width(cr, 1);
    
    // Set OPERATOR_OVER operator.
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    
    // Draw outside 8 points.
    cairo_set_source_rgba(cr, 0, 0, 0, 0.4);
    
    cairo_rectangle(cr, x, y + 1, 1, 1); // top-left
    cairo_rectangle(cr, x + 1, y, 1, 1);
    
    cairo_rectangle(cr, x + w - 1, y + 1, 1, 1); // top-right
    cairo_rectangle(cr, x + w - 2, y, 1, 1);
    
    cairo_rectangle(cr, x, y + h - 2, 1, 1); // bottom-left
    cairo_rectangle(cr, x + 1, y + h - 1, 1, 1);
    
    cairo_rectangle(cr, x + w - 1, y + h - 2, 1, 1); // bottom-right
    cairo_rectangle(cr, x + w - 2, y + h - 1, 1, 1);
    
    cairo_fill(cr);
    
    // Draw outside 4 points.
    cairo_set_source_rgba(cr, 0, 0, 0, 0.4);
    
    cairo_rectangle(cr, x + 1, y + 1, 1, 1); // top-left
    cairo_rectangle(cr, x + w - 2, y + 1, 1, 1); // top-right
    
    cairo_rectangle(cr, x + 1, y + h - 2, 1, 1); // bottom-left
    cairo_rectangle(cr, x + w - 2, y + h - 2, 1, 1); // bottom-right
    
    cairo_fill(cr);
    
    // Draw outside frame.
    cairo_set_source_rgba(cr, 0, 0, 0, 0.45);

    cairo_rectangle(cr, x + 2, y, w - 4, 1); // top side
    cairo_rectangle(cr, x + 2, y + h - 1, w - 4, 1); // bottom side

    cairo_rectangle(cr, x, y + 2, 1, h - 4); // left side
    cairo_rectangle(cr, x + w - 1, y + 2, 1, h - 4); // right side
    
    cairo_fill(cr);
    
    // Draw outside 4 points.
    cairo_set_source_rgba(cr, 1, 1, 1, 0.2);
    
    cairo_rectangle(cr, x + 1, y + 1, 1, 1); // top-left
    cairo_rectangle(cr, x + w - 2, y + 1, 1, 1); // top-right
    
    cairo_rectangle(cr, x + 1, y + h - 2, 1, 1); // bottom-left
    cairo_rectangle(cr, x + w - 2, y + h - 2, 1, 1); // bottom-right
    
    cairo_fill(cr);
    
    // Draw inside 4 points.
    cairo_set_source_rgba(cr, 1, 1, 1, 0.2);
    
    cairo_rectangle(cr, x + 2, y + 2, 1, 1); // top-left
    cairo_rectangle(cr, x + w - 3, y + 2, 1, 1); // top-right
    
    cairo_rectangle(cr, x + 2, y + h - 3, 1, 1); // bottom-left
    cairo_rectangle(cr, x + w - 3, y + h - 3, 1, 1); // bottom-right
    
    cairo_fill(cr);
    
    // Draw inside frame.
    cairo_set_source_rgba(cr, 1, 1, 1, 0.6);

    cairo_rectangle(cr, x + 2, y + 1, w - 4, 1); // top side
    cairo_rectangle(cr, x + 2, y + h - 2, w - 4, 1); // bottom side
    
    cairo_rectangle(cr, x + 1, y + 2, 1, h - 4); // left side
    cairo_rectangle(cr, x + w - 2, y + 2, 1, h - 4); // right side
    cairo_fill(cr);

    //cairo_set_antialias(cr, antialias);

    cairo_destroy(cr);
}

static void on_window_frame_size_allocate(GtkWidget *widget, GtkAllocation *allocation, gpointer data)
{
    ItkWindow *self = ITK_WINDOW(data);
    gint x, y, width, height;
    x = allocation->x;
    y = allocation->y;
    width = allocation->width;
    height = allocation->height;
    g_message("x:%d,y:%d,w:%d,h:%d\n", x, y, width, height);
    gtk_widget_set_allocation(widget, allocation);

    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t *cr = cairo_create(surface);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);

    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

    GdkWindowState state = gdk_window_get_state(gtk_widget_get_window(GTK_WIDGET(self)));
    if(state & GDK_WINDOW_STATE_MAXIMIZED & GDK_WINDOW_STATE_FULLSCREEN)
    {
        cairo_rectangle(cr, x, y, width, height);
    }
    else
    {
        cairo_rectangle(cr, x+1, y, width-2, 1);
        cairo_rectangle(cr, x, y+1, width, height-2);
        cairo_rectangle(cr, x+1, y+height-1, width-2, 1);
                    /*cr.rectangle(x + 1, y, w - 2, 1)*/
                    /*cr.rectangle(x, y + 1, w, h - 2)*/
                    /*cr.rectangle(x + 1, y + h - 1, w - 2, 1)*/
    }
    cairo_fill(cr);
    cairo_destroy(cr);

    //cairo_region_t *region = gdk_cairo_region_create_from_surface(surface);
    //gtk_widget_shape_combine_region(GTK_WIDGET(self), region);
}

static gboolean on_motion_notify(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    ItkWindow *self = ITK_WINDOW(widget);
    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);
    if (event->type == GDK_MOTION_NOTIFY)
    {
        get_window_cursor(event->x, event->y, alloc.width, alloc.height, 
                        self->shadow_radius, self->shadow_padding, 
                        &self->cursor_type);

        GdkCursor *cursor = gdk_cursor_new(self->cursor_type);
        gdk_window_set_cursor(gtk_widget_get_window(widget), cursor);
        g_object_unref(cursor);

    }
    return FALSE;
}

static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    ItkWindow *self = ITK_WINDOW(widget);
    if (event->type == GDK_BUTTON_PRESS && event->button == GDK_BUTTON_PRIMARY)
    {
        GdkWindowEdge edge;
        if(get_edge(self->cursor_type, &edge))
        {
            gtk_window_begin_resize_drag(GTK_WINDOW(widget), 
                                        edge,
                                        event->button, 
                                        event->x_root, 
                                        event->y_root, 
                                        event->time);
        }
    }
}

static gboolean get_window_cursor(gdouble x, gdouble y, gint width, gint height, 
                                gint shadow_radius, gint shadow_padding, 
                                GdkCursorType *cursor)
{
    gboolean is_edge = TRUE;
    if (x <= shadow_padding)
    {
        if (y >= shadow_radius && y <= height - shadow_radius)
        {
            /**edge = GDK_WINDOW_EDGE_WEST;*/
            *cursor = GDK_LEFT_SIDE;
            return TRUE;
        }
    }
    else if (x >= width - shadow_padding)
    {
        if (y >= shadow_radius && y <= height - shadow_radius)
        {
            /**edge = GDK_WINDOW_EDGE_EAST;*/
            *cursor = GDK_RIGHT_SIDE;
            return TRUE;
        }
    }
    else if (y <= shadow_padding)
    {
        if (x >= shadow_radius && x <= width - shadow_radius)
        {
            /**edge = GDK_WINDOW_EDGE_NORTH;*/
            *cursor = GDK_TOP_SIDE;
            return TRUE;
        }
    }
    else if (y >= height - shadow_padding)
    {
        if (x >= shadow_radius && x <= width - shadow_radius)
        {
            /**edge = GDK_WINDOW_EDGE_SOUTH;*/
            *cursor = GDK_BOTTOM_SIDE;
            return TRUE;
        }
    }
    else
    {
        is_edge = FALSE;
    }

    if (!is_edge)
    {
        if (x <= shadow_radius)
        {
            if (y <= shadow_radius)
            {
                /*gdouble radius;*/
                /*radius = sqrt(pow(x-shadow_radius, 2.0) + pow(y-shadow_radius, 2.0));*/
                /*if (radius >= shadow_radius - shadow_padding)*/
                /*{*/
                    /*g_message("%lf, %lf, %d, %d, %lf", x, y, shadow_radius, shadow_padding, radius);*/
                /*}*/
                *cursor = GDK_TOP_LEFT_CORNER;
                return TRUE;
            }
            else if (y >= height - shadow_radius)
            {
                /*gdouble radius;*/
                /*radius = sqrt(pow(x-shadow_radius, 2.0) + pow(y+height-shadow_radius, 2.0));*/
                /*if (radius >= shadow_radius - shadow_padding)*/
                /*{*/
                /*}*/
                *cursor = GDK_BOTTOM_LEFT_CORNER;
                return TRUE;
            }
        }
        else if (x >= width - shadow_radius)
        {
            if (y <= shadow_radius)
            {
                /*gdouble radius;*/
                /*radius = sqrt(pow(x-width+shadow_radius, 2.0) + pow(y-shadow_radius, 2.0));*/
                /*if (radius >= shadow_radius - shadow_padding)*/
                /*{*/
                /*}*/
                *cursor = GDK_TOP_LEFT_CORNER;
                return TRUE;
            }
            else if (y >= height - shadow_radius)
            {
                /*gdouble radius;*/
                /*radius = sqrt(pow(x-width+shadow_radius, 2.0) + pow(y-height+shadow_radius, 2.0));*/
                /*if (radius >= shadow_radius - shadow_padding)*/
                /*{*/
                /*}*/
                *cursor = GDK_BOTTOM_RIGHT_CORNER;
                return TRUE;
            }
        }
    }
    *cursor = GDK_LEFT_PTR;
    return FALSE;
}

static gboolean get_edge(GdkCursorType cursor, GdkWindowEdge *edge)
{
    switch (cursor)
    {
    case GDK_TOP_LEFT_CORNER:
        *edge = GDK_WINDOW_EDGE_NORTH_WEST;
        return TRUE;
    case GDK_TOP_SIDE:
        *edge = GDK_WINDOW_EDGE_NORTH;
        return TRUE;
    case GDK_TOP_RIGHT_CORNER:
        *edge = GDK_WINDOW_EDGE_NORTH_EAST; 
        return TRUE;
    case GDK_LEFT_SIDE:
        *edge = GDK_WINDOW_EDGE_WEST;
        return TRUE;
    case GDK_RIGHT_SIDE:
        *edge = GDK_WINDOW_EDGE_EAST;
        return TRUE;
    case GDK_BOTTOM_SIDE:
        *edge = GDK_WINDOW_EDGE_SOUTH;
        return TRUE;
    case GDK_BOTTOM_LEFT_CORNER:
        *edge = GDK_WINDOW_EDGE_SOUTH_WEST;
        return TRUE;
    case GDK_BOTTOM_RIGHT_CORNER:
        *edge = GDK_WINDOW_EDGE_SOUTH_EAST;
        return TRUE;
    default:
        break;
    }
    return FALSE;
}
