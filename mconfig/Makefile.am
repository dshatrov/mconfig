COMMON_CFLAGS =			\
	-ggdb			\
	-Wno-long-long -Wall    \
	$(THIS_CFLAGS)

if !PLATFORM_WIN32
    COMMON_CFLAGS += -pthread
    # For backtraces
    COMMON_CFLAGS += -rdynamic
endif

AM_CFLAGS   += -std=gnu99 $(COMMON_CFLAGS)
AM_CXXFLAGS += $(COMMON_CFLAGS)

INCLUDES = -I$(top_srcdir) -I$(top_builddir)

mconfig_private_headers =

# TODO There's no need to distribute mconfig_pargen.h
mconfig_target_headers =	\
	mconfig.h		\
	config.h		\
	config_parser.h		\
	mconfig_pargen.h

MCONFIG_GENHEADERS =		\
	mconfig_pargen.h
MCONFIG_GENSOURCES =		\
	mconfig_pargen.cpp
MCONFIG_GENFILES =		\
	$(MCONFIG_GENHEADERS)	\
	$(MCONFIG_GENSOURCES)
BUILT_SOURCES = mconfig_pargen.h
CLEANFILES = $(MCONFIG_GENFILES)

mconfig_pargen.h mconfig_pargen.cpp: mconfig.par
	pargen --extmode --module-name MConfig --header-name mconfig $(srcdir)/mconfig.par

mconfigincludedir = $(includedir)/mconfig-1.0/mconfig
mconfiginclude_HEADERS =	\
	$(mconfig_target_headers)

lib_LTLIBRARIES = libmconfig-1.0.la
libmconfig_1_0_la_SOURCES =		\
	mconfig.cpp			\
	config.cpp			\
	config_parser.cpp		\
	mconfig_pargen.cpp

mconfig_extra_dist =

libmconfig_1_0_la_LDFLAGS = -no-undefined -version-info "0:0:0"
libmconfig_1_0_la_LIBADD = $(THIS_LIBS)

EXTRA_DIST = $(mconfig_private_headers) $(mconfig_extra_dist)

