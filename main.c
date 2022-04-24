#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include "tcp.h"
#define PORT 50000

// my system implemented with fork and exec and wait
int my_system(const char *command)
{
    // creating the child process
    pid_t child;
    child = fork();
    if (child == -1)
    {
        perror("");
    }
    // executing the command if you are the child process
    else if (child == 0)
    {
        execl("/bin/sh", "sh", "-c", command, (char *)NULL);
        exit(700);
    }
    // wait for the child process to finish
    else
    {
        wait(NULL);
    }

    return 0;
}
int main(int argc, char const *argv[])
{
    int sock = -1;
    int saved_stdout = dup(1);
    while (1)
    {
        printf("%s$ ", getcwd((char *)(NULL), (size_t)(NULL)));
        // printf("Please enter a command:");
        //  command pointer
        char *command = NULL;
        // size of the command
        size_t len = 0;
        // size of the chars we got from stdin
        ssize_t lineSize = 0;
        lineSize = getline(&command, &len, stdin);
        command[lineSize - 1] = '\0';
        // printf("You entered %s, which has %zu chars.\n", command, lineSize - 1);
        //  command is exit break
        
        // library call
        if (strcmp("EXIT", command) == 0)
        {
            break;
        }
        // if command is getcwd print the current directory
        // library call
        else if (strcmp("getcwd", command) == 0)
        {
            // print current dir full path
            char buff[256] = {0};
            if (getcwd(buff, sizeof(buff)))
            {
                printf("Current working dir: %s\n", buff);
            }
            else
            {
                perror("getcwd() error");
            }
        }
        // prints whatever it gets
        // library function
        else if (strncmp("ECHO", command, 4) == 0)
        {
            for (int i = 5; i < lineSize; i++)
            {
                putchar(command[i]);
            }
            putchar('\n');
        }
        // prints the content of the directory
        // library function
        else if (strcmp("DIR", command) == 0)
        {
            DIR *d;
            struct dirent *dir;
            d = opendir(".");
            if (d)
            {
                while ((dir = readdir(d)) != NULL)
                {
                    printf("%s\n", dir->d_name);
                }
                closedir(d);
            }
        }
        // go into the given folder
        // system call
        else if (strncmp("CD", command, 2) == 0 || strncmp("cd", command, 2)==0)
        {
            char buff[256] = {0};
            if (chdir(strcpy(buff, command + 3)) == -1)
            {
                perror("");
            }
        }
        // copy a file from one directory to another
        // library call
        else if (strncmp("COPY", command, 4) == 0)
        {
            char *cmd = strtok(command, " ");
            char *src = strtok(NULL, " ");
            char *dest = strtok(NULL, " ");
            if (cmd == NULL || src == NULL || dest == NULL)
            {
                printf("no source or destination");
            }
            else
            {
                FILE *src_file = fopen(src, "rb");
                if (src_file)
                {
                    FILE *dest_file = fopen(dest, "wb");
                    if (dest_file)
                    {
                        fseek(src_file, 0, SEEK_END);
                        long length = ftell(src_file);
                        fseek(src_file, 0, SEEK_SET);
                        char *buff = (char *)malloc(length * sizeof(char));
                        if (buff == NULL)
                        {
                            printf("could not malloc");
                        }
                        else if (fread(buff, sizeof(char), length, src_file) == -1)
                        {
                            printf("could not read from the file:%s", src);
                        }
                        else if (fwrite(buff, sizeof(char), length, dest_file) == -1)
                        {
                            printf("could not write to file:%s", dest);
                        }
                        fclose(src_file);
                        fclose(dest_file);
                        free(buff);
                    }
                    else
                    {
                        perror("");
                    }
                }
                else
                {
                    perror("");
                }
            }
        }
        // here we use System calls
        else if (strncmp("DELETE", command, 6) == 0)
        {
            char buff[256] = {0};
            if (unlink(strcpy(buff, command + 7)) == -1)
            {
                perror("");
            }
        }
        // connects to a given port
        // system call
        else if (strncmp("TCP PORT", command, 8) == 0)
        {
           sock = create_socket(PORT);
            
        }
        // disconnect from the given port
        // system call
        else if (strcmp("LOCAL", command) == 0)
        {
            if (sock == -1)
            {
                printf("socket is not open");
            }
            else
            {
                local(sock, saved_stdout);
            }
        }
        // execute a command using my_system
        else
        {
            //system(command)
            my_system(command);
        }

        free(command);
    }
    return 0;
}
