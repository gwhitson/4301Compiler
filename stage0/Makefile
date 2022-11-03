CLASS = 4301
BASE_DIR = /usr/local/$(CLASS)
LINK_DIRS = -L$(BASE_DIR)/lib/
INCLUDE_DIRS = -I$(BASE_DIR)/include/ -I.
LFLAGS = -lm
CFLAGS = -g -Wall -std=c++11

# Assembler is NASM
ASM = nasm

# Format of object file is linux
LINUX_FORMAT = elf32

# Flags used in assemblying phase
ASMFLAGS =

# Linker is ld
LINKER = ld

# Share libbrary option for linker
SHLIB = --dynamic-linker

.SECONDEXPANSION:
CC = g++
.SUFFIXES:.o .C .CPP .cpp .c++ .cc .cxx .cp .asm

.C.o:
	$(CC) $(CFLAGS) -c $< $(INCLUDE_DIRS)

.CPP.o:
	$(CC) $(CFLAGS) -c $< $(INCLUDE_DIRS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< $(INCLUDE_DIRS)

.c++.o:
	$(CC) $(CFLAGS) -c $< $(INCLUDE_DIRS)

.cc.o:
	$(CC) $(CFLAGS) -c $< $(INCLUDE_DIRS)

.cxx.o:
	$(CC) $(CFLAGS) -c $< $(INCLUDE_DIRS)

.cp.o:
	$(CC) $(CFLAGS) -c $< $(INCLUDE_DIRS)

.asm.o:
	$(ASM) -f $(LINUX_FORMAT) $(ASMFLAGS) -o $*.o $< $(INCLUDE_DIRS)

targets1srcfile = 

$(targets1srcfile): $$@.o
	$(CC) -o $@ $@.o $(LINK_DIRS) $(LFLAGS)

targets2srcfiles = stage0

$(targets2srcfiles): $$@main.o $$@.o
	$(CC) -o $@ $@main.o $@.o $(LINK_DIRS) $(LFLAGS)

targets2srcfileswithlibrary = 

$(targets2srcfileswithlibrary): $$@main.o $$@.o
	$(CC) -o $@ $@main.o $@.o $(LINK_DIRS) \
	-Wl,-whole-archive -l$@ -Wl,-no-whole-archive $(LFLAGS)

targetsAsmLanguage = 

$(targetsAsmLanguage): $$@.o
	$(LINKER) -m elf_i386 $(SHLIB) /lib/ld-linux.so.2 -o $@ $@.o \
	$(BASE_DIR)/src/Along32.o -lc

clean:
	rm -f *.o core *~ $(targets1srcfile) $(targets2srcfiles)
	rm -f $(targets2srcfileswithlibrary)
