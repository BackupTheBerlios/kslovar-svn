INCLUDES = -I$(top_srcdir)/src $(all_includes) $(LIBXML_CFLAGS) \
	$(LIBXSLT_CFLAGS)
METASOURCES = AUTO
noinst_LTLIBRARIES = libhandler.la

libhandler_la_SOURCES = ksdbhandler.cpp ksxslhandler.cpp ksxmlhandler.cpp \
	ksoutputhandler.cpp
noinst_HEADERS = ksdbhandler.h ksxslhandler.h ksxmlhandler.h ksoutputhandler.h


libhandler_la_LIBADD = $(LIBXML_LIBS) $(LIBXSLT_LIBS)
