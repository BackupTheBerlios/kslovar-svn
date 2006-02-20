INCLUDES = $(all_includes)
METASOURCES = AUTO
noinst_LTLIBRARIES = libconfiguration.la
libconfiguration_la_SOURCES = configuration.kcfgc ksappearance.cpp \
	ksconfigdialog.cpp kswizard.cpp

kde_kcfg_DATA = kslovar.kcfg
noinst_HEADERS = ksconfigdialog.h ksappearance.h kswizard.h

SUBDIRS = ui


libconfiguration_la_LDFLAGS = $(all_libraries)
libconfiguration_la_LIBADD = $(top_builddir)/src/configuration/ui/libui.la
