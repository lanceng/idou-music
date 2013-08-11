#ifndef GSTPLAY_H
#define GSTPLAY_H

#include <glib.h>

extern gpointer gst_play_thread_cb(gpointer);
extern gint64 gst_play_get_position();
extern gint64 gst_play_get_duration();

#endif
