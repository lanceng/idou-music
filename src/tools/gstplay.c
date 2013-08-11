#include <gst/gst.h>
#include <glib.h>

typedef struct _GstPlay {
    GMainContext *context;
    GMainLoop *loop;
    GstElement *pipeline;
}GstPlay;

static GstPlay *gst_play;
static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data);

gpointer gst_play_thread_cb(gpointer *data)
{
    GMainContext *context;
    GMainLoop *loop;
    gchar *filename = (gchar*)data;
    GstElement *pipeline, *source, *decoder, *sink;
    GstBus *bus;

    gst_init(NULL, NULL);

    gst_play = g_slice_new0(GstPlay);

    gst_play->context = g_main_context_new();
    gst_play->loop = g_main_loop_new(context, FALSE);
    gst_play->pipeline = gst_pipeline_new("audio-player");

    context = gst_play->context;
    loop = gst_play->loop;
    pipeline = gst_play->pipeline;

    source = gst_element_factory_make("filesrc", "file-source");
    decoder = gst_element_factory_make("mad", "mad-decoder");
    sink = gst_element_factory_make("autoaudiosink", "audio-output");

    if(!pipeline || !source || !decoder || !sink)
    {
        g_error("Gst Error!");
        exit(-1);
    }

    g_object_set(G_OBJECT(source), "location", filename, NULL);
    bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_watch(bus, bus_call, loop);
    gst_object_unref(bus);

    gst_bin_add_many(GST_BIN(pipeline), source, decoder, sink, NULL);
    gst_element_link_many(source, decoder, sink, NULL);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    g_message("Running...");
    g_main_loop_run(loop);
}

void gst_play_quit()
{
    gst_element_set_state(gst_play->pipeline, GST_STATE_NULL);
    g_main_context_unref(gst_play->context);
    gst_object_unref(GST_OBJECT(gst_play->pipeline));
    g_main_loop_quit(gst_play->loop);
}

gint64 gst_play_get_position()
{
    GstFormat fmt = GST_FORMAT_TIME;
    gint64 pos;

    gst_element_query_position(GST_ELEMENT(gst_play->pipeline), &fmt, &pos);
    return pos;
}

gint64 gst_play_get_duration()
{
    GstFormat fmt = GST_FORMAT_TIME;
    gint64 dur;

    gst_element_query_duration(GST_ELEMENT(gst_play->pipeline), &fmt, &dur);
    return dur;
}

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data)
{
    GMainLoop *loop = (GMainLoop*)data;
    switch(GST_MESSAGE_TYPE(msg))
    {
        case GST_MESSAGE_EOS:
            g_main_loop_quit(loop);
            break;
        case GST_MESSAGE_ERROR:
            g_main_loop_quit(loop);
            break;
        default:
            break;
    }
    return TRUE;
}
