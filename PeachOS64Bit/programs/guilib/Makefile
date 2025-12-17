FILES=./build/window.o ./build/gui/event.o ./build/gui/property.o ./build/gui/exampleproperty.o ./build/image.o ./build/gui/scrollable.o ./build/gui/listarea.o ./build/bmp.o ./build/font.o ./build/gui/image_element.o  ./build/graphics.o ./build/gui/gui.o ./build/gui/plane.o ./build/gui/textfield.o ./build/gui/button.o ./build/gui/element.o
INCLUDES=-I./src  -I../stdlib/src -I../containerlib/src
FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc
.PHONY: all clean

all: ${FILES}
	x86_64-elf-ld -m elf_x86_64 -relocatable ${FILES} -o ./guilib.elf  

./build/window.o: ./src/window.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/window.c -o ./build/window.o

./build/bmp.o: ./src/bmp.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/bmp.c -o ./build/bmp.o

./build/font.o: ./src/font.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/font.c -o ./build/font.o


./build/image.o: ./src/image.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/image.c -o ./build/image.o

./build/graphics.o: ./src/graphics.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/graphics.c -o ./build/graphics.o

./build/gui/textfield.o: ./src/gui/textfield.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/gui/textfield.c -o ./build/gui/textfield.o

./build/gui/scrollable.o: ./src/gui/scrollable.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/gui/scrollable.c -o ./build/gui/scrollable.o

./build/gui/gui.o: ./src/gui/gui.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/gui/gui.c -o ./build/gui/gui.o

./build/gui/event.o: ./src/gui/event.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/gui/event.c -o ./build/gui/event.o
./build/gui/property.o: ./src/gui/property.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/gui/property.c -o ./build/gui/property.o
./build/gui/exampleproperty.o: ./src/gui/exampleproperty.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/gui/exampleproperty.c -o ./build/gui/exampleproperty.o



./build/gui/image_element.o: ./src/gui/image_element.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/gui/image_element.c -o ./build/gui/image_element.o

./build/gui/element.o: ./src/gui/element.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/gui/element.c -o ./build/gui/element.o

./build/gui/button.o: ./src/gui/button.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/gui/button.c -o ./build/gui/button.o

./build/gui/plane.o: ./src/gui/plane.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/gui/plane.c -o ./build/gui/plane.o

./build/gui/listarea.o: ./src/gui/listarea.c
	x86_64-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/gui/listarea.c -o ./build/gui/listarea.o



clean:
	rm -f ${FILES}
	rm -f ./guilib.elf
