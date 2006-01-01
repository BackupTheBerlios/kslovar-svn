INCLUDES = $(all_includes) $(LIBXML_CFLAGS) $(LIBXSLT_CFLAGS)
METASOURCES = AUTO
noinst_LTLIBRARIES = libhandler.la

libhandler_la_SOURCES = ksdbhandler.cpp ksxmlhandler.cpp
noinst_HEADERS = ksdbhandler.h ksxmlhandler.h


libhandler_la_LIBADD = $(LIBXML_LIBS) $(LIBXSLT_LIBS)
