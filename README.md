# not-a-c-backdoor-rpi

This is an implementation of the compiler hack described by Ken Thompson's article [Reflections on Trusting Trust](https://www.cs.cmu.edu/~rdriley/487/papers/Thompson_1984_ReflectionsonTrustingTrust.pdf)

## le write-up
The main crux of the hack is just reading a c file, reading a file line by line, and once you reach
a line you want to change, shove your exploit as a string into the file. It’s kind of like if you
handed a document to the post office, and a man in the middle intercepts your file, changes
some of it without you knowing, and hands it down to the post office without suspicion.

Clean version: <original source code>

Dirty version: <some source code> <exploit code> <some source code>

For the context of this assignment, we hack two things - the login.c file, as well as the compiler
itself.

The login.c hack by itself is quite simple. Following the above guidelines in the dirty compiler, we
check if the file is called “login.c”. If so, we begin our exploit. Read through each line until we
reach the line in question that we want to change, i.e. the line that compares your password to
the fixed string “root”. As of this point, we can just get rid of the existing line from the file, replace
it with an additional condition, perhaps something like “if password == root OR if password ==
whatever_i_want”. We then finish parsing the file, and smack it on its way for it to begin
compiling. In our implementation we used a pipe to transfer the contents of the original file to the
bugged file.

Now obviously, if anyone was to review the source of the compiler, then you’re screwed. To do
so, we need to pass a clean version of the compiler through our bugged version, and still result
in a bugged executable. In other words, we propagate knowledge of our bugged compiler into
the clean executable.

For this, we need to find the line of code in TCC that handles compilation. After a decent
amount of digging, we found that a chain of calls:

- main : tcc.c ->
- tcc_add_file : libtcc.c ->
- tcc_add_file_internal : libtcc.c ->
- tcc_compile : libtcc.c
which led us to the tcc_compile function. So we have the name of the file to change “libtcc.c”,
and the function to target “tcc_compile”

The next problem we need to solve is figuring out how to inject the bug into the tcc_compile
function. With this, we follow the same steps as we did with login.c, reading through each line
until we reach the tcc_compile function, and then inject the bug. However, what makes this part
a little more interesting is that the string we inject into this file happens to be the same file we’re
editing, as we’re feeding libtcc.c into libtcc.c. In other words, libtcc serves as a program that has
the ability to print out its own source code. For our purposes, however, this doesn’t actually have
to be a perfect quine, so long as this injected string captures the functionality of the libtcc.c
bugged file. For this to occur, it does still take the general structure of a quine, being a massive
char* at the beginning of the file containing the source, followed by the actual code. We use the
same piping logic as we did for the login.c exploit.

So with this, now we’re able to pass in a clean copy of tcc’s source code into our bugged
compiler, which will identify the corresponding lines in said source to change and bug, and spit
out an executable without visual detection. In other words, even if we compile the clean code
into an executable, and then run login.c through said executable, our bugged functionality will
still work.

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
  - if you type `./login.out backdoor`, the magic happens
