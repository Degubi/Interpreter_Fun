from subprocess import call
from os import remove

call('gcc tests.c interpreter.c compiler.c -o Prog')
call('Prog')
remove('Prog.exe')