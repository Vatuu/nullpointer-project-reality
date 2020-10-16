PROJECT 	= clockwork_creature
SRCDIR 		= src
BUILDIR 	= build

ULTRAINCDIR = /etc/n64/usr/include
ULTRALIBDIR = /etc/n64/lib
PRRESDIR	= /etc/n64/usr/lib/PR
NUSYSINCDIR = $(ULTRAINCDIR)/nusys
NUSYSLIBDIR = $(ULTRALIBDIR)/nusys
PRINCDIR	= $(ULTRAINCDIR)/PR
PRLIBDIR	= $(ULTRALIBDIR)/PR


CC  		= $(N64_TOOLCHAIN)/bin/mips64-elf-gcc
LD  		= $(N64_TOOLCHAIN)/bin/mips64-elf-ld
MAKEROM 	= $(N64_TOOLCHAIN)/bin/mild
MAKEMASK	= $(N64_TOOLCHAIN)/bin/makemask

TARGETS 	= $(BUILDIR)/$(PROJECT).n64
APP 		= $(BUILDIR)/$(PROJECT).out
CODE 		= $(BUILDIR)/$(PROJECT).code.o

VPATH 		= $(SRCDIR) $(SRCDIR)/stages
HFILES		= types.h gfx_management.h assets/models/the_n.h stages/stage00_n64.h
CODEFILES	= main.c gfx_management.c stages/stages.c stages/stage00_n64.c
DATAFILES	= 

CODEOBJECTS = $(addprefix $(BUILDIR)/, $(CODEFILES:.c=.c.o)) $(NUSYSLIBDIR)/nusys.o
DATAOBJECTS = $(DATAFILES:.c=.c.o)

MCPU_FLAGS 	= -march=vr4300 -mtune=vr4300 -mfix4300 -mabi=32 -mno-abicalls
INC_FLAGS	= -I$(SRCDIR) -I$(SRCDIR)/stages -I$(SRCDIR)/assets/models -I$(NUSYSINCDIR) -I$(ULTRAINCDIR)
PLAT_FLAGS	= -DNU_DEBUG -DF3DEX_GBI_2 -G 0
C_FLAGS 	= $(MCPU_FLAGS) $(INC_FLAGS) $(PLAT_FLAGS) -mno-long-calls -Wall -Wno-unknown-pragmas -Wno-missing-braces -nostdlib
OPT_FLAGS 	= -g

LDFLAGS 	= -L$(NUSYSLIBDIR) -L$(ULTRALIBDIR) -lnusys_d -lultra_d -lkmc

# +-------+
# | Rules |
# *-------+

default: $(TARGETS)

$(TARGETS):	$(CODE) $(DATAOBJECTS)
		$(MAKEROM) spec -I$(NUSYSINCDIR) -L$(PRRESDIR) -r$(TARGETS) -e$(APP) 
		$(MAKEMASK) $(TARGETS)

$(CODE): $(CODEOBJECTS) Makefile
		$(LD) -o $(CODE) -r $(CODEOBJECTS) $(LDFLAGS)

$(BUILDIR)/%.c.o: %.c
		mkdir -p $(dir $@)
		$(CC) $(C_FLAGS) $(OPT_FLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDIR)