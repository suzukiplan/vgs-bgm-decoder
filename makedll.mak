OBJS=\
	./vgsdec.obj\
	./vgs2tone.obj

DEPS=./src/vgsdec.def

CFLAG=\
	/nologo\
	/Zi\
	/Ot\
	/GS-\
	/GR-\
	/GF\
	/W3\
	/WX\
	/LD\
	/D_CRT_SECURE_NO_WARNINGS\
	/I..\template\
	/Fo./\
	/c

all: vgsdec.dll

vgsdec.dll: $(OBJS) $(DEPS)
	LINK\
	/NOLOGO\
	/DEBUG\
	/DLL\
	/OUT:vgsdec.dll\
	/MAP:vgsdec.map\
	/PDB:vgsdec.pdb\
	/DEF:.\src\vgsdec.def\
	/MACHINE:X86\
	/SUBSYSTEM:WINDOWS\
	$(OBJS)\
	KERNEL32.lib\
	ws2_32.lib

./vgsdec.obj: ./src/vgsdec.c $(DEPS)
	CL $(CFLAG) ./src/vgsdec.c

./vgs2tone.obj: ./src/vgs2tone.c $(DEPS)
	CL $(CFLAG) ./src/vgs2tone.c

