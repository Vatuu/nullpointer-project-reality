#include <nusys.h>

beginseg
	name	"code"
	flags	BOOT OBJECT
	entry	nuBoot
	address	NU_SPEC_BOOT_ADDR
	stack	NU_SPEC_BOOT_STACK
	include	"./build/clockwork_creature.code.o"
	include	"/etc/n64/lib/PR/rspboot.o"
	include	"/etc/n64/lib/PR/gspF3DEX2.fifo.o"
	include	"/etc/n64/lib/PR/gspL3DEX2.fifo.o"
	include	"/etc/n64/lib/PR/gspF3DEX2.Rej.fifo.o"
	include	"/etc/n64/lib/PR/gspF3DEX2.NoN.fifo.o"
	include	"/etc/n64/lib/PR/gspF3DLX2.Rej.fifo.o"
	include	"/etc/n64/lib/PR/gspS2DEX2.fifo.o"
endseg


beginwave
	name 	"clockworkcreature"
	include "code"
endwave