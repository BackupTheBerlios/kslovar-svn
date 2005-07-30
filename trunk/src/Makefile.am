# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = kslovar.h dbhandler.h createdictionary.h

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
kslovar_SOURCES = main.cpp kslovar.cpp dbhandler.cpp createdictionarydlg.ui \
	createdictionary.cpp
kslovar_LDFLAGS = $(KDE_RPATH) $(all_libraries)
kslovar_LDADD = -lsqlite3 -lkhtml $(LIB_KDEUI)

# this is where the desktop file will go 
shelldesktopdir   = $(kde_appsdir)/Utilities
shelldesktop_DATA = kslovar.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/kslovar
shellrc_DATA = kslovarui.rc

