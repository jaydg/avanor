##############################################################################
# Compiling Avanor: make {debug=1}                                           #
#                                                                            #
# Just typing 'make' builds the release version for the host platform - the  #
# Windows/*nix split (native MinGW-w64 vs. notcurses-on-*nix's own build)    #
# is detected from the environment, not a flag: $(OS) is Windows_NT on any   #
# native Windows shell, MSYS2's UCRT64/MINGW64/CLANG64 included.             #
# Define xmingw=1 when cross-compiling a Windows binary from *nix with the   #
# MinGW-w64 crosscompiler - everything else about the build is identical     #
# either way: same notcurses/fmt/luajit/zstd stack via pkg-config.           #
# Define debug=1 when you want to build debug version of Avanor              #
#                                                                            #
# Define stc=1 to swap notcurses for a plain-ANSI terminal backend built on  #
# the header-only stc.hpp, for a Windows terminal where notcurses' ConPTY/   #
# terminfo negotiation doesn't come good (garbage escapes, or a crash).      #
# Windows-only - it's built on conio.h and the Win32 console API. See        #
# engine/global.cpp's USE_STC-guarded code for what it can't do that the     #
# notcurses backend can (live resize, mainly).                               #
#                                                                            #
# argparse and stc.hpp are both header-only and not packaged for MSYS2's     #
# mingw environments (argparse is on Fedora/Debian/Ubuntu, at least) - a     #
# Windows/xmingw=1 build fetches whichever it needs with wget into           #
# external/ on demand, pinned to the versions below, so the build stays      #
# reproducible without a manual install step.                                #
#                                                                            #
# There are also targets for making tarballs with the sources and binaries   #
# Example:                                                                   #
#    Create tarball (avanor-0.5.7-src.tar.bz2) with the sources of the game  #
#    make VERSION=0.5.7 source-bz2                                           #
##############################################################################

ifdef VERSION
	DISTNAME := avanor-$(VERSION)
else
	DISTNAME := avanor-r${shell svnversion .}
endif

ifeq ($(OS),Windows_NT)
	win = 1
endif

CX = g++
CC = gcc
LD = g++
CFLAGS = -std=c++17 -fsigned-char -pipe -Wall -Wextra

ifdef xmingw
    CX = x86_64-w64-mingw32-g++
    CC = x86_64-w64-mingw32-gcc
    LD = x86_64-w64-mingw32-g++
    win = 1
endif

ifdef DATA_DIR
	CFLAGS += -DDATA_DIR=\"$(DATA_DIR)\"
endif

ifndef OPTFLAGS
	OPTFLAGS = -O2
endif

OBJDIR = obj
NAME = avanor

ARGPARSE_VERSION = v3.2
ARGPARSE_HEADER = external/argparse/argparse.hpp
ARGPARSE_URL = https://raw.githubusercontent.com/p-ranav/argparse/$(ARGPARSE_VERSION)/include/argparse/argparse.hpp

# simple_term_colors has no tagged releases, so this pins a commit instead.
STC_VERSION = 85c2194fe861d411c9790ea5362953ff5ca1bcd0
STC_HEADER = external/stc.hpp
STC_URL = https://raw.githubusercontent.com/illyigan/simple_term_colors/$(STC_VERSION)/include/stc.hpp

CFLAGS += -I.

ifdef win
	CFLAGS += -Iexternal
endif

ifdef stc
	CFLAGS += -DUSE_STC
	CFLAGS += $(shell pkg-config --cflags fmt luajit)

	ifndef win
		CFLAGS += -Iexternal
	endif
else
	CFLAGS += $(shell pkg-config --cflags fmt notcurses luajit)
endif

VPATH = creature engine game global helpers item magic map other lua

SRCS = xlua.cpp api_actor.cpp api_world.cpp xweapon.cpp xtime.cpp xstring.cpp \
       xscheduler.cpp xscroll.cpp xring.cpp xpotion.cpp                       \
       xobject.cpp xmoney.cpp xmissileweapon.cpp xmissile.cpp xmapobj.cpp     \
       xhero3.cpp xhero2.cpp xhero.cpp xherb.cpp xguihtml.cpp xgui.cpp        \
       xgloves.cpp xgen.cpp xenhance.cpp xcorpse.cpp xclothes.cpp xcloak.cpp  \
       xcap.cpp xboots.cpp xbook.cpp xbaseobj.cpp xarmor.cpp xarchive.cpp     \
       xapi.cpp xanyplace.cpp xanyfood.cpp xamulet.cpp wskills.cpp            \
       uniquei.cpp shopkeeper.cpp keyword_dice.cpp std_ai.cpp lua_ai.cpp      \
       xshield.cpp stats.cpp skills.cpp skill.cpp attack_effect_type.cpp      \
       skeep_ai.cpp shop.cpp setting.cpp resist.cpp rect.cpp windroad.cpp     \
       quest.cpp map_objects.cpp msgwin.cpp fov.cpp modifiers.cpp map.cpp     \
       modifier.cpp manual.cpp Main.cpp pattern.cpp magic.cpp location.cpp    \
       itemf.cpp itemdb.cpp item_misc.cpp item.cpp hiscore.cpp                \
       global.cpp game.cpp effect.cpp dice.cpp deity.cpp cr_defs.cpp          \
       creature2.cpp creature.cpp dungeon_builder.cpp cave_builder.cpp        \
       pattern_builder.cpp chambers_builder.cpp plain_builder.cpp             \
       delve_builder.cpp bodypart.cpp anycr.cpp ai_view.cpp

ifdef stc
	LIBS = $(shell pkg-config --libs fmt luajit) -lzstd
else
	LIBS = $(shell pkg-config --libs fmt notcurses++ luajit) -lzstd
endif

ifdef debug
	CFLAGS += -g
	OBJDIR := ${addsuffix -d,$(OBJDIR)}
	NAME := ${addsuffix -d,$(NAME)}
else
	CFLAGS += $(OPTFLAGS)
endif

ifdef win
	NAME := ${addsuffix .exe,$(NAME)}
endif

OBJS := $(SRCS:.cpp=.o)
OBJS := $(OBJS:.c=.o)
OBJS := $(addprefix $(OBJDIR)/,$(OBJS))
DEPS = $(OBJS:.o=.d)

##############################################################################

all: $(OBJDIR) $(NAME)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
	$(CX) -MMD $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.c
	$(CC) -MMD $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(LD) $(CFLAGS) -o $@ $^ $(LIBS)

$(ARGPARSE_HEADER):
	mkdir -p $(dir $@)
	wget -q -O $@ $(ARGPARSE_URL)

$(STC_HEADER):
	mkdir -p $(dir $@)
	wget -q -O $@ $(STC_URL)

ifdef win
$(OBJDIR)/Main.o: $(ARGPARSE_HEADER)
endif

ifdef stc
$(OBJDIR)/global.o: $(STC_HEADER)
endif

clean:
	$(RM) $(OBJDIR)/*.o
	$(RM) $(OBJDIR)/*.d
	$(RM) $(NAME)

source-zip:
# create zip archive with Avanor sources, requires subversion command line client
# and 7-zip archiver
	-$(RM) $(DISTNAME)-src.zip
	svn export . $(DISTNAME)
	7z a -tzip -r -mx $(DISTNAME)-src.zip "$(DISTNAME)/*"
	svn delete --force $(DISTNAME)

source-bz2:
# create tar.bz2 archive with Avanor sources (on *nix systems)
	-$(RM) $(DISTNAME)-src.tar.bz2
	svn export . $(DISTNAME)
	tar -cjf $(DISTNAME)-src.tar.bz2 $(DISTNAME)
	svn delete --force $(DISTNAME)

source-gz:
# create tar.gz archive with Avanor sources (on *nix systems)
	-$(RM) $(DISTNAME)-src.tar.gz
	svn export . $(DISTNAME)
	tar -czf $(DISTNAME)-src.tar.gz $(DISTNAME)
	svn delete --force $(DISTNAME)

binary-zip: all
	-$(RM) $(DISTNAME).zip
	upx --best $(NAME)
	7z a -tzip -mx $(DISTNAME).zip "$(NAME)" gpl.txt changes.txt "manual/*.html" "manual/*.css"

binary-gz: all
	-$(RM) $(DISTNAME).tar.gz
	tar -czf $(DISTNAME).tar.gz avanor gpl.txt changes.txt manual/*.html manual/*.css

-include $(DEPS)
