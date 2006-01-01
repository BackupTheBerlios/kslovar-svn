INCLUDES = $(all_includes)
METASOURCES = AUTO
noinst_LTLIBRARIES = libdialog.la

libdialog_la_SOURCES = ksphrase.cpp ksdictionary.cpp kslanguage.cpp
noinst_HEADERS = ksphrase.h ksdictionary.h kslanguage.h

SUBDIRS= ui

libdialog_la_LDFLAGS = $(all_libraries)
libdialog_la_LIBADD = $(top_builddir)/src/dialog/ui/libui.la
