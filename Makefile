all: sami.o
clean:
	rm -f sami.o

README: sami.3
	mandoc -Tascii sami.3 | col -b > README
