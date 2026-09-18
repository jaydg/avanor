##############################################################################
# Compiling Avanor: make {debug=1} {notcurses=1}                             #
#                                                                            #
# Just typing 'make' builds the release version for the host platform - the  #
# Windows/*nix split is detected from the environment, not a flag: $(OS) is  #
# Windows_NT on any native Windows shell, MSYS2's UCRT64/MINGW64/CLANG64     #
# included. Define xmingw=1 when cross-compiling a Windows binary from *nix  #
# with the MinGW-w64 crosscompiler - everything else about the build is      #
# identical either way.                                                      #
# Define debug=1 when you want to build debug version of Avanor              #
#                                                                            #
# The terminal is driven by plain ANSI escape sequences, on the header-only  #
# stc.hpp plus the platform's own keyboard reading - Win32 console calls on  #
# Windows, termios and poll() everywhere else. Nothing beyond fmt, LuaJIT    #
# and zstd has to be found, which is what keeps the game easy to build and   #
# to hand to somebody.                                                       #
#                                                                            #
# Define notcurses=1 to build against notcurses instead, at the price of a   #
# shared library to find, package and ship. What it buys is its own          #
# terminal capability negotiation, and a live resize on Windows, where the   #
# plain backend has no way to hear about one - everywhere else SIGWINCH      #
# tells it and both handle resizing alike. See the USE_NOTCURSES-guarded     #
# code in engine/global.cpp for exactly where the two differ.                #
#                                                                            #
# Both backends build the same avanor out of the same obj/, and nothing in   #
# a source file says which one built it, so switching between them needs a   #
# 'make clean' first. Without one make finds nothing to do and leaves the    #
# binary already standing, whichever backend that was.                       #
#                                                                            #
# argparse and stc.hpp are both header-only. stc.hpp is not packaged         #
# anywhere, and argparse is missing from MSYS2's mingw environments (it is   #
# on Fedora/Debian/Ubuntu, at least), so the build fetches whichever it      #
# needs with wget into external/ on demand, pinned to the versions below,    #
# so it stays reproducible without a manual install step.                    #
#                                                                            #
# There are also targets for making tarballs with the sources and binaries.  #
# The source ones pack what is committed, named after the version the game   #
# reports, so `make source-gz` writes avanor-0.6.0-src.tar.gz. Override the  #
# name with VERSION=x.y.z, and ask for it with `make -s version`.            #
##############################################################################

# What the archives are named after. The version is not asked of the
# version control system - the tags this history carries are not ancestors
# of anything current, so git describe has nothing to say - but read from
# the one place that already decides it: GAME_VERSION, which is what the
# binary prints for --version and what stands on the title screen. Pass
# VERSION=x.y.z to name an archive something else for once.
ifndef VERSION
	VERSION := $(shell sed -n 's/^#define GAME_VERSION "\(.*\)".*/\1/p' helpers/hiscore.h)

	# In a git checkout, a commit that no tag points at is not a release,
	# and both the archive name and the version the game reports say so by
	# carrying the commit they were built from. A tagged commit is left
	# alone, and so is a source archive, which has no repository to ask.
	#
	# --points-at rather than --contains: the question is whether *this*
	# commit was released, not whether some later tag has it in its
	# history - by that test every commit before a release counts as one.
	ifneq ($(wildcard .git),)
		GITREV := $(shell git log -n1 --format=%h)
		GITTAG := $(shell git tag --points-at HEAD)

		ifeq ($(GITTAG),)
			ifneq ($(GITREV),)
				VERSION := $(VERSION)-g$(GITREV)
				CFLAGS += -DGITREV=\"-g$(GITREV)\"
			endif
		endif
	endif
endif

DISTNAME := avanor-$(VERSION)

ifeq ($(OS),Windows_NT)
	win = 1
endif

# Homebrew installs outside the compiler's default search path, so the
# header-only packages - sol2, cereal, argparse - are invisible without
# saying where they are. pkg-config finds the rest by itself. The prefix
# is asked for rather than assumed: it is /opt/homebrew on Apple silicon
# and /usr/local on Intel.
ifeq ($(shell uname -s),Darwin)
	BREW_PREFIX := $(shell brew --prefix 2>/dev/null)

	ifneq ($(BREW_PREFIX),)
		CFLAGS += -isystem $(BREW_PREFIX)/include
		LDFLAGS += -L$(BREW_PREFIX)/lib
	endif
endif

CFLAGS += -std=c++17 -fsigned-char -pipe -Wall -Wextra -Werror -I.

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

# Every directory and binary any configuration of this Makefile produces.
# Taken now, with ':=', because $(OBJDIR) and $(NAME) pick up suffixes
# further down according to the flags this particular run was given -
# expanded later, 'make debug=1 clean' would go looking for obj-d-d.
# `clean` uses these so that it clears the lot however it is invoked.
ALL_OBJDIRS := $(OBJDIR) $(OBJDIR)-d
ALL_NAMES := $(NAME) $(NAME)-d $(NAME).exe $(NAME)-d.exe

ARGPARSE_VERSION = v3.2
ARGPARSE_HEADER = external/argparse/argparse.hpp
ARGPARSE_URL = https://raw.githubusercontent.com/p-ranav/argparse/$(ARGPARSE_VERSION)/include/argparse/argparse.hpp

# simple_term_colors has no tagged releases, so this pins a commit instead.
STC_VERSION = 85c2194fe861d411c9790ea5362953ff5ca1bcd0
STC_HEADER = external/stc.hpp
STC_URL = https://raw.githubusercontent.com/illyigan/simple_term_colors/$(STC_VERSION)/include/stc.hpp

# Whatever the build has to fetch for itself lands in external/ - stc.hpp
# always, argparse.hpp on Windows.
CFLAGS += -Iexternal $(shell pkg-config --cflags fmt luajit)
LIBS = $(shell pkg-config --libs fmt luajit) -lzstd

# The terminal backend, and the only thing that changes what has to be
# installed. See the notes at the top of this file.
ifdef notcurses
	CFLAGS += -DUSE_NOTCURSES $(shell pkg-config --cflags notcurses)
	LIBS += $(shell pkg-config --libs notcurses++)
endif

VPATH = creature engine game helpers item lua magic map player

SRCS = xlua.cpp api_actor.cpp api_world.cpp xweapon.cpp xtime.cpp xstring.cpp \
       xscheduler.cpp xscroll.cpp xring.cpp xpotion.cpp                       \
       xobject.cpp xmoney.cpp xmissileweapon.cpp xmissile.cpp xmapobj.cpp     \
       xhero_commands.cpp xhero_game.cpp xhero_input.cpp xhero_items.cpp      \
       xhero_sheet.cpp xhero.cpp xherb.cpp xguihtml.cpp xgui.cpp              \
       xgloves.cpp xgen.cpp xenhance.cpp xcorpse.cpp xclothes.cpp xcloak.cpp  \
       xcap.cpp xboots.cpp xbook.cpp xbaseobj.cpp xarmor.cpp xarchive.cpp     \
       xapi.cpp xanyplace.cpp xanyfood.cpp xamulet.cpp cskills.cpp            \
       shopkeeper.cpp keyword_dice.cpp std_ai.cpp lua_ai.cpp simulation.cpp   \
       xshield.cpp stats.cpp skills.cpp skill.cpp brand.cpp                   \
       skeep_ai.cpp shop.cpp resist.cpp rect.cpp windroad.cpp                 \
       quest.cpp map_objects.cpp msgwin.cpp fov.cpp modifiers.cpp map.cpp     \
       modifier.cpp manual.cpp Main.cpp pattern.cpp magic.cpp location.cpp    \
       itemf.cpp itemdb.cpp item_misc.cpp item.cpp hiscore.cpp                \
       global.cpp game.cpp effect.cpp dice.cpp deity.cpp cr_defs.cpp          \
       creature2.cpp creature.cpp dungeon_builder.cpp cave_builder.cpp        \
       pattern_builder.cpp chambers_builder.cpp plain_builder.cpp             \
       delve_builder.cpp bodypart.cpp anycr.cpp ai_view.cpp

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

.PHONY: all clean version source-zip source-gz binary-zip binary-gz

all: $(OBJDIR) $(NAME)

# What the archives will be named after, for a packaging script to read -
# `make -s version` prints it and nothing else.
version:
	@echo $(VERSION)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
	$(CXX) -MMD $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

$(ARGPARSE_HEADER):
	mkdir -p $(dir $@)
	wget -q -O $@ $(ARGPARSE_URL)

$(STC_HEADER):
	mkdir -p $(dir $@)
	wget -q -O $@ $(STC_URL)

ifdef win
$(OBJDIR)/Main.o: $(ARGPARSE_HEADER)
endif

# Main.o is where the version string is compiled in, and nothing in
# Main.cpp changes when the commit does - so after a commit, a rebuild
# would happily keep reporting the commit before it. This stamp holds the
# version and is rewritten only when that actually changes, which makes
# Main.o out of date exactly then and at no other time.
ifneq ($(GITREV),)
VERSION_STAMP := $(OBJDIR)/.version

$(OBJDIR)/Main.o: $(VERSION_STAMP)

$(VERSION_STAMP): FORCE | $(OBJDIR)
	@echo '$(VERSION)' > $@.new
	@cmp -s $@.new $@ || mv -f $@.new $@
	@$(RM) $@.new

FORCE:
.PHONY: FORCE
endif

ifndef notcurses
$(OBJDIR)/global.o: $(STC_HEADER)
endif

# Clears every configuration, not just the one named on the command line:
# a clean that left the other one behind is how stale objects survive to
# confuse the next build, and telling the two backends apart needs a full
# clean anyway. Leaves external/ alone - those are downloads, not build
# output, and throwing them away only means fetching them again.
clean:
	$(RM) $(addsuffix /*.o,$(ALL_OBJDIRS))
	$(RM) $(addsuffix /*.d,$(ALL_OBJDIRS))
	$(RM) $(addsuffix /.version,$(ALL_OBJDIRS))
	$(RM) $(ALL_NAMES)

# The source archives, one command each: git archive writes out what is
# committed, under a $(DISTNAME)/ prefix, already compressed - so there is
# no export into a scratch directory to tidy up afterwards, and nothing
# but git to have installed. What it packs is HEAD, not the working tree,
# so an archive never carries a change that was not committed.
source-zip:
	git archive --format=zip --prefix=$(DISTNAME)/ -o $(DISTNAME)-src.zip HEAD

source-gz:
	git archive --format=tar.gz --prefix=$(DISTNAME)/ -o $(DISTNAME)-src.tar.gz HEAD

binary-zip: all
	-$(RM) $(DISTNAME).zip
	upx --best $(NAME)
	7z a -tzip -mx $(DISTNAME).zip "$(NAME)" gpl.txt changes.txt "manual/*.html" "manual/*.css"

binary-gz: all
	-$(RM) $(DISTNAME).tar.gz
	tar -czf $(DISTNAME).tar.gz avanor gpl.txt changes.txt manual/*.html manual/*.css

-include $(DEPS)
