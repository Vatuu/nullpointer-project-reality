# +---------+
# | Defines |
# *---------+

PROJECT 	= clockwork_creature
SRCDIR 		= src
BUILDIR 	= build
TOOLDIR		= tools
TEXBUILDDIR = $(BUILDIR)/textures
TOOLBUILD	= $(BUILDIR)/tools

ULTRAINCDIR = /usr/include/n64
NUSTDINCDIR = $(ULTRAINCDIR)/nustd 
NUSYSINCDIR = $(ULTRAINCDIR)/nusys

ULTRALIBDIR = /usr/lib/n64
PRLIBDIR	= $(ULTRALIBDIR)/PR
NUSYSLIBDIR = $(ULTRALIBDIR)/nusys

ULTRAGCCDIR = /usr/local/lib/gcc/mips64-elf/10.2.0
GCCINCDIR	= $(ULTRAGCCDIR)/include

TEXDIR		= $(SRCDIR)/assets/textures

# +-------------+
# | Executables |
# *-------------+

CC  		= mips-n64-gcc
LD  		= mips-n64-ld
MAKEROM 	= mild
MAKEMASK	= makemask
N64GRAPHICS = $(TOOLDIR)/n64graphics_wrap.sh
BLOBINATOR	= $(TOOLBUILD)/blobinator.tool

# +----------+
# | Binaries |
# *----------+

ROM 		= $(BUILDIR)/$(PROJECT).n64
BINARY		= $(BUILDIR)/$(PROJECT).out
CODE 		= $(BUILDIR)/$(PROJECT).code.o
TEXTURES	= $(BUILDIR)/$(PROJECT).textures.o
TEXTUREMAP	= $(TEXBUILDDIR)/$(PROJECT).texmap
TEXTURELUT	= $(TEXBUILDDIR)/texture_lut.c

# +-------------+
# | Directories |
# *-------------+

TEXDIRS		= $(TEXDIR)/n64
INCDIRS		= $(SRCDIR) $(SRCDIR)/core $(SRCDIR)/boos_bs $(SRCDIR)/assets/models $(NUSYSINCDIR) $(NUSTDINCDIR) $(ULTRAINCDIR) $(GCCINCDIR)
VPATH 		= $(SRCDIR) $(SRCDIR)/stages $(SRCDIR)/core $(SRCDIR)/boos_bs $(SRCDIR)/actors

# +--------------+
# | Source Files |
# *--------------+

CODEFILES	= $(foreach dir,$(VPATH),$(wildcard $(dir)/*.c)) $(TEXTURELUT)
INCFILES	= $(foreach dir,$(INCDIRS),$(wildcard $(dir)/*.h))
TEXFILES	= $(foreach dir,$(TEXDIRS),$(wildcard $(dir)/*.png))
TOOLFILES	= $(wildcard $(TOOLDIR)/*.cs)

# +---------------+
# | Final Objects |
# *---------------+

CODEOBJECTS = $(addprefix $(BUILDIR)/, $(CODEFILES:.c=.c.o)) $(NUSYSLIBDIR)/nusys.o
TEXOBJECTS	= $(patsubst $(TEXDIR)/%.png, $(TEXBUILDDIR)/%.png.inc, $(TEXFILES))
TOOLOBJECTS	= $(patsubst $(TOOLDIR)/%.cs, $(TOOLBUILD)/%.tool, $(TOOLFILES))

# +---------------+
# | CC & LD Flags |
# *---------------+

MCPU_FLAGS 	= -march=vr4300 -mtune=vr4300 -mfix4300 -mabi=32 -mno-abicalls
INC_FLAGS	= $(foreach dir,$(INCDIRS),-I$(dir))
PLAT_FLAGS	= -DNU_DEBUG -DNDEBUG -DF3DEX_GBI_2 -G 0
C_FLAGS 	= $(MCPU_FLAGS) $(INC_FLAGS) $(PLAT_FLAGS) -MD -mno-long-calls -Wall -Wno-unknown-pragmas -Wno-missing-braces -nostdlib
OPT_FLAGS 	= -g

LDFLAGS 	= -L$(NUSYSLIBDIR) -L$(ULTRALIBDIR) -L$(ULTRAGCCDIR) -lnusys_d -lultra_rom -lgcc -lnustd

# +-------+
# | Rules |
# *-------+

-include $(BUILDIR)/*.d

default: $(ROM)

$(ROM):	$(TEXTURES) $(CODE) 
		$(MAKEROM) spec.cvt -I$(NUSYSINCDIR) -L$(PRLIBDIR) -r$(ROM) -e$(BINARY)
		$(MAKEMASK) $(ROM)

$(CODE): $(CODEOBJECTS) Makefile
		$(LD) -o $(CODE) -r $(CODEOBJECTS) $(LDFLAGS)

$(BUILDIR)/%.c.o: %.c
		mkdir -p $(dir $@)
		$(CC) $(C_FLAGS) $(OPT_FLAGS) -c $< -o $@

$(TEXTURES): $(TEXOBJECTS) $(TOOLOBJECTS) Makefile
		$(BLOBINATOR) -i $(TEXBUILDDIR) -e .inc -t TEXTURES -o $(TEXTURES) -d $(TEXTUREMAP) -m $(TEXTURELUT)

$(TEXBUILDDIR)/%.rgba16.png.inc: $(TEXDIR)/%.rgba16.png
		mkdir -p $(dir $@)
		$(N64GRAPHICS) -i $@ -g $< -f rgba -s 16 -m $(TEXTUREMAP)

$(TOOLBUILD)/%.tool: $(TOOLDIR)/%.cs
		mkdir -p $(dir $@)
		mcs $< -out:$@

clean:
	rm -rf $(BUILDIR)