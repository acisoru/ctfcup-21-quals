all:
	gcc -masm=intel main.c -o loader
	strip -s loader
	./injector.py loader server