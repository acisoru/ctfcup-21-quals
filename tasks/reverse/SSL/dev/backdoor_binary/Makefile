all:
	gcc -static main.c aes.c -lpthread  -o backdoor
	strip -s backdoor
	upx -9 backdoor
