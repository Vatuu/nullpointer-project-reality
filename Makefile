# +---------+
# | Defines |
# *---------+

PROJECT 	= clockwork_creature
SRCDIR 		= src
BUILDIR 	= build
TEXBUILDDIR = $(BUILDIR)/textures

ULTRAINCDIR = /etc/n64/usr/include
ULTRALIBDIR = /etc/n64/lib
NUSYSINCDIR = $(ULTRAINCDIR)/nusys
NUSYSLIBDIR = $(ULTRALIBDIR)/nusys
PRRESDIR	= /etc/n64/usr/lib/PR
PRINCDIR	= $(ULTRAINCDIR)/PR
PRLIBDIR	= $(ULTRALIBDIR)/PR
GCCLIBDIR	= $(N64_TOOLCHAIN)/lib/gcc/mips64-elf/10.2.0
GCCINCDIR	= $(GCCLIBDIR)/include
TEXDIR		= $(SRCDIR)/assets/textures

# +-------------+
# | Executables |
# *-------------+

CC  		= $(N64_TOOLCHAIN)/bin/mips64-elf-gcc
LD  		= $(N64_TOOLCHAIN)/bin/mips64-elf-ld
MAKEROM 	= $(N64_TOOLCHAIN)/bin/mild
MAKEMASK	= $(N64_TOOLCHAIN)/bin/makemask
N64GRAPHICS = n64graphics

# +----------+
# | Binaries |
# *----------+

ROM 		= $(BUILDIR)/$(PROJECT).n64
BINARY		= $(BUILDIR)/$(PROJECT).out
CODE 		= $(BUILDIR)/$(PROJECT).code.o
DATA		= $(BUILDIR)/$(PROJECT).data.o

# +-------------+
# | Directories |
# *-------------+

VPATH 		= $(SRCDIR) $(SRCDIR)/stages $(SRCDIR)/core
INCDIRS		= $(SRCDIR) $(SRCDIR)/core $(SRCDIR)/stages $(SRCDIR)/assets/models $(NUSYSINCDIR) $(ULTRAINCDIR) $(GCCINCDIR)
TEXDIRS		= $(TEXDIR)/n64

# +--------------+
# | Source Files |
# *--------------+

CODEFILES	= $(foreach dir,$(VPATH),$(wildcard $(dir)/*.c))
INCFILES	= $(foreach dir,$(INCDIRS),$(wildcard $(dir)/*.h))
TEXFILES	= $(foreach dir,$(TEXDIRS),$(wildcard $(dir)/*.png))

# +---------------+
# | Final Objects |
# *---------------+

CODEOBJECTS = $(addprefix $(BUILDIR)/, $(CODEFILES:.c=.c.o)) $(NUSYSLIBDIR)/nusys.o
DATAOBJECTS = $(addprefix $(TEXBUILDDIR)/, $(TEXFILES:.png=.png.inc))

# +---------------+
# | CC & LD Flags |
# *---------------+

MCPU_FLAGS 	= -march=vr4300 -mtune=vr4300 -mfix4300 -mabi=32 -mno-abicalls
INC_FLAGS	= $(foreach dir,$(INCDIRS),-I$(dir))
PLAT_FLAGS	= -DNU_DEBUG -DNDEBUG -DF3DEX_GBI_2 -G 0
C_FLAGS 	= $(MCPU_FLAGS) $(INC_FLAGS) $(PLAT_FLAGS) -MD -mno-long-calls -Wall -Wno-unknown-pragmas -Wno-missing-braces -nostdlib
OPT_FLAGS 	= -g

LDFLAGS 	= -L$(NUSYSLIBDIR) -L$(ULTRALIBDIR) -L$(GCCLIBDIR) -lnusys_d -lultra_rom -lgcc -lnustd

# +-------+
# | Rules |
# *-------+

-include $(BUILDIR)/*.d

default: $(ROM)

$(ROM):	$(CODE) $(DATA)
		$(MAKEROM) spec.cvt -I$(NUSYSINCDIR) -L$(PRRESDIR) -r$(ROM) -e$(BINARY) 
		$(MAKEMASK) $(ROM)

$(DATA): $(DATAOBJECTS) Makefile
		# Creating the data blob

$(CODE): $(CODEOBJECTS) Makefile
		$(LD) -o $(CODE) -r $(CODEOBJECTS) $(LDFLAGS)

$(BUILDIR)/%.c.o: %.c
		mkdir -p $(dir $@)
		$(CC) $(C_FLAGS) $(OPT_FLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDIR)