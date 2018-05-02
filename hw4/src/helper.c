#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <wait.h>
#include "sfish.h"



char ** input_split;
int num_args = 0;
int num_token = 0;
void help(){
    printf("Type help to display this menu\n");
    printf("%-19s\t%-19s\n%-19s\t%-19s\n%-19s\t%-19s\n%-19s\t%-19s\n", "help [name]", "Displays list of command line functions","exit", "Exits the command line", "cd [pathname]", "changes current directory to requested pathname", "pwd", "Prints the absolute path of current directory");
}
void func_help(char * str){
    if (strcmp(str, "help") == 0){
        printf("Displays a list of all command line functions or displays description of requested function.\n");
    }
    else if(strcmp(str, "exit") == 0){
        printf("Exits out of the command line.\n");
    }
    else if(strcmp(str, "cd") == 0){
        printf("Change directory to directory contained in current directory.\n");
    }
    else if(strcmp(str, "pwd") == 0){
        printf("Print the absolute path of current directory.\n");
    }
    else{
        printf("Function %s not found.\n", str+6);
    }
}

char ** split(char * input, char c){
    int i, j, length=strlen(input), count = 0;
    input_split = (char **) calloc(1, sizeof(char *));
    for (i = 0; i < length; i++){
        if (input[i] != c){
            j = i;
            while (input[j] != c && j < length)
                j ++;
            input[j] = 0;
            input_split = (char **) realloc(input_split, (count+1)*sizeof(char *));
            input_split[count] = &input[i]; //set pointer to first char of token
            count ++;
            i = j; //skip the next delimiters
        }
    }
    input_split = realloc(input_split, (count+1)*sizeof(char *));
    input_split[count] = NULL;
    num_token = count;
    return input_split;
}


char * get_first_arg(){
    return input_split[1];
}

char * get_builtin(){
   return input_split[0];
}

char * get_cur_dir(char * cur_dir){
    int i = 2;
    errno = 0;
    cur_dir = (char * ) malloc(10); //add an extra 14 for id portion
    cur_dir = getcwd(cur_dir, 10);
    while(errno == ERANGE){
        errno =0;
        cur_dir = (char *)realloc(cur_dir, 10*i);
        getcwd(cur_dir, 10*i);
        i++;
    }
    cur_dir = (char *) realloc(cur_dir, 10*i+15);
    getcwd(cur_dir, 10*i);
    return cur_dir;
}

int is_builtin(char * str){
    if (strcmp(str, "cd") == 0 || strcmp(str, "help") == 0 || strcmp(str, "exit") == 0
        || strcmp(str, "pwd") == 0)
        return 1;
    return 0;
}

int contains_right_hoinky(){
    int i;
    for (i = 0; i < num_token; i++){
        if (strcmp(input_split[i], ">") == 0)
            return 1;
    }
    return 0;
}

int contains_left_hoinky(){
    int i;
    for (i = 0; i < num_token; i++)
        if (strcmp(input_split[i], "<") == 0)
            return 1;
    return 0;
}

int get_left_hoinky_index(){
    int i;
    for (i = 0; i < num_token; i++)
        if (strcmp(input_split[i], "<") == 0)
            return i;
    return -1;
}

int get_right_hoinky_index(){
    int i;
    for (i = 0; i < num_token; i++)
        if (strcmp(input_split[i], ">") == 0)
            return i;
    return -1;
}
int find_pipe(){
    for (int i = 0; i < num_token; i++)
        if (strcmp(input_split[i], "|") == 0)
            return i;
    return -1;
}

int countpipes(char ** argv, int num_token){
    int i ;
    int count = 0;
    for (i = 0; i < num_token; i++){
        if (strcmp(argv[i], "|") == 0)
            count ++;
    }
    return count;
}
void pipeline(char ** argv){
    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if (pid == 0){

    }
    else if (pid == -1){

    }
    else{

    }

}

int find_next_pipe(char ** input_split){
    int i = 0;
    int j = 0;
    int k = 0;
    while (input_split[i] != NULL){
        if (strcmp(input_split[i], "|") == 0){
            int fd[2];
            int state;
            pipe(fd);
            pid_t pid = fork();

            if (pid == 0){
                char ** argv2 = calloc(j+1, sizeof(char *));
                for (k = i - j; k < j; k++){
                    argv2[k] = input_split[k];
                }
                argv2[k] = NULL;
                dup2(fd[0], STDIN_FILENO); //
                dup2(fd[1], STDOUT_FILENO);
                execvp(argv2[0], argv2);
            }
            else if (pid == -1){
                printf(SYNTAX_ERROR, "Piping");
                return 0;
            }
            else
                wait(&state);
            j = -1;
        }
        i++;
        j++;
    }
    return 1;
}