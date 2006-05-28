INCLUDES = -I$(top_srcdir)/src $(all_includes) $(LIBXML_CFLAGS) \
	$(LIBXSLT_CFLAGS)
METASOURCES = AUTO
noinst_LTLIBRARIES = libhandler.la

libhandler_la_SOURCES = ksxslhandler.cpp ksxmlhandler.cpp 	ksoutputhandler.cpp \
	ksdbhandler.cpp
noinst_HEADERS = ksxslhandler.h ksxmlhandler.h ksoutputhandler.h ksdbhandler.h


libhandler_la_LIBADD = $(LIBXML_LIBS) $(LIBXSLT_LIBS)
