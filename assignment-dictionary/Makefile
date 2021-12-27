ELF = -felf64

main: main.o lib.o dict.o
	ld -o $@ $^

main.o: main.asm dict.o lib.o words.inc colon.inc
	nasm $(ELF) -o $@ $<

dict.o: dict.asm lib.o
	nasm $(ELF) -o $@ $<

lib.o: lib.asm
	nasm $(ELF) -o $@ $<

.PHONY: clean
clean:
	rm *.o

