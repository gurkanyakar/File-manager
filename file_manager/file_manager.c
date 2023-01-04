// file_manager.c

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define MAX_THREADS 5
#define MAX_FILES 10
#define BUFFER_SIZE 1024

pthread_t thread_list[MAX_THREADS];
char file_list[MAX_FILES][BUFFER_SIZE];

pthread_mutex_t mutex; // mutex lock variable
void create_file(char* file_name)
{
    // find the first empty slot in the file_list array
    int index = -1;
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (strlen(file_list[i]) == 0)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        // file_list array is full, return an error
        return;
    }

    // write the file name to the file_list array
    strcpy(file_list[index], file_name);

    // create the file in the file system
    FILE* fp = fopen(file_name, "w");
    fclose(fp);
}

void delete_file(char* file_name)
{
    // find the file in the file_list array
    int index = -1;
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (strcmp(file_list[i], file_name) == 0)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        // file not found, return an error
        return;
    }

    // delete the file from the file system
    remove(file_name);

    // clear the file name from the file_list array
    strcpy(file_list[index], "");
}

void read_file(char* file_name)
{
    // find the file in the file_list array
    int index = -1;
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (strcmp(file_list[i], file_name) == 0)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        // file not found, return an error
        return;
    }

    // read the file from the file system
    FILE* fp = fopen(file_name, "r");
    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
    {
        printf("%s", buffer);
    }
    fclose(fp);
}

void write_file(char* file_name, char* text)
{
    // find the file in the file_list array
    int index = -1;
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (strcmp(file_list[i], file_name) == 0)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        // file not found, return an error
        return;
    }

    // write the file to the file system
    FILE* fp = fopen(file_name, "a");
    fprintf(fp, "%s", text);
    fclose(fp);
}


void *listen_pipe()
{
    int fd1;
    int fd2;

    char *myfifo = "/tmp/my_named_pipe"; // name of the named pipe

    mkfifo(myfifo, 0666); // create the named pipe

    char text[80];
    char success[80] = "successful command";

    while (1)
    {
        fd1 = open(myfifo, O_RDONLY); // open the named pipe
        
        // wait for a message from file_client
        pthread_mutex_lock(&mutex); // acquire the mutex lock
        read(fd1, text, sizeof(text));
        printf("Received: %s", text); // print the message

        char *command;
        char *arguments;
        command = strtok(text, " "); // parse the received message into tokens
        arguments = strtok(NULL, " "); // get the arguments for the command
        
        if(strcmp(command, "Create") == 0)
        {
            printf("Creating file\n");
            create_file(arguments); // call create_file with the arguments
        }
        else if(strcmp(command, "Delete") == 0)
        {
            printf("Deleting file\n");
            delete_file(arguments); // call delete_file with the arguments
        }
        else if(strcmp(command, "Read") == 0)
        {
            printf("Reading file\n");
            read_file(arguments); // call read_file with the arguments
        }
        else if(strcmp(command, "Write") == 0)
        {
            printf("Writing to file\n");
            write_file(arguments); // call write_file with the arguments
        }
        else if(strcmp(command, "Exit") == 0)
        {
            printf("Exiting\n");
            break; // exit the loop
        }
        close(fd1);
        fd2 = open(myfifo, O_WRONLY); // open the named pipe
        pthread_mutex_unlock(&mutex); // release the mutex lock

        // send a response to file_client
        write(fd2, success, strlen(success) + 1);

        close(fd2);
    }

    return NULL;
}


int main()
{
    pthread_t tcb;
    pthread_create(&tcb, NULL, listen_pipe, NULL);
    pthread_mutex_init(&mutex, NULL); // initialize the mutex lock
    char input[80];

    while (1)
    {
        fgets(input, 80, stdin);

        if (strcmp(input, "exit\n") == 0) // exit command no argument
        {
            exit(0);
        }else{
            printf("Invalid command\n");
        }
    }
    pthread_join(tcb, NULL); // wait for the thread to finish
    

    pthread_mutex_destroy(&mutex); // destroy the mutex lock
    return 0;
}