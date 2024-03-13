caso_teste = 13

p_in = $(caso_teste).in
p_bin= binario$(caso_teste).bin
p_ind= indice$(caso_teste).bin

all:
	gcc -o programaTrab *.c -lm -Wall -g
run:
	./programaTrab


run2:
	./programaTrab < input.in
test:
	make copy
	clear
	make all
	make run2
	cp $(p_ind) Debugador/meu.bin
valgrind:
	make copy
	gcc -g -Wall *.c && valgrind ./programTrab -s < input.in
zip:
	rm Trabalho1.zip
	zip Trabalho1.zip *.c *.h Makefile -r arquivos casos_de_teste Debugador
rmbin:
	rm *.bin
copy:
	cp casos_de_teste/in/$(p_in) input.in
	cp arquivos/antes/$(p_bin) $(p_bin)
	cp arquivos/antes/$(p_ind) $(p_ind)
	cp arquivos/depois/$(p_ind) Debugador/correto.bin
zip_runcodes:
	zip Trabalho_arquivos_1 *.c *.h Makefile