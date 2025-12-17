FILES=./build/binary.bin
.PHONY: all clean
all: ${FILES}

./build/binary.bin: ./src/binary.asm
	nasm -f bin ./src/binary.asm -o ./build/binary.bin


clean:
	rm -f ${FILES}
