uid.hex:	uid.ihx
			srec_cat uid.ihx -intel -o uid.hex -intel

uid.ihx:	uid.c uid.h 8051.h 8052.h
			sdcc -mmcs51 --stack-auto uid.c

uid_hostside.exe: uid_hostside.c
			gcc uid_hostside.c -o uid_hostside

clean:
			del uid.hex uid.asm uid.ihx uid.lk uid.lst uid.map uid.mem uid.rel uid.rst uid.sym \
			uid_hostside.exe