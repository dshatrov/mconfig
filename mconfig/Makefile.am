COMMON_CFLAGS =				\
	-D_POSIX_C_SOURCE=199309L	\
	-D_XOPEN_SOURCE=600		\
	-ggdb				\
	-Wno-long-long -Wall -Wextra	\
	$(THIS_CFLAGS)
# -pedantic

COMMON_CFLAGS += -DLIBMARY_MTSAFE -DLIBMARY_TLOCAL=__thread

if PLATFORM_WIN32
    COMMON_CFLAGS += -DPLATFORM_WIN32
else
    # For backtraces
    COMMON_CFLAGS += -rdynamic -pthread
endif

AM_CFLAGS = -std=c99 $(COMMON_CFLAGS)
AM_CXXFLAGS = -std=c++0x $(COMMON_CFLAGS)

if PLATFORM_WIN32
else
# <fstream> doesn't work in strict ANSI mode with MinGW.
# Upd: Variadic macros produce warnings with -ansi.
#AM_CXXFLAGS += -std=c++98 -ansi
endif

INCLUDES = -I$(top_srcdir)

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
	pargen --extmode --module-name MConfig --header-name mconfig $^

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

