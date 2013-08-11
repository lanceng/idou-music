bin_PROGRAMS = iDouMusic
iDouMusic_SOURCES = idou_main.c
iDouMusic_LDADD = ../gui/libgui.la

AM_CFLAGS = \
	-DRESDIR=\"$(pkgdatadir)/resource/\" \
	-I../gui \
	$(GTK3_CFLAGS) \
	$(GST_LIBS)

AM_LDFLAGS = \
	$(GTK3_LIBS) \
	$(GST_LIBS)
	
