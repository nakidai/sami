all: sami.o
clean:
	rm -f sami.o

README:
	mandoc -Tascii sami.3 | col -b > README
