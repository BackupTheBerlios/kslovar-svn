INCLUDES = $(all_includes)
METASOURCES = AUTO
noinst_LTLIBRARIES = libmisc.la

libmisc_la_SOURCES = ksdata.cpp
noinst_HEADERS = ksdata.h


SUBDIRS= widget

libmisc_la_LDFLAGS = $(all_libraries)

libmisc_la_LIBADD = $(top_builddir)/src/misc/widget/libwidget.la
