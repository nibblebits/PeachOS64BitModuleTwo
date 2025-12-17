FILES=./build/blank.o
INCLUDES= -I../stdlib/src -I../guilib/src -I../containerlib/src
FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc
.PHONY: all clean
all: ${FILES}
	x86_64-elf-gcc  -g -T ./linker.ld -o ./blank.elf -ffreestanding -O0 -nostdlib -fpic -g ${FILES} ../containerlib/containerlib.elf ../stdlib/stdlib.elf ../guilib/guilib.elf

./build/blank.o: ./blank.c
	x86_64-elf-gcc ${INCLUDES} -I./ $(FLAGS) -std=gnu99 -c ./blank.c -o ./build/blank.o

clean:
	rm -f ${FILES}
	rm -f ./blank.elf
