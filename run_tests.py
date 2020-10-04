from subprocess import call
from os import remove, path

call('gcc tests.c interpreter.c compiler.c -o Prog')
call('Prog')
remove('Prog.exe')

if path.exists('Prog.exe.stackdump'):
    remove('Prog.exe.stackdump')