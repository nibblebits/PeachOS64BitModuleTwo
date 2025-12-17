FILES=./build/vector.o
INCLUDES=-I./src -I../stdlib/src
FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc
.PHONY: all clean

all: ${FILES}
	x86_64-elf-ld -m elf_x86_64 -relocatable ${FILES} -o ./containerlib.elf

./build/vector.o: ./src/vector.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/vector.c -o ./build/vector.o 

clean:
	rm -f ${FILES}
	rm -f ./containerlib.elf
