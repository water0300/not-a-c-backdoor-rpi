# not-a-c-backdoor-rpi

## le code
- tinycc_dirty is the bad boy, contains the compiler hack. The exploit lives in the file `tinycc_dirty\libtcc.c -> tcc_compile(...) found [here](https://github.com/water0300/not-a-c-backdoor-rpi/blob/main/tinycc_dirty/libtcc.c#L730)
- tinycc_clean is the clean version of tinycc. Can replace with your own version of tinycc cloned [here](https://github.com/TinyCC/tinycc):
 

## running le exploit
- (Optional) Install TinyCC into this working directory and rename it to tinycc_clean
  - Don't forget to build TinyCC using their make instructions
- run `make step1`. This compiles tinycc_dirty using your normal install of tcc
- run `make step2`. This compiles tinycc_clean using the bugged version of tcc
- run `make step3`. This compiles the login.c file into a .out file
- test the login file by entering ./login.out <\password\>. Can verify with `echo $?` after running.
  - if you type `./login.out root`, it works as expected
  - if you type './login.out backdoor', the magic happens
