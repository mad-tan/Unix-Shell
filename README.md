# Unix-Shell

1 -> I made a simple Unix shell program in C. 
2 -> It supports majority of Unix commands.
3 -> It has two modes:
        -interactive mode: runs like a normal shell.
        -batch mode: takes the file as arguement and runs the commands inside the file, the shell then quits. Multiple file arguements are not allowed.
4 -> The built in commands used are:
        -cd: "cd" will result in error, cd will not take two arguements.
        -path: behaves differently, "path" will remove every path, "path /test /test1 adds the two paths, every new path query replaces the old path.
        -exit
5 -> The shell also supports redirection. Multiple arguements "ls>test>test1" are not allowed. Redirection for built-ins are not supported.
6 -> It also supports parallel commands. Ex: "ls & ls > test1 & ls > test2" will run parallelly and thus the output of ls will include both test1 and test2.


BUGS:
 
A -> Normal compiling works perfectly but when compiled like this "gcc -o shell shell.c -Wall -Werror -O", the path behaves improperly.
B -> Multiple files in batch mode.
C -> Redirection for built-in commands.
D -> cd built-in not working in parallel execution("cd test & ls", ls shows the contents of home directory and cd command don't execute at all ).
