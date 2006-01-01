INCLUDES = $(all_includes)
METASOURCES = AUTO
noinst_LTLIBRARIES = libdialog.la

libdialog_la_SOURCES = addphrase.cpp createdictionary.cpp kslanguages.cpp
noinst_HEADERS = addphrase.h createdictionary.h kslanguages.h

SUBDIRS= ui

libdialog_la_LDFLAGS = $(all_libraries)
libdialog_la_LIBADD = $(top_builddir)/src/dialog/ui/libui.la
