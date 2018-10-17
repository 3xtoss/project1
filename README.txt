Title: Project 1
Name: Jared Mulhausen
Email: jaredmulhausen@ou.edu
Date: 9/27/18

Description:
Simple shell implemented in c. Supports file or stdin.
Reads file or stdin. Supports internal commands
and external commands!

Simple Solution:
The execute function handles the majority of my
system calls. I did have some issues whenever I realized
that we were supposed to handle FILE and stdin.

The program determines if it was passed file input:
if it does----
      run shell()
      exit()
if it doesn't---
      continue main()
      continue until esc or error

Possible Issues:
Worried about string overflow. Over this project, I realized
how dangerous strcat and strcpy could be. some of the internal
implementations should have been more precise on allocating
space. I realized that some of the errors that I began to receive
would cause the stdout to not be flushed and I would eventually
run into some small overflow bugs in shell and main.

References:
Websites referenced:

makefile and basic shell implementation code:
oudalab.github.io

Websites where I read the documentation for
the libraries I included.

freopen, fopen, fwrite, fread, waitpid, execvp, types.h:
linux.die.net/man/

stat, fgets, fputs:
pubs.opengroup.org


Functions:
int checkDirectory(char * directory);
int checkFile(char * filePath);
int isDirectory(char * path);
void open_read_me();
void prompt();
void parse(char * readLine, char ** arguments);
int movePath(char * startingPath, char * destinationPath);
int copyFile(char * source, char * destination);
int execute(char ** arguments);

Internal Commands:
esc			(exits program)
wipe			(clears the shell)
filez [target]		(displays the current dir, using ls -l [target])
environ			(displays the environment variables)
ditto [comment]		(displays the [comment] on the shell)
help			(displays the README.txt file on the shell)
mimic [src] [dst]	(moves the [src] to a new [dst] --> mv)
erase [myfile]		(erases [myfile] in the working directory)
morph [src] [dst]	(copies the [src] to [dst] --> cp)
chdir [path]		(changes the working directory to [path]
