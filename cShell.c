/*

 Our program implements an interface for a shell by taking in a command from the user. The program then parses that command and executes it in a child process. Checks for the following cases are done before execution:
 
 1. If command entered by the user is “exit” then the program will stop.
 2. If command entered by the user is “history” then the program will print the previous commands entered by the user up to 10 commands. Commands are printed from most recent to least recent.
 3. If command entered by the user is “!!” then the most recent command will be executed. "!!" executes the previous command as long as less than 10 commands have been exected in total.
 4. If command entered by the user is “!” followed by a number of a command from the history (!num) then the corresponding command from the history will be executed.
 */

#include <stdio.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define HISTORY_COUNT 10
int hist_num = 0;

void  tokenize(char *line, char **argv)
{
    while (*line != '\0') { // if not the end of line
        while (*line == ' ' || *line == '\t' || *line == '\n')
            *line++ = '\0';  // replace white spaces with 0
        *argv++ = line;  // save the argument position
        while (*line != '\0' && *line != ' ' &&
               *line != '\t' && *line != '\n')
            line++;
    }
    *argv = '\0';
}

int history(char *hist[], int current)
{
    int i = current - 1;
    
    //prints the history as requested by the project from most recent to least recent
    for (int j = i, h = hist_num; j >= 0; j--, h--) {
        printf("%4d  %s\n", h, (hist[j]));
    }
    return 0;
}

//executes the command
void  execute(char **argv){
    
    pid_t  pid;
    int    status;
    
    if ((pid = fork()) < 0) {     // fork a child process
        printf("*** ERROR: forking child process failed\n");
        exit(1);
    }
    else if (pid == 0) {
        if (execvp(*argv, argv) < 0) { // execute the command
            exit(1);
        }
    }
    else {
        while (wait(&status) != pid);  // wait for completion
    }
}

int  main(void){
    
    char command[1024]; //the input command line
    int shouldRun = 1;
    char  *array[64];
    int i = 0;
    char *hist[HISTORY_COUNT]; //history array to store the commands
    int j, current = 0;
    
    //initialize the history array to null
    for (j = 0; j < HISTORY_COUNT; j++)
        hist[j] = NULL;
    
    while (shouldRun) {
        printf("osh> ");
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0';
        printf("\n");
        
        hist[current] = strdup(command); //store the command in history
        hist_num++;
        
        current = (current + 1) % HISTORY_COUNT; //circular array index
        
        tokenize(command, array); //tokenize the commands
        
        /*
         
         Checks if the first command is ! and the second is !, if so runs the !! command,
         else runs !num command as specified by the user
         
         */
        if(command[0] == '!'){
            if(command[1] == '!'){
                
                if (hist[1] == NULL) { //error handling
                    printf("No commands in history.\n");
                }
                
                else {
                    printf("Previous Command: %s\n", hist[current - 2]);
                    tokenize(hist[current - 2], array);
                }
                
            }
            
            else {
                int index = atoi(command + 1);
                
                //Checking if index exists in history
                if (index < 1 || index > 10 || index >= hist_num)
                    printf("No such command in history.\n");
                
                else {
                    //prints and run the command from the history
                    printf("Command is: %s\n",hist[index - 1]);
                    tokenize(hist[index - 1], array);
                }
            }
        }
        //exit command
        if (strcmp(array[0], "exit") == 0){
            shouldRun = 0;
            printf("Program Sucessfully Exited\n");
            exit(0);
        }
        //runs history
        else if (strcmp(array[0], "history") == 0){
            history(hist, current);
        }
        
        //executes the command
        execute(array);
        i = 0;
    }
    return 0;
}
