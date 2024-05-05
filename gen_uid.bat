sdcc -mmcs51 --stack-auto .\uid.c
srec_cat .\uid.ihx -intel -o uid.hex -intel