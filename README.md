BUILD Commands:
as --64 <input.S> -o <output.o>

Both are equally trash:
For LINUX:
gcc -o <output> <input.o> -e _start -z noexecstack
For windows:
ld -L "<path_to_mvcrt.a>" -lc -o <output.exe> -subsys=console  <input.o>
