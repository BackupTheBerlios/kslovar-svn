INCLUDES = $(all_includes)
METASOURCES = AUTO
libsqlite_la_LDFLAGS = $(LIBPTHREAD)
noinst_LTLIBRARIES = libsqlite.la
libsqlite_la_SOURCES = alter.c attach.c auth.c btree.c build.c date.c delete.c \
	expr.c func.c hash.c insert.c legacy.c main.c opcodes.c os_unix.c os_win.c \
	pager.c parse.c pragma.c prepare.c printf.c random.c select.c shell.c table.c \
	tokenize.c trigger.c update.c utf.c util.c vacuum.c vdbeapi.c vdbeaux.c vdbe.c \
	vdbemem.c where.c
noinst_HEADERS = btree.h hash.h keywordhash.h opcodes.h os_common.h os.h\
	os_unix.h os_win.h pager.h parse.h sqlite3.h sqliteInt.h vdbe.h\
	vdbeInt.h
