step1:
	@echo "[*] Compiling the cheeky version"
	( cd tinycc_dirty; ./configure; make; sudo make install; )

step2:
	@echo "[*] Compiling the 'clean' version"
	( cd tinycc_clean; ./configure --cc="/usr/local/bin/tcc"; make; sudo make install; )

reset_tinycc:
	(rm -r tinycc_clean; cp -r tinycc tinycc_clean)

#"/usr/local/bin/tcc" is the dirty version, compile tinycc_clean with it
# step2:



login:
	tcc login.c -o login.out
