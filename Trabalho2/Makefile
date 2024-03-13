all:
	gcc -o programaTrab *.c  #-g -fsanitize=address
run:
	./programaTrab






caso_teste = 6
p_in = $(caso_teste).in
p_bin= binario$(caso_teste).bin
p_ind= indice$(caso_teste).bin

clear:
	rm *.bin *.txt *.in *.out *.zip programaTrab
run2:
	./programaTrab < input.in
test:
	make copy
	clear
	make all
	#make run
	make run2
	cp $(p_bin) Debugador/meu.bin
valgrind:
	make copy
	gcc -g -Wall *.c -o ./programTrab && valgrind ./programTrab -s < input.in
zip:
	touch Trabalho2.zip
	rm Trabalho2.zip
	zip Trabalho2.zip *.c *.h Makefile -r arquivos casos_de_teste Debugador *.txt *.in *.bin
copy:
	cp casos_de_teste/in/$(p_in) input.in
	cp arquivos/antes/$(p_bin) $(p_bin)
	cp arquivos/antes/$(p_ind) $(p_ind)
	cp arquivos/depois/$(p_bin) Debugador/correto.bin
zip_runcodes:
	zip Trabalho_arquivos_2 *.c *.h Makefile
compare:
	hexdump -C RESPOSTA1.bin > RESPOSTA1.txt
	hexdump -C indice1.bin > indice1.txt
	diff RESPOSTA1.txt indice1.txt > diferenca.txt