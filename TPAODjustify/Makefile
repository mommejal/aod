# Modele de Makefile : a adapter à vos besoins, mais rapport.pdf doit rester en haut du repertoire et
# le fichier a tester doit correspondre aux spécifications et etre dans le repertoire ../TPAODjustify/bin/APDjustify 

CC = gcc -O3 -std=c99 

all: bin/AODjustify AODrapport-modele.pdf

doc: src/AODjustify.c  # A COMPLETER
	doxygen src/AODjustify.c

eval: bin/AODjustify
	cd ..; source /matieres/4MMAOD6/install/AODjustify-calcul-note.bash # 2&> /tmp/foo
	
bin/AODjustify: src/AODjustify.c  # A COMPLETER
	$(CC) -o bin/AODjustify src/AODjustify.c
	# chmod 0111 bin/AODjustify

AODrapport-modele.pdf: rapport/AODrapport-modele.tex 
	cd rapport; pdflatex AODrapport-modele; pdflatex AODrapport-modele
	mv rapport/AODrapport-modele.pdf .

clean:
	rm bin/AODjustify  rapport/AODrapport-modele.aux rapport/AODrapport-modele.log AODrapport-modele.pdf
	rm -R TESTS

