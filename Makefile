step1:
	( cd tinycc_dirty; ./configure; make; sudo make install; )


#"/usr/local/bin/tcc" is the dirty version, compile tinycc_clean with it
step2:
	( cd tinycc_clean; ./configure --cc="/usr/local/bin/tcc"; make; sudo make install; )



login:
	tcc login.c -o login.out
