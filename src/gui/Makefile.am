noinst_LTLIBRARIES = libgui.la

AM_CFLAGS = \
	-DRESDIR=\"$(pkgdatadir)/resource/\" \
	$(GTK3_CFLAGS) \
	$(GST_CFLAGS) \
	-I../tools \
	-std=c99 \
	$(NULL)

AM_LDFLAGS = \
	$(GTK3_LIBS) \
	$(GST_LIBS) \
	../tools/libtools.la \
	$(NULL)

libgui_la_SOURCES = \
		idou_button.c \
		idou_titlebar.c \
		idou_titlebar2.c \
		idou_theme_dialog.c \
		idou_window.c \
		$(NULL)
