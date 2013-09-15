noinst_LTLIBRARIES = libgui.la

AM_CFLAGS = \
	-DRESDIR=\"$(pkgdatadir)/resource/\" \
	$(GTK3_CFLAGS) \
	$(GST_CFLAGS) \
	$(WEBKIT_CFLAGS) \
	-I../tools \
	-I../itk \
	-std=c99 \
	$(NULL)

AM_LDFLAGS = \
	$(GTK3_LIBS) \
	$(GST_LIBS) \
	$(WEBKIT_LIBS) \
	../tools/libtools.la \
	../itk/libitk.la \
	$(NULL)

libgui_la_SOURCES = \
	idou_titlebar.c \
	idou_theme_dialog.c \
	idou_combobox.c \
	idou_music_manager.c \
	idou_window.c \
	$(NULL)
