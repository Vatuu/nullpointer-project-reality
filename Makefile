# +---------+
# | Defines |
# *---------+

PROJECT 	= clockwork_creature
SRCDIR 		= src
BUILDIR 	= build
TOOLDIR		= tools
TEXBUILDDIR = $(BUILDIR)/textures
TOOLBUILD	= $(BUILDIR)/tools

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

MILD_OPT	= -Map $(BUILDIR)/$(PROJECT).map

# +-------------+
# | Executables |
# *-------------+

CC  		= $(N64_TOOLCHAIN)/bin/mips64-elf-gcc
CC_TOOL		= gcc
LD  		= $(N64_TOOLCHAIN)/bin/mips64-elf-ld
MAKEROM 	= $(N64_TOOLCHAIN)/bin/mild
MAKEMASK	= $(N64_TOOLCHAIN)/bin/makemask
N64GRAPHICS = ./$(TOOLDIR)/n64graphics_wrapper.sh
BLOBINATOR	= $(TOOLBUILD)/blobinator.tool

# +----------+
# | Binaries |
# *----------+

ROM 		= $(BUILDIR)/$(PROJECT).n64
BINARY		= $(BUILDIR)/$(PROJECT).out
CODE 		= $(BUILDIR)/$(PROJECT).code.o
TEXTURES	= $(BUILDIR)/$(PROJECT).textures.o
TEXTUREMAP	= $(TEXBUILDDIR)/$(PROJECT).texmap

# +-------------+
# | Directories |
# *-------------+

VPATH 		= $(SRCDIR) $(SRCDIR)/stages $(SRCDIR)/core $(TEXDIR)/n64
INCDIRS		= $(SRCDIR) $(SRCDIR)/core $(SRCDIR)/assets/models $(NUSYSINCDIR) $(ULTRAINCDIR) $(GCCINCDIR)
TEXDIRS		= $(TEXDIR)/n64

# +--------------+
# | Source Files |
# *--------------+

CODEFILES	= $(foreach dir,$(VPATH),$(wildcard $(dir)/*.c))
INCFILES	= $(foreach dir,$(INCDIRS),$(wildcard $(dir)/*.h))
TEXFILES	= $(foreach dir,$(TEXDIRS),$(wildcard $(dir)/*.png))
TOOLFILES	= $(wildcard $(TOOLDIR)/*.c)

# +---------------+
# | Final Objects |
# *---------------+

CODEOBJECTS = $(addprefix $(BUILDIR)/, $(CODEFILES:.c=.c.o)) $(NUSYSLIBDIR)/nusys.o
TEXOBJECTS	= $(patsubst $(TEXDIR)/%.png, $(TEXBUILDDIR)/%.png.inc, $(TEXFILES))
TOOLOBJECTS	= $(patsubst $(TOOLDIR)/%.c, $(TOOLBUILD)/%.tool, $(TOOLFILES))

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

$(ROM):	$(TEXTURES) $(CODE) 
		$(MAKEROM) spec.cvt -I$(NUSYSINCDIR) -L$(PRRESDIR) -r$(ROM) -e$(BINARY) -Map $(MAP)
		$(MAKEMASK) $(ROM)

$(CODE): $(CODEOBJECTS) Makefile
		$(LD) -o $(CODE) -r $(CODEOBJECTS) $(LDFLAGS)

$(BUILDIR)/%.c.o: %.c
		mkdir -p $(dir $@)
		$(CC) $(C_FLAGS) $(OPT_FLAGS) -c $< -o $@

#TODO MAPFILE
$(TEXTURES): $(TEXOBJECTS) $(TOOLOBJECTS) Makefile
		$(BLOBINATOR) -i $(TEXBUILDDIR) -e .inc -t texture -o $(TEXTURES) -d $(TEXTUREMAP) -c $(TEXBUILDDIR)

$(TEXBUILDDIR)/%.rgba8.png.inc: %.rgba16.png
		mkdir -p $(dir $@)
		$(N64GRAPHICS) -i $@ -g $< -f rgba -s 16 -m $(TEXTUREMAP)

$(TOOLBUILD)/%.tool: %.c
		mkdir -p $(dir $@)
		$(CC_TOOL) -Wall $< -o $@

clean:
	rm -rf $(BUILDIR)