#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAXSIZE 80


void commandreader(char *args[], int *ampersand, int *argv){
    char cmd[MAXSIZE];
    int size = 0;
    char delimiter[] = " ";

    length = read(STDIN_FILENO, cmd, 80);

    if (cmd[size-1] == '\n'){
        cmd[size-1] = '\0';
    }

    if (strcmp(cmd, "!!") == 0){
        if (*argv == 0){
            printf("History is Isreal\n");
        }
        return;
    }
    *argv = 0;
    *ampersand = 0;
    char *pointer = strtok(cmd, delimiter);
    
    while (pointer != NULL){
        if (pointer[0] == "&"){
            *ampersand = 1;
            poitner = strtok(NULL, delimiter);
            continue;
        }
        *argv += 1;
        args[*argv - 1] = strdup(pointer);
        pointer = strtok(NULL, delimiter);

    }
    args[*argv] = NULL;

}


int main(void){
    char *args[MAXSIZE];
    int keeprolling = 1;
    pid_t process_num;
    int ampersand = 0;
    int argv = 0;
    int pipeflag = 0;
    while (keeprolling) {
        pipeflag = 0;
        printf("osh>");
        fflush(stdout);
        commandreader(args, &ampersand, &argv);
        process_num = fork();
        if (process_num == 0) {
            if (argv == 0){
                continue;
            }
            else
            {
                int redirect = 0;
                int file;
                for (int i = 1; i <= argv -1; i++){
                    if (strcmp(args[i],"<")== 0){
                         file = open(args[i + 1], O_RDONLY);
                        if (file == -1 || args[i+1]  == NULL) {
                            printf("Invalid Command!\n");
                            exit(1);
                        }
                        dup2(file, STDIN_FILENO);
                        args[i] = NULL;
                        args[i + 1] = NULL;
                        redirect = 1;
                        break;
                    }
                    else if (strcmp(args[i], ">") == 0){
                        file = open(args[i + 1], O_WRONLY | O_CREAT, 0644);
                        if (file == -1 || args[i+1]  == NULL) {
                            printf("Invalid Command!\n");
                            exit(1);
                        }
                        dup2(file, STDOUT_FILENO);
                        args[i] = NULL;
                        args[i + 1] = NULL;
                        redirect = 2;
                        break;
                    }
                    else if (strcmp(args[i], "|") == 0) {
                        pipeflag = 1;

                        int fd1[2];

                        if (pipe(fd1) == -1) {
                            fprintf(stderr, "Pipe Failed\n");
                            return 1;
                        }
                        
                        char *cmd1[i + 1];
                        char *cmd2[argv - i];
                        for (int j = 0; j < i; j++) {
                            cmd1[j] = args[j];
                        }
                        cmd1[i] = NULL;
                        for (int j = 0; j < argv - i - 1; j++) {
                            cmd2[j] = args[j + i + 1];

                        }
                        cmd2[argv - i - 1] = NULL;

                        int pipeID = fork();
                        if (pipeID > 0) {
                            wait(NULL);
                            close(fd1[1]);
                            dup2(fd1[0], STDIN_FILENO);
                            close(fd1[0]);
                            if (execvp(cmd2[0], cmd2) == -1) {
                                printf("Invalid Command!\n");
                                return 1;
                            }

                        } else if (pipeID == 0) {

                            close(fd1[0]);
                            dup2(fd1[1], STDOUT_FILENO);
                            close(fd1[1]);
                            if (execvp(cmd1[0], cmd1) == -1) {
                                printf("Invalid Command!\n");
                                return 1;
                            }
                            exit(1);
                        }
                        close(fd1[0]);
                        close(fd1[1]);
                        break;
                    }
                }

            if (pipeflag == 0) {
                    if (execvp(args[0], args) == -1) {
                        printf("Invalid Command!\n");
                        return 1;
                    }
                }
                if (redirect == 1) {
                    close(STDIN_FILENO);

                } else if (redirect == 2) {
                    close(STDOUT_FILENO);
                }
                close(file);

            }
            exit(1);
        }
        else if (process_num > 0) {
            if (ampersand == 0) wait(NULL);
        } 
        else {
            printf("Error fork!!");
        }
    }

}