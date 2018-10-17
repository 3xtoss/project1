/*
name: Jared Mulhausen
project: 1
date: 9/27/18
*/

//required header files
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <libgen.h>
#include <dirent.h>
#include <errno.h>


/*
defined macros
*/

#define MAX_BUFFER 1024         //max character for line
#define MAX_ARGS 64             //max arguments
#define SEPARATORS " \t\n"      //seperates tokens


//global variables
extern char ** environment;


//global functions
int checkDirectory(char * directory);
int checkFile(char * filePath);
int isDirectory(char * path);
void open_read_me();
void prompt();
void parse(char * readLine, char ** arguments);
int movePath(char * startingPath, char * destinationPath);
int copyFile(char * source, char * destination);
int execute(char ** arguments);


/*
@function: checkDirectory
@params:   char * directoryPath
@return:    bool
*/
void open_read_me() {
  FILE *fp;
  int c;
  fp = fopen("README.txt", "r");

  if (fp) {
    while((c = getc(fp)) != EOF) {
      putchar(c);
    }
    fclose(fp);
  }
}

/*
@function: checkDirectory
@params:   char * directoryPath
@return:    bool
*/
int checkDirectory(char * directoryPath) {
  DIR* dir;
  dir = opendir(directoryPath);
  if (dir) { //directoryPath exists
    closedir(dir);
    return 1;
  }
  else if (ENOENT == errno) { //directoryPath does not exist
    return 0;
  }
  else {                      //error opendir
    fprintf(stderr, "ERROR WITH opendir()");
    exit(-1);
  }
}


/*
@function: checkFile
@params:   char * filePath
@return:   bool
@description: returns true or false if a given filepath is a file.
*/

int checkFile(char * filePath) {
  FILE * file;

  if (file = fopen(filePath, "r"))
  { //file exists
        fclose(file);
        return 1;
  }
  else if (ENOENT == errno)
  { //file does not exist
        return 0;
  }
  else
  {                      //error fopen
        fprintf(stderr, "ERROR WITH fopen()");
        exit(-1);
  }
}

/*
@function:    isDirectory
@params:      char * path
@return:      bool
*/

int isDirectory(char * path) {
  struct stat statbuf;

  if(stat(path, &statbuf) != 0)
  {
    return 0;
  }
  return 1;
}

/*
@function:    prompt
@params:
@return:      void
@description: Helpful prompt command that will continually update the working
              environment.
*/

void prompt()  {
  printf("==>");
  //user name
  char * user;
  //current working directory
  char directory[256];

  //get the user name
  user = getenv("USER");
  getcwd(directory, sizeof(directory));
}

/*
@function:      parse
@params:        char * readLine, char ** commands
@return:        void
@description:   Parses a line of user input and creates commands
                to be executed.
*/

void parse(char * readLine, char ** commands) {
  while (*readLine != '\0')         //while the character is vaild
{
      while (*readLine == ' ' || *readLine == '\t' || *readLine == '\n') //check for SEPARATORS
      {
        *readLine++ = '\0';         //end string
      }
      *commands++ = readLine;      //increment commands[]

      while (*readLine != '\0' && *readLine != ' ' && *readLine != '\t' && *readLine != '\n') //continues reading the command
      {
        readLine++;
      }
  }
    * commands = NULL;            //error parsing command line.
}

/*
@function:      execute
@params:        commands
@return:        bool
@description:   forks child and parent processes to system.
*/

int execute(char ** commands) {
  //pid_t is a special type, help with processing
  pid_t branch, wait;
  //helper variable.
  int path;

  //create a process ---
  switch(branch = fork())
  {
      case -1:          //failure fork(), handle error
        perror("fork");
        exit(1);
      case 0:
        if (execvp(commands[0], commands) == -1) //executes the commands, unless----, error.
        {
          perror("exec"); //system could not execute the external command --- damn.
          exit(1);        //exit the program.
        }
      default:
        do
        {
          wait = waitpid(branch, &path, WUNTRACED);
        } while(!WIFEXITED(path) && !WIFSIGNALED(path));
    }
    return 1;
}

/*
          @function:     movePath
          @params:       char * startingPath, char * destinationPath
          @return:       bool
          @desciption:   implements are morph command by moving files
                         using fopen, fclose, and rename.
*/

int movePath(char * startingPath, char * destinationPath) {

  //temporary variables
  //help with checks.
  FILE * firstFile;
  FILE * secondFile;

  //checks if the file exists in the startingPath and checks
  //if a file doesnt exist in the same path.
  //this condition would rename the existing files
  //to the current path.

  if((firstFile = fopen(startingPath, "r")) && ((secondFile = fopen(destinationPath, "r")) == NULL))
  {
    //returns 0 if the rename is possible
    int success = rename(startingPath, destinationPath);
    if (success == 0)
    {

      //successful move
      //close files
      fclose(firstFile);
      fclose(secondFile);
      return 1;
    }
    //error case, error moving path
    else
    {
      fprintf(stderr, "error moving path");
      return 0;
    }
  }

  else if ((isDirectory(startingPath)) && (!isDirectory(destinationPath))) //move directory to an empty directory
  {
    //int success = rename(startingPath, destinationPath);

    //shotty implementation of string copying and concat
    //if it overflows, we will atleast report and error.

    char * changePath = NULL;
    strcpy(changePath, destinationPath);
    strcat(changePath, "/");
    strcat(changePath, startingPath);
    printf("%s", changePath);

    int success = rename(startingPath, changePath); //error handling
    if (success == 0)
    {
      return 1;
    }
    else
    {
      fprintf(stderr, "error in moving directory\n");
      return 0;
    }
  }

  //moving a directory to another directory
  else if ((isDirectory(startingPath)) && (isDirectory(destinationPath)))
  {
    //creating a new destination Path by cat
    strcat(destinationPath, "/");
    strcat(destinationPath, startingPath);

    //error handling
    int success = rename(startingPath, destinationPath);
    if (success == 0)
    {
      return 1;
    }
    else
    {
      fprintf(stderr, "error moving directory");
      return 0;
    }
  }

  //moving a file to a directory
  else if((firstFile = fopen(startingPath, "r")) && (isDirectory(destinationPath)))
  {
    //more bad handling in c, however, it will handle the overflow error.
    strcat(destinationPath, "/");
    strcat(destinationPath, startingPath);

    //bool condition
    int success = rename(startingPath, destinationPath);
    if (success == 0)
    {
      return 1;
    }
    else
    {
      fprintf(stderr, "morph error\n");
      return 0;
    }
  }
  //handles wrong arguments
  else
  {
    fprintf(stderr, "wrong arguments given:");
    return 0;
  }

  //something terrible happened, exit program.
  return -1;
}
/*
@Function: Implements the mimic function, moves [src] to [dst]
@Params:  char * source, char * destination
@Return:   int
*/
int copyFile(char * source, char * destination) {
  //helpful for copying string
  size_t bytes;
  //temporary variable for copying strings
  char buffer[MAX_BUFFER];
  //handles file input
  FILE * input;
  //handles file output
  FILE * output;


  //requires source to be a valid argument,
  //this function will not be able to handle copying directory to directory
  if (checkFile(source)) {

    //copy the source directly inside the same directory.
    //checks if both source and destination are file format.
    if((input = fopen(source, "r")) && (output = fopen(destination, "w")))
    {
      //reading the bytes from the input until the end.
      while((bytes = fread(buffer, 1, MAX_BUFFER, input) != 0))
      {
            //if it doesnt write anything.
            if(fwrite(buffer, 1, strlen(buffer) ,output) == 0)
            {
              fprintf(stderr, "error with fwrite()\n");
              return 0;
            }
            //successful write.. close files.
            else
            {
              fclose(input);
              fclose(output);
              return 1;
            }
        }
    }

    //copying file to directory
    //checks if file is a file, and checks if the directory is valid.
    else if((input = fopen(source, "r")) && (checkDirectory(destination))) {

      //sprintf will save the string from destination and source
      //to the buffer.
      sprintf(buffer, "%s/%s", destination, basename(source));

      //check if you can open the new filepath
      if((output = fopen(buffer, "w")))
      {
        //if you cannot write to the new path, error------
        if(fwrite(buffer, 1, strlen(buffer), output) == 0)
        {
          fprintf(stderr, "cannot write to output file --- \n");
          return 0;
        }

        //successful copy condition.
        else
        {
          fclose(input);
          fclose(output);
          return 1;
        }
      }
    }
    //error handler,
    //invalid arguments are given to the copyFile function.
    else
    {
      fprintf(stderr, "invalid arguments\n");
      return 0;
    }
  }
  //error handling,
  //source is not a file.
  else {
    fprintf(stderr, "source is not a file ---\n");
    return 0;
  }
  return 0; //error message
}


/*
@function: shell
@params: char * filePath, char ** environment
@return: bool
@description: terrible implementation that I added last minute to handle
file input. Basically run the same loop to process commands as my main.
*/
void shell(FILE * filePath, char ** environment) {
  char buffer[MAX_BUFFER];
  char commandBuffer[MAX_BUFFER];
  char * arguments[MAX_ARGS];
  char ** argument;

  while (!feof(filePath)) {
    //prompt();
    fflush(stdout);

    if(fgets(buffer, MAX_BUFFER, filePath)) {
      argument = arguments;
      *argument++ = strtok(buffer, SEPARATORS);
      printf("==>");
      while((*argument++ = strtok(NULL, SEPARATORS)));

      //printf("\n");

      if(arguments[0]) {

        for(int i = 0; arguments[i] != NULL; i++) {
          printf("%s ", arguments[i]);
        }
        printf("\n");

        if(!strcmp(arguments[0], "wipe")) {
          strcpy(commandBuffer, "clear");
        }
        else if(!strcmp(arguments[0], "esc")) {
          exit(-1);
        }
        else if(!strcmp(arguments[0], "environ")) {
          char ** env;
          for(env = environment; *env != 0; env++) {
            char * currentEnv = * env;
            printf("%s\n", currentEnv);
          }
        }
        else if(!strcmp(arguments[0], "ditto")) {
            char ** comments;
            comments = arguments;
            comments++;
            for(comments = comments; *comments != 0; comments++) {
              char * currentComment = * comments;
              printf("%s ", currentComment);
            }
            printf("\n");
            continue;
        }
        else if(!strcmp(arguments[0], "filez")) {
          strcpy(commandBuffer, "ls -l");
          if(!arguments[1]) {
            arguments[1] = ".";
          }
          else {
            strcat(commandBuffer, " ");
            strcat(commandBuffer, arguments[1]);
          }
        }

        else if(!strcmp(arguments[0], "help")) {
          //strcpy(commandBuffer, "clear");
          open_read_me();
        }

        else if(!strcmp(arguments[0], "mimic")) {
          if(arguments[1] && arguments[2]) {
            int temp = copyFile(arguments[1], arguments[2]);
            if(temp == -1) {
              perror("copyFile");
            }
          }
          continue;
        }
        else if(!strcmp(arguments[0], "erase")) {
          if(!arguments[1]) {
            fprintf(stderr, "NO FILE SPECIFIED\n");
          }
          else {
            int success = remove(arguments[1]);
            if(success == 0) {
            }
            else {
              fprintf(stderr, "[myfile] does not exist\n");
            }
          }
          //continue;
        }
        else if(!strcmp(arguments[0], "morph")) {
          if(arguments[1] && arguments[2]) {
            int temp = movePath(arguments[1], arguments[2]);
            if (temp == 0) {
              perror("movePath:");
            }
          }
          continue;
        }
        else if(!strcmp(arguments[0], "chdir")) {
          char cwd[256];
          if (arguments[1] && chdir(arguments[1]) == 0) {
            setenv("PWD",getcwd(cwd, sizeof(cwd)), 1);
            continue;
          }
          else if(!arguments[1]){
            strcpy(commandBuffer, "ls -l");
          }
        }
        else  {
          strcpy(commandBuffer, arguments[0]);
          char ** readCommand = arguments;
          readCommand++;
          while(*readCommand != 0) {
            strcat(commandBuffer, " ");
            strcat(commandBuffer, *readCommand++);
          }
          //const char * systemCall = commandBuffer;
          //system(systemCall);
          //fflush(stdout);
          //continue;
        }

        if (commandBuffer[0]) {
          char * commandArguments[MAX_ARGS];
          parse(commandBuffer, commandArguments);
          execute(commandArguments);
          //fflush(stdout);
        }
      }
    }
  }
  return;
}
int main(int argc, char * argv[], char ** environment) {
  FILE * fp;
  char buffer[MAX_BUFFER];
  char commandBuffer[MAX_BUFFER];
  char * arguments[MAX_ARGS];
  char ** argument;

  if(argv[1]) {
    //printf("success\n");
    if((fp = fopen(argv[1], "r"))) {
      shell(fp, environment);
      fclose(fp);
      exit(-1);
    }
    else {
      fprintf(stderr, "error opening file.");
      printf("\n");
      //error error!!!
      return 0;
    }
  }

  while (!feof(stdin)) {
    prompt();
    fflush(stdout);

    if(fgets(buffer, MAX_BUFFER, stdin)) {
      argument = arguments;
      *argument++ = strtok(buffer, SEPARATORS);

      while((*argument++ = strtok(NULL, SEPARATORS)));

      if(arguments[0]) {
        if(!strcmp(arguments[0], "wipe")) {
          strcpy(commandBuffer, "clear");
        }
        else if(!strcmp(arguments[0], "esc")) {
          exit(-1);
        }
        else if(!strcmp(arguments[0], "environ")) {
          char ** env;
          for(env = environment; *env != 0; env++) {
            char * currentEnv = * env;
            printf("%s\n", currentEnv);
          }
        }
        else if(!strcmp(arguments[0], "ditto")) {
            char ** comments;
            comments = arguments;
            comments++;
            for(comments = comments; *comments != 0; comments++) {
              char * currentComment = * comments;
              printf("%s ", currentComment);
            }
            printf("\n");
            continue;
        }
        else if(!strcmp(arguments[0], "filez")) {
          strcpy(commandBuffer, "ls -l");
          if(!arguments[1]) {
            arguments[1] = ".";
          }
          else {
            strcat(commandBuffer, " ");
            strcat(commandBuffer, arguments[1]);
          }
        }

        else if(!strcmp(arguments[0], "help")) {
          //strcpy(commandBuffer, "clear");
          open_read_me();
          continue;
        }

        else if(!strcmp(arguments[0], "mimic")) {
          if(arguments[1] && arguments[2]) {
            int temp = copyFile(arguments[1], arguments[2]);
            if(temp == -1) {
              perror("copyFile");
            }
          }
          continue;
        }
        else if(!strcmp(arguments[0], "erase")) {
          if(!arguments[1]) {
            fprintf(stderr, "NO FILE SPECIFIED\n");
          }
          else {
            int success = remove(arguments[1]);
            if(success == 0) {
              //printf("success\n");
            }
            else {
              fprintf(stderr, "[myfile] does not exist\n");
            }
          }
          continue;
        }
        else if(!strcmp(arguments[0], "morph")) {
          if(arguments[1] && arguments[2]) {
            int temp = movePath(arguments[1], arguments[2]);
            if (temp == 0) {
              perror("movePath:");
            }
          }
          continue;
        }
        else if(!strcmp(arguments[0], "chdir")) {
          char cwd[256];
          if (arguments[1] && chdir(arguments[1]) == 0) {
            setenv("PWD",getcwd(cwd, sizeof(cwd)), 1);
          }
          else if(!arguments[1]){
            strcpy(commandBuffer, "ls -l");
          }
          continue;
        }
        else  {
          strcpy(commandBuffer, arguments[0]);
          char ** readCommand = arguments;
          readCommand++;
          while(*readCommand != 0) {
            strcat(commandBuffer, " ");
            strcat(commandBuffer, *readCommand++);
          }
          const char * systemCall = commandBuffer;
          system(systemCall);
          fflush(stdout);
          continue;
        }

        if (commandBuffer[0]) {
          char * commandArguments[MAX_ARGS];
          parse(commandBuffer, commandArguments);
          execute(commandArguments);
        }
      }
    }
  }
  return 0;
}

