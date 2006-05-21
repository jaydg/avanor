##############################################################################
# Compiling Avanor: make {dos=1} {win=1} {debug=1}                           #
#                                                                            #
# Define dos=1 when compiling with DJGPP gcc compiler for MS-DOS             #
# Define win=1 when compiling with Mingw gcc compiler for windows            #
# Define xmingw=1 when compiling win32-binary with Mingw gcc crosscompiler   #
# Define debug=1 when you want to build debug version of Avanor              #
#                                                                            #
# Just typing 'make' builds the release version of ufo2000 for *nix          #
# (Linux, FreeBSD, ...)                                                      #
#                                                                            #
# There are also targets for making tarballs with the sources and binaries   #
# Examples:                                                                  #
# 1) Create tarball (avanor-0.5.7-src.tar.bz2) with the sources of the game  #
#    make VERSION=0.5.7 source-bz2                                           #
# 2) Compile and package DOS-version of Avanor                               #
#    make VERSION=0.5.7 dos=1 binary-zip                                     #
##############################################################################

ifdef VERSION
	DISTNAME := avanor-$(VERSION)
else
	DISTNAME := avanor-r${shell svnversion .}
endif

ifndef dos
	CC = g++
	LD = g++
	CFLAGS = -fsigned-char -pipe
else
	CC = gpp
	LD = gpp
	CFLAGS = -fsigned-char
endif

ifdef xmingw
    CC = i386-mingw32msvc-g++
    LD = i386-mingw32msvc-g++
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

ROOT = ./
TOROOT = ./../
IPATH = -I${ROOT}global -I${ROOT}map -I${ROOT}creature -I${ROOT}engine -I${ROOT}helpers \
        -I${ROOT}item -I${ROOT}magic -I${ROOT}other -I${ROOT} -I${ROOT}game \
        -I${TOROOT}global -I${TOROOT}map -I${TOROOT}creature -I${TOROOT}engine -I${TOROOT}helpers \
        -I${TOROOT}item -I${TOROOT}magic -I${TOROOT}other -I${TOROOT} -I${TOROOT}game

CFLAGS += $(IPATH)

VPATH = creature engine game global helpers item magic map other

SRCS = xweapon.cpp xtool.cpp xtime.cpp xstring.cpp xstr.cpp xshield.cpp       \
       xshedule.cpp xscroll.cpp xring.cpp xration.cpp xpotion.cpp xobject.cpp \
       xmoney.cpp xmissileweapon.cpp xmissile.cpp xmapobj.cpp xlist.cpp       \
       xhero3.cpp xhero2.cpp xhero.cpp xherb.cpp xguihtml.cpp xgui.cpp        \
       xgloves.cpp xgen.cpp xenhance.cpp xcorpse.cpp xclothe.cpp xcloak.cpp   \
       xcap.cpp xboots.cpp xbook.cpp xbaseobj.cpp xarmor.cpp xarchive.cpp     \
       xapi.cpp xanyplace.cpp xanyfood.cpp xamulet.cpp wskills.cpp            \
       wmagic.cpp wizard_dungeon.cpp Uniquem.cpp uniquei.cpp udeadtomb.cpp    \
       strproc.cpp Std_ai.cpp stats.cpp spell.cpp skills.cpp skill.cpp        \
       skeep_ai.cpp shop.cpp setting.cpp resist.cpp rect.cpp rat_cell.cpp     \
       quest.cpp other_misc.cpp Mushcave.cpp msgwin.cpp            \
       modifers.cpp modifer.cpp map.cpp manual.cpp mainloc.cpp Main.cpp       \
       magic.cpp los.cpp location.cpp ldebug.cpp lbuilderpalette.cpp          \
       itemf.cpp itemdb.cpp item_misc.cpp item.cpp hiscore.cpp global.cpp     \
       Game.cpp effect.cpp dwarfcity.cpp dice.cpp deity.cpp dwarfcave.cpp     \
       creatures.cpp creature2.cpp creature.cpp cbuilder.cpp cave.cpp         \
       bodypart.cpp anycr.cpp ai_view.cpp

LIBS = -lncurses

ifdef debug
	CFLAGS += -g
	OBJDIR := ${addsuffix -d,$(OBJDIR)}
	NAME := ${addsuffix -d,$(NAME)}
else
	CFLAGS += $(OPTFLAGS)
endif

ifdef win
	OBJDIR := ${addsuffix win,$(OBJDIR)}
	NAME := ${addsuffix -win.exe,$(NAME)}
	LIBS :=
endif
ifdef dos
	OBJDIR := ${addsuffix dos,$(OBJDIR)}
	NAME := ${addsuffix -dos.exe,$(NAME)}
	LIBS :=
endif

OBJS = $(addprefix $(OBJDIR)/,$(SRCS:.cpp=.o))
DEPS = $(addprefix $(OBJDIR)/,$(SRCS:.cpp=.d))

##############################################################################

all: $(OBJDIR) $(NAME)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
	$(CC) -MMD $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(LD) $(CFLAGS) -o $@ $^ $(LIBS)

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
	cp $(NAME) avanor.exe
	upx --best avanor.exe
	7z a -tzip -mx $(DISTNAME).zip "avanor.exe" gpl.txt changes.txt "manual/*.html" "manual/*.css"

binary-gz: all
	-$(RM) $(DISTNAME).tar.gz
	tar -czf $(DISTNAME).tar.gz avanor gpl.txt changes.txt manual/*.html manual/*.css

-include $(DEPS)
