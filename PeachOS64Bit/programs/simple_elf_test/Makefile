FILES=./build/simple_elf_test.o
FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc
.PHONY: all clean
all: ${FILES}
	x86_64-elf-gcc -g -T ./linker.ld -o ./elftest.elf -ffreestanding -O0 -nostdlib -fpic -g ${FILES}

./build/simple_elf_test.o: ./src/simple_elf_test.asm
	nasm -f elf64 ./src/simple_elf_test.asm -o ./build/simple_elf_test.o


clean:
	rm -f ${FILES}
