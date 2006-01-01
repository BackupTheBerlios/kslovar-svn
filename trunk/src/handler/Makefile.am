INCLUDES = $(all_includes) $(LIBXML_CFLAGS) $(LIBXSLT_CFLAGS)
METASOURCES = AUTO
noinst_LTLIBRARIES = libhandler.la

libhandler_la_SOURCES = dbhandler.cpp ksxmlhandler.cpp
noinst_HEADERS = dbhandler.h ksxmlhandler.h


libhandler_la_LIBADD = $(LIBXML_LIBS) $(LIBXSLT_LIBS)
