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

# VERSIONINFO in the Windows resource takes four plain numbers, so the
# release number is split up and any "-g<commit>" dropped - the full
# string still goes in as text, under FileVersion.
VERSION_NUMBER := $(firstword $(subst -, ,$(VERSION)))
VERSION_MAJOR := $(word 1,$(subst ., ,$(VERSION_NUMBER)))
VERSION_MINOR := $(word 2,$(subst ., ,$(VERSION_NUMBER)))
VERSION_PATCH := $(word 3,$(subst ., ,$(VERSION_NUMBER)))

ifeq ($(OS),Windows_NT)
	win = 1
endif

# Homebrew installs outside the compiler's default search path, so the
# header-only packages - sol2, cereal, argparse - are invisible without
# saying where they are. pkg-config finds the rest by itself. The prefix
# is asked for rather than assumed: it is /opt/homebrew on Apple silicon
# and /usr/local on Intel.
ifeq ($(shell uname -s),Darwin)
	macos = 1
	BREW_PREFIX := $(shell brew --prefix 2>/dev/null)

	ifneq ($(BREW_PREFIX),)
		CFLAGS += -isystem $(BREW_PREFIX)/include
		LDFLAGS += -L$(BREW_PREFIX)/lib
	endif
endif

CFLAGS += -std=c++17 -fsigned-char -pipe -Wall -Wextra -Werror -I.

WINDRES = windres

# Cross-compiling a Windows binary from *nix. CXX and WINDRES are what
# the rules below actually use - this block used to set CX/CC/LD, which
# nothing has read since the rules were changed, so xmingw=1 quietly
# built with the host compiler and produced a Linux binary called
# avanor.exe.
ifdef xmingw
    CXX = x86_64-w64-mingw32-g++
    WINDRES = x86_64-w64-mingw32-windres
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

	# The icon and the version information, compiled by windres and linked
	# in like any other object. The .rc takes its numbers from here so it
	# never has to be edited for a release; the backslashes survive make,
	# the shell and windres in that order, which is why there are so many.
	RESOURCE := $(OBJDIR)/avanor.res
	RESDEFINE := -DVERSION_MAJOR=$(VERSION_MAJOR)
	RESDEFINE += -DVERSION_MINOR=$(VERSION_MINOR)
	RESDEFINE += -DVERSION_PATCH=$(VERSION_PATCH)
	RESDEFINE += -DVINFO=\\\"$(VERSION)\\\"
endif

OBJS := $(SRCS:.cpp=.o)
OBJS := $(OBJS:.c=.o)
OBJS := $(addprefix $(OBJDIR)/,$(OBJS))
DEPS = $(OBJS:.o=.d)

##############################################################################

.PHONY: all clean version installer dmg source-zip source-gz binary-zip binary-gz

all: $(OBJDIR) $(NAME)

# What the archives will be named after, for a packaging script to read -
# `make -s version` prints it and nothing else.
version:
	@echo $(VERSION)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
	$(CXX) -MMD $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS) $(RESOURCE)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

# windres reads resources/avanor.rc, which names the .ico beside it - so
# it is run with that directory included, and rebuilt when either changes.
$(RESOURCE): resources/avanor.rc resources/avanor.ico | $(OBJDIR)
	$(WINDRES) $(RESDEFINE) -I resources $< -O coff -o $@

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
	$(RM) mainfiles.nsh datafiles.nsh
	$(RM) -r $(DMGROOT) resources/Avanor.icns

# The Windows installer. The two .nsh files are the lists of what to
# install, generated here rather than kept in step by hand: MAINFILES is
# what sits beside the executable, DATADIRS the trees that keep their
# shape. The DLLs are asked of the binary itself - which ones are needed
# depends on the backend it was built with, and a list written down here
# would be wrong the first time that changed.
INSTALLER := avanor-$(VERSION).exe
MAINFILES = $(NAME) COPYING CHANGELOG.md README.md
DATADIRS = world manual

mainfiles.nsh: $(NAME)
	@$(RM) $@
	@for file in $(MAINFILES); do echo "  File \"$$file\"" >> $@; done
	@dlls=$$(ldd $(NAME) | sed -n 's|.*=> \(/[^/]*/bin/[^ ]*\) .*|\1|p'); \
	if [ -z "$$dlls" ]; then \
		echo "$@: ldd named no libraries outside the system directories." >&2; \
		echo "An installer without them would install a game that cannot start," >&2; \
		echo "so this stops here. Check what 'ldd $(NAME)' prints." >&2; \
		$(RM) $@; \
		exit 1; \
	fi; \
	for dll in $$dlls; do echo "  File \"$$(cygpath -w $$dll)\"" >> $@; done

datafiles.nsh:
	@$(RM) $@
	@for dir in $(DATADIRS); do echo "  File /r \"$$dir\"" >> $@; done

installer: $(NAME) mainfiles.nsh datafiles.nsh avanor.nsi
	makensis //DVERSION="$(VERSION)" \
		//DVERSION_MAJOR=$(VERSION_MAJOR) \
		//DVERSION_MINOR=$(VERSION_MINOR) \
		//DVERSION_PATCH=$(VERSION_PATCH) avanor.nsi
	@$(RM) mainfiles.nsh datafiles.nsh

# The macOS disk image: an application bundle, and the documents beside
# it, wrapped in a .dmg with an Applications folder to drag it into.
#
# The bundle keeps the game in Contents/MacOS and its world and manual in
# Contents/Resources, which is the layout vEnterDataDir() knows to look
# for. What Finder runs is resources/avanor-launcher rather than the game
# itself - see the comment in that file.
#
# dylibbundler copies the Homebrew libraries the binary needs into the
# bundle and rewrites its load paths to point inside it, so the .dmg runs
# on a Mac that has never seen Homebrew.
DMG := avanor-$(VERSION).dmg
DMGROOT := dmgroot
APPDIR := $(DMGROOT)/Avanor.app

resources/Avanor.icns: resources/avanor.iconset
	iconutil -c icns -o $@ $<

dmg: $(NAME) resources/Avanor.icns
	$(RM) -r $(DMGROOT)
	mkdir -p $(APPDIR)/Contents/MacOS $(APPDIR)/Contents/Resources

	cp -p $(NAME) $(APPDIR)/Contents/MacOS/
	cp -p resources/avanor-launcher $(APPDIR)/Contents/MacOS/
	chmod +x $(APPDIR)/Contents/MacOS/avanor-launcher

	dylibbundler -cd -b -of \
		-x $(APPDIR)/Contents/MacOS/$(NAME) \
		-d $(APPDIR)/Contents/MacOS/libs \
		-p '@executable_path/libs/'

	cp -pR $(DATADIRS) $(APPDIR)/Contents/Resources/
	cp -p resources/Avanor.icns $(APPDIR)/Contents/Resources/
	cp -p resources/Info.plist $(APPDIR)/Contents/

	/usr/libexec/PlistBuddy -c "Set :CFBundleVersion $(VERSION)" \
		$(APPDIR)/Contents/Info.plist
	/usr/libexec/PlistBuddy -c \
		"Add :CFBundleShortVersionString string $(VERSION_NUMBER)" \
		$(APPDIR)/Contents/Info.plist

	cp -p COPYING CHANGELOG.md README.md $(DMGROOT)/

	create-dmg \
		--volname "Avanor $(VERSION)" \
		--volicon resources/Avanor.icns \
		--window-pos 400 100 \
		--window-size 480 360 \
		--icon-size 64 \
		--icon "Avanor.app" 110 130 \
		--hide-extension "Avanor.app" \
		--app-drop-link 350 130 \
		--icon "README.md" 70 260 \
		--icon "CHANGELOG.md" 190 260 \
		--icon "COPYING" 310 260 \
		$(DMG) $(DMGROOT)/

	$(RM) -r $(DMGROOT)

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
