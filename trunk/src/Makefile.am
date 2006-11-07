# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/src/configuration $(all_includes)

# these are the headers for your project
noinst_HEADERS = kslovar.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kslovar.pot

KDE_ICON = AUTO

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kslovar

# the application source, library search path, and link libraries
kslovar_SOURCES = main.cpp kslovar.cpp
kslovar_LDFLAGS = -no-undefined $(KDE_RPATH) $(all_libraries)
kslovar_LDADD =	$(SQLITE3_LIB) $(top_builddir)/src/misc/libmisc.la $(top_builddir)/src/handler/libhandler.la \
	$(top_builddir)/src/dialog/libdialog.la $(top_builddir)/src/configuration/libconfiguration.la -lkdeprint \
	-lkhtml $(LIB_KDEUI)

#if DO_NOT_COMPILE_SQLITE
#kslovar_LDADD += -lsqlite3
#else
#kslovar_LDADD += $(top_builddir)/src/sqlite/libsqlite.la
#endif

# this is where the desktop file will go 
shelldesktopdir = $(kde_appsdir)/Utilities
shelldesktop_DATA = kslovar.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/kslovar
shellrc_DATA = kslovarui.rc splash.png

mimedir = $(kde_mimedir)/application
mime_DATA = vnd.kslovar.ksd.desktop vnd.kslovar.ksl.desktop vnd.kslovar.scmd.desktop vnd.kslovar.ldft.desktop
EXTRA_DIST = $(mime_DATA)







#SUBDIRS = configuration handler misc dialog styles
if DO_NOT_COMPILE_SQLITE
SUBDIRS = configuration handler misc dialog styles
else
SUBDIRS = configuration handler sqlite misc dialog styles
endif
