
# Output.
KERNEL := skyekernel.elf

# C compiler.
CC := x86_64-elf-gcc

# OK flag.
CFLAGS := -Wall -Wno-unused-function -Wno-unused-parameter -Wextra -O2 -pipe

# All subdirectories of source code, as include arguments.
IDIRS  := $(shell find ./src/ -type d | sed -E 's|.*|-I\0|')

# Link flags.
LINKFLAGS :=		\
	-fno-pic -fpie	\
	-static-pie		\
	-nostdlib		\
	-Tlinker.ld		\
	-B/home/julian/osdev/tools/opt/cross/x86_64-elf/bin/ld	\
	-ggdb			\
	-z max-page-size=0x1000

# Compile flags.
CCFLAGS :=					\
	$(IDIRS)				\
	-std=gnu11				\
	-ffreestanding			\
	-fno-stack-protector	\
	-fno-pic -fpie			\
	-mgeneral-regs-only		\
	-nostdlib				\
	-ggdb					\
	-mno-red-zone

# Use find to glob all *.c files in the directory and extract the object names.
CFILES := $(shell find ./src/ -type f '(' -name '*.c' -o -name '*.s' ')')
OBJ    := $(shell find ./src/ -type f '(' -name '*.c' -o -iname '*.s' ')' | sed -E 's|^\./src/(.+)|./build/\1.o|')
#OBJ    := build/$(_FILES:.c=.o)

.PHONY: all clean

# Target.
all: $(KERNEL)

# Link rules.
$(KERNEL): $(OBJ)
		$(CC) $(LINKFLAGS) $(OBJ) -o $@

# Compile rules.
build/%.c.o: src/%.c
		$(CC) $(CFLAGS) $(CCFLAGS) -c $< -o $@

build/%.s.o: src/%.s
		$(CC) $(CFLAGS) $(CCFLAGS) -c $< -o $@

build/%.S.o: src/%.S
		$(CC) $(CFLAGS) $(CCFLAGS) -c $< -o $@

# Cleaning.
clean:
		rm -rf $(KERNEL) $(OBJ)
