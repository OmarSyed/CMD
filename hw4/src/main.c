#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#define COLOR
#include "sfish.h"
#include "debug.h"
#include "helper.h"



int main(int argc, char *argv[], char* envp[]) {
    char * color = KNRM;
    char* input; // cmd input
    char * cur_dir = ""; //holds the current directory
    char * last_dir = NULL; //holds the last directory
    bool exited = false;
    int i;
    if(!isatty(STDIN_FILENO)) {
        // If your shell is reading from a piped file
        // Don't have readline write anything to that file.
        // Such as the prompt or "user input"
        if((rl_outstream = fopen("/dev/null", "w")) == NULL){
            perror("Failed trying to open DEVNULL");
            exit(EXIT_FAILURE);
        }
    }

    do {
        char last_letter;
        int counter = 0; //used to keep track of beginning of buff3
        char * buff3 = getenv("HOME"); // holds the path to home directory
        cur_dir = get_cur_dir(cur_dir); // get current working directory
        char * id = " :: ossyed >> ";
        char * new_str = strcat(cur_dir, id); //concatenate buff with id

        if (strstr(new_str, buff3) != NULL){ //turn home directory to ~
            while(*(buff3) != '\0'){
                new_str++;
                buff3++;
                counter++;
            }
            last_letter = *(new_str-1);
            *(new_str-1) = '~';
        }
        char * color_string = NULL;
        color_string = calloc(strlen(color)+14+strlen(new_str-1),sizeof(char));
        color_string = strcat(color_string, color);
        color_string = strcat(color_string, new_str-1);
        color_string = strcat(color_string, KNRM);
        input = readline(color_string);
        split(input, ' ');
        char * bi = get_builtin(); // builtin
        char * fa = get_first_arg(); // first arg
       /* write(1, "\e[s", strlen("\e[s"));
        write(1, "\e[20;10H", strlen("\e[20;10H"));
        //write(1, "SomeText", strlen("SomeText"));
        write(1, "\e[u", strlen("\e[u"));
*/
        // If EOF is read (aka ^D) readline returns NULL
        if(input == NULL) {
            continue;
        }
        if (bi == NULL)
            ;
        else if (strcmp(bi, "help") == 0 && (contains_left_hoinky() == 0 && contains_right_hoinky() == 0)){ // check if first argument is help
            help();
        }
        else if (strcmp(bi, "cd") == 0 && (contains_left_hoinky() == 0 && contains_right_hoinky() == 0)){
            int success = chdir(fa);
            if (fa == NULL){
                success = chdir(getenv("HOME"));
                if (success == -1)
                    printf(BUILTIN_ERROR, bi);
                else{
                    last_dir = realloc(last_dir, strlen(cur_dir));
                    last_dir = strcpy(last_dir, cur_dir); //copy current working directory
                    last_dir[counter-1] = last_letter; //change ~ back to original letter
                    last_dir[strlen(cur_dir)-14] = 0; //get rid of id portion
                }
            }
            else{
                if (strcmp(fa,".")==0){
                    last_dir = realloc(last_dir,strlen(cur_dir));
                    last_dir = strcpy(last_dir, cur_dir); //copy current working directory
                    last_dir[counter-1] = last_letter; //change ~ back to original letter
                    last_dir[strlen(cur_dir)-14] = 0;
                }
                else if (strcmp("..", fa) == 0){
                    last_dir = realloc(last_dir,strlen(cur_dir));
                    last_dir = strcpy(last_dir, cur_dir); //copy current working directory
                    last_dir[counter-1] = last_letter;
                    last_dir[strlen(cur_dir)-14] = 0;
                    if (success == -1)
                       printf(BUILTIN_ERROR, bi);
                }
                else if (strcmp("-", fa) == 0){
                    success = chdir(last_dir);
                    if (success == -1)
                        printf(BUILTIN_ERROR, bi);
                    else{
                        last_dir = realloc(last_dir,strlen(cur_dir));
                        last_dir = strcpy(last_dir, cur_dir); //copy current working directory
                        last_dir[counter-1] = last_letter;
                        last_dir[strlen(cur_dir)-14] = 0;
                    }
                }
                else{ //if it isn't any of the previous args, check if the pathname is successful
                    if (success == -1)
                        printf(BUILTIN_ERROR, bi);
                    else{
                        last_dir = realloc(last_dir,strlen(cur_dir));
                        last_dir = strcpy(last_dir, cur_dir); //copy current working directory
                        last_dir[counter-1] = last_letter;
                        last_dir[strlen(cur_dir)-14] = 0;
                    }
                }
            }
        }
        else if (strcmp(bi, "pwd") == 0 && (contains_left_hoinky() == 0 && contains_right_hoinky() == 0)){
            cur_dir[strlen(cur_dir)-13] = 0;
            cur_dir[counter -1] = last_letter;
            printf("%s\n", cur_dir);
        }
        else if (strcmp(bi, "exit") == 0)
            goto exit;
        else if (strcmp(bi, "color") == 0){
            if (fa == NULL)
                printf(BUILTIN_ERROR, "color");
            else if (strcmp(fa, "RED") == 0)
                color = KRED;
            else if (strcmp(fa, "BLU") == 0)
                color = KBLU;
            else if (strcmp(fa, "GRN") == 0)
                color = KGRN;
            else if (strcmp(fa, "YEL") == 0)
                color = KYEL;
            else if (strcmp(fa, "MAG") == 0)
                color = KMAG;
            else if (strcmp(fa, "CYN") == 0)
                color = KCYN;
            else if (strcmp(fa, "WHT") == 0)
                color = KWHT;
            else if (strcmp(fa, "BWN") == 0)
                color = KBWN;
            else if (strcmp(fa, "NRM") == 0)
                color = KNRM;
            else
                printf("Not a valid color.\n");
        }
        else if (is_builtin(bi) == 0 || contains_left_hoinky() == 0 || contains_right_hoinky() == 0){
            int state;
            pid_t pid = fork();
            if (pid == 0){
                if (contains_right_hoinky() == 1 && contains_left_hoinky() == 1){
                    int inp_file = open(input_split[get_left_hoinky_index() + 1], O_RDONLY);
                    int out_file = open(input_split[get_right_hoinky_index() + 1], O_WRONLY | O_TRUNC | O_CREAT,  S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                    dup2(inp_file, STDIN_FILENO);
                    dup2(out_file, STDOUT_FILENO);
                    if (strcmp(bi, "pwd") == 0){
                        cur_dir[strlen(cur_dir)-13] = 0;
                        cur_dir[counter -1] = last_letter;
                        printf("%s\n", cur_dir);
                        close(out_file);
                        close(inp_file);
                        exit(0);
                    }
                    else if (strcmp(bi, "help") == 0){
                        printf("Type help to display this menu\n");
                        printf("%-19s\t%-19s\n%-19s\t%-19s\n%-19s\t%-19s\n%-19s\t%-19s\n", "help [name]", "Displays list of command line functions","exit", "Exits the command line", "cd [pathname]", "changes current directory to requested pathname", "pwd", "Prints the absolute path of current directory");
                        close(out_file);
                        close(inp_file);
                        exit(0);
                    }
                    else{
                        char ** argv2;
                        if (get_left_hoinky_index() < get_right_hoinky_index()){
                            argv2 = calloc(get_left_hoinky_index()+1, sizeof(char *));
                            for (i = 0; i < get_left_hoinky_index(); i++)
                                argv2[i] = input_split[i];

                        }
                        else{
                            argv2 = calloc(get_right_hoinky_index()+1, sizeof(char *));
                            for (i = 0; i < get_right_hoinky_index(); i++)
                                argv2[i] = input_split[i];
                        }
                        argv2[i] = NULL;
                        execvp(argv2[0], argv2);
                        close(inp_file);
                        close(out_file);
                        free(argv2);
                    }
                }
                else if (contains_left_hoinky() == 1){
                    int inp_file = open(input_split[get_left_hoinky_index()+1], O_RDONLY);
                    if (inp_file < 0){
                        printf(SYNTAX_ERROR, "File not found");
                        exit(0);
                    }
                    int dup = dup2(inp_file, STDIN_FILENO);
                    if (dup < 0)
                        printf("%d\n", dup);
                    // check if the command is a builtin

                    //make a new truncated array
                    char ** argv2 = calloc(get_left_hoinky_index()+1, sizeof(char *));
                    for (i = 0; i < get_left_hoinky_index(); i++){
                            argv2[i] = input_split[i];
                    }
                    argv2[i] = NULL;
                    int status = execvp(argv2[0], argv2);
                    if (status == -1){ //if execution failed then print error
                        printf(EXEC_NOT_FOUND, bi);
                        exit(0);
                    }
                    free(argv2);
                    close(inp_file);
                }
                else if (contains_right_hoinky() == 1){
                    int out_file = open(input_split[get_right_hoinky_index()+1], O_WRONLY | O_TRUNC | O_CREAT,  S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

                    if (out_file < 0){
                        printf(SYNTAX_ERROR, "Unable to open file.");
                        printf("%d\n", errno);
                        exit(0);
                    }
                    int dup = dup2(out_file, STDOUT_FILENO);
                    if (dup < 0)
                        printf("%d\n", dup);
                    // check whether the command is either pwd or help
                    if (strcmp(bi, "pwd") == 0){
                        cur_dir[strlen(cur_dir)-13] = 0;
                        cur_dir[counter -1] = last_letter;
                        printf("%s\n", cur_dir);
                        close(out_file);
                        exit(0);
                    }
                    else if (strcmp(bi, "help") == 0){
                        printf("Type help to display this menu\n");
                        printf("%-19s\t%-19s\n%-19s\t%-19s\n%-19s\t%-19s\n%-19s\t%-19s\n", "help [name]", "Displays list of command line functions","exit", "Exits the command line", "cd [pathname]", "changes current directory to requested pathname", "pwd", "Prints the absolute path of current directory");
                        close(out_file);
                        exit(0);
                    }
                    else{
                        //make new truncated array
                        char ** argv2 = calloc(get_right_hoinky_index()+1, sizeof(char *)); //copy everything before hoinky
                        for (i = 0; i < get_right_hoinky_index(); i++){
                            argv2[i] = input_split[i];
                            debug("%s", argv2[i]);
                        }
                        argv2[i] = NULL;
                        int status = execvp(argv2[0], argv2);
                        if (status == -1){ //if execution failed then print error
                            printf(EXEC_NOT_FOUND, bi);
                            exit(0);
                        }
                        close(out_file);
                        free(argv2);
                    }

                }
                else{
                    //check the execution
                    int status = execvp(bi, input_split);
                    if (status == -1){
                        printf(EXEC_ERROR, "bi");
                        exit(0);
                    }
                }
            }
            else if (pid == -1){
                printf(EXEC_ERROR , bi);
            }
            else
                wait(&state);

        }
        else if (find_pipe() == 1){
            printf(SYNTAX_ERROR, "Piping");
        }
        else
            printf(EXEC_NOT_FOUND, input);
        // You should change exit to a "builtin" for your hw.
        exit:
        if (bi == NULL)
            ;
        else
            exited = strcmp(bi, "exit") == 0;
        // Readline mallocs the space for input. You must free it.
        free(input_split);
        free(cur_dir);
        free(color_string);
        rl_free(input);
    } while(!exited);

    debug("%s", "user entered 'exit'");
    free(last_dir);
    return EXIT_SUCCESS;
}
