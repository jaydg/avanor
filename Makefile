##############################################################################
# Compiling Avanor: make {dos=1} {win=1} {debug=1}                           #
#                                                                            #
# Define dos=1 when compiling with DJGPP gcc compiler for MS-DOS             #
# Define win=1 when compiling with Mingw32 gcc compiler for windows          #
# Define debug=1 when you want to build debug version of Avanor              #
#                                                                            #
# Running just make builds the release version of Avanor for *nix            #
# (Linux, FreeBSD, ...)                                                      #
#                                                                            #
# Also it is highly recommended that you have subversion client installed    #
# ???                                                                        #
##############################################################################

ifndef dos
	CC = g++
	LD = g++
	AVANOR_SVNVERSION := ${shell svnversion .}
	CFLAGS = -funsigned-char
	CFLAGS += -pipe -DAVANOR_SVNVERSION=\"$(AVANOR_SVNVERSION)\"
else
	CC = gpp
	LD = gpp
	CFLAGS = -funsigned-char
endif

#CFLAGS += -Wall
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

ifdef debug
	CFLAGS += -g
	OBJDIR := ${addsuffix -d,$(OBJDIR)}
	NAME := ${addsuffix -d,$(NAME)}
else
	CFLAGS += -O2 -mcpu=i586 -s
endif

LIBS = -static -lncurses
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
	-$(RM) avanor-r$(AVANOR_SVNVERSION)-src.zip
	svn export . avanor-r$(AVANOR_SVNVERSION)-src
	7z a -tzip -r -mx avanor-r$(AVANOR_SVNVERSION)-src.zip "avanor-r$(AVANOR_SVNVERSION)-src/*"
	svn delete --force avanor-r$(AVANOR_SVNVERSION)-src

-include $(DEPS)
