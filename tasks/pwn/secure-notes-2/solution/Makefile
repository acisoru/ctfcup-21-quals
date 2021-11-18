all:	
	gcc -static priv_esc.c -o exploit_priv_esc
	strip -s exploit_priv_esc
	upx -9 exploit_priv_esc
	cp exploit_priv_esc /home/user/kernel-src/securenotes/initramfs/
